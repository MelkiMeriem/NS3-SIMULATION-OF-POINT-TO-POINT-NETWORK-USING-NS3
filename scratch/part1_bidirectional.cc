#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

int main(int argc, char *argv[]) {
    NodeContainer nodes;
    nodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    UdpEchoServerHelper echoServer1(9);
    ApplicationContainer serverApps1 = echoServer1.Install(nodes.Get(0));
    serverApps1.Start(Seconds(1.0));
    serverApps1.Stop(Seconds(10.0));

    UdpEchoServerHelper echoServer2(10);
    ApplicationContainer serverApps2 = echoServer2.Install(nodes.Get(1));
    serverApps2.Start(Seconds(1.0));
    serverApps2.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient1(interfaces.GetAddress(0), 9);
    echoClient1.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient1.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer clientApps1 = echoClient1.Install(nodes.Get(1));
    clientApps1.Start(Seconds(2.0));
    clientApps1.Stop(Seconds(10.0));

    UdpEchoClientHelper echoClient2(interfaces.GetAddress(1), 10);
    echoClient2.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient2.SetAttribute("PacketSize", UintegerValue(512));
    ApplicationContainer clientApps2 = echoClient2.Install(nodes.Get(0));
    clientApps2.Start(Seconds(4.0));
    clientApps2.Stop(Seconds(10.0));

    pointToPoint.EnablePcapAll("part1_bidirectional");
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}