#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("UdpClientServer");

int main(int argc, char *argv[])
{
    // -------------------------
    // Command line parsing
    // -------------------------
    CommandLine cmd(__FILE__);
    cmd.Parse(argc, argv);

    // -------------------------
    // Time resolution & Logging
    // -------------------------
    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // -------------------------
    // Nodes
    // -------------------------
    NodeContainer nodes;
    nodes.Create(2);

    // -------------------------
    // Mobility
    // -------------------------
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

    mobility.Install(nodes.Get(0));
    Ptr<ConstantPositionMobilityModel> mob0 = nodes.Get(0)->GetObject<ConstantPositionMobilityModel>();
    mob0->SetPosition(Vector(0.0, 0.0, 0.0));

    mobility.Install(nodes.Get(1));
    Ptr<ConstantPositionMobilityModel> mob1 = nodes.Get(1)->GetObject<ConstantPositionMobilityModel>();
    mob1->SetPosition(Vector(10.0, 0.0, 0.0));

    // -------------------------
    // Point-to-Point link
    // -------------------------
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices = pointToPoint.Install(nodes);

    // -------------------------
    // Internet stack & IP
    // -------------------------
    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // -------------------------
    // UDP Applications
    // -------------------------
    // Node 1: UDP Echo Server (port 9)
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Node 0: UDP Echo Client (send to Node 1 port 9)
    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    // Node 0: UDP Echo Server (port 10)
    UdpEchoServerHelper clientResponseServer(10);
    ApplicationContainer clientServerApps = clientResponseServer.Install(nodes.Get(0));
    clientServerApps.Start(Seconds(2.5));
    clientServerApps.Stop(Seconds(10.0));

    // Node 1: UDP Echo Client (send to Node 0 port 10)
    UdpEchoClientHelper serverResponseClient(interfaces.GetAddress(0), 10);
    serverResponseClient.SetAttribute("MaxPackets", UintegerValue(1));
    serverResponseClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    serverResponseClient.SetAttribute("PacketSize", UintegerValue(512));

    ApplicationContainer serverClientApps = serverResponseClient.Install(nodes.Get(1));
    serverClientApps.Start(Seconds(3.0));
    serverClientApps.Stop(Seconds(10.0));

    // -------------------------
    // Animation (NetAnim) & PCAP
    // -------------------------
    AnimationInterface anim("part1_bidirectional.xml"); // même nom que le fichier .cc
    pointToPoint.EnablePcapAll("part1_bidirectional");

    // -------------------------
    // FlowMonitor (optionnel)
    // -------------------------
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    // -------------------------
    // Forcer la fin de la simulation
    // -------------------------
    Simulator::Stop(Seconds(12.0)); // simulation s'arrête à 12s max
    Simulator::Run();

    // Sauvegarde FlowMonitor
    monitor->SerializeToXmlFile("part1_bidirectional-flow.xml", true, true);

    Simulator::Destroy();
    return 0;
}
