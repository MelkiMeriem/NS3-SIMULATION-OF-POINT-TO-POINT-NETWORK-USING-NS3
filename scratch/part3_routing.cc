#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Part3RoutingExample");

int main(int argc, char *argv[])
{
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(3); // n0, n1, n2

    // Définir les positions des nœuds pour NetAnim
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));    // n0
    positionAlloc->Add(Vector(100.0, 0.0, 0.0));  // n1
    positionAlloc->Add(Vector(200.0, 0.0, 0.0));  // n2
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer dev0dev1 = p2p.Install(NodeContainer(nodes.Get(0), nodes.Get(1)));
    NetDeviceContainer dev1dev2 = p2p.Install(NodeContainer(nodes.Get(1), nodes.Get(2)));

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces0_1 = address.Assign(dev0dev1);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces1_2 = address.Assign(dev1dev2);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Serveur sur n2
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(nodes.Get(2));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Client sur n0 -> n2 via n1
    UdpEchoClientHelper echoClient(interfaces1_2.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(3));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(512));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(10.0));

    // Générer fichiers PCAP et XML pour NetAnim
    p2p.EnablePcapAll("part3_routing");
    AnimationInterface anim("part3_routing.xml");

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
