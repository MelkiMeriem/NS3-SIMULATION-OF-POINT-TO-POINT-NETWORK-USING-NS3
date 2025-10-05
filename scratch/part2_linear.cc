#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Part2LinearExample");

int main(int argc, char *argv[])
{
    // Activer les logs pour voir les traces dans le terminal
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(3);

    // Définir la position des nœuds pour NetAnim
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0));    // n0
    positionAlloc->Add(Vector(100.0, 0.0, 0.0));  // n1
    positionAlloc->Add(Vector(200.0, 0.0, 0.0));  // n2
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("10ms"));

    NetDeviceContainer dev0dev1 = p2p.Install(NodeContainer(nodes.Get(0), nodes.Get(1)));
    NetDeviceContainer dev1dev2 = p2p.Install(NodeContainer(nodes.Get(1), nodes.Get(2)));

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces0_1 = address.Assign(dev0dev1);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces1_2 = address.Assign(dev1dev2);

    // Serveur UDP sur n1 pour n0
    UdpEchoServerHelper echoServer1(9);
    ApplicationContainer serverApps1 = echoServer1.Install(nodes.Get(1));
    serverApps1.Start(Seconds(1.0));
    serverApps1.Stop(Seconds(10.0));

    // Client UDP n0 -> n1
    UdpEchoClientHelper echoClient1(interfaces0_1.GetAddress(1), 9);
    echoClient1.SetAttribute("MaxPackets", UintegerValue(2));
    echoClient1.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient1.SetAttribute("PacketSize", UintegerValue(512));
    ApplicationContainer clientApps1 = echoClient1.Install(nodes.Get(0));
    clientApps1.Start(Seconds(2.0));
    clientApps1.Stop(Seconds(10.0));

    // Serveur UDP sur n2 pour n1
    UdpEchoServerHelper echoServer2(10);
    ApplicationContainer serverApps2 = echoServer2.Install(nodes.Get(2));
    serverApps2.Start(Seconds(1.0));
    serverApps2.Stop(Seconds(10.0));

    // Client UDP n1 -> n2
    UdpEchoClientHelper echoClient2(interfaces1_2.GetAddress(1), 10);
    echoClient2.SetAttribute("MaxPackets", UintegerValue(2));
    echoClient2.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient2.SetAttribute("PacketSize", UintegerValue(512));
    ApplicationContainer clientApps2 = echoClient2.Install(nodes.Get(1));
    clientApps2.Start(Seconds(3.0));
    clientApps2.Stop(Seconds(10.0));

    // Générer les fichiers PCAP et XML pour NetAnim
    p2p.EnablePcapAll("part2_linear");
    AnimationInterface anim("part2_linear.xml");

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
