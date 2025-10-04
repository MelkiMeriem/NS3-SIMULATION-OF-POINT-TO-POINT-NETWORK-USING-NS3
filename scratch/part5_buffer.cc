#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/traffic-control-module.h"

using namespace ns3;

int main(int argc, char *argv[]) {
    std::string queueType = "DropTail";
    CommandLine cmd(__FILE__);
    cmd.AddValue("queueType", "Type de file d'attente (DropTail ou Red)", queueType);
    cmd.Parse(argc, argv);

    NodeContainer sources, router, destination;
    sources.Create(3);
    router.Create(1);
    destination.Create(1);

    PointToPointHelper p2pSource, p2pBottleneck;
    p2pSource.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2pSource.SetChannelAttribute("Delay", StringValue("1ms"));
    p2pSource.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("100p"));

    p2pBottleneck.SetDeviceAttribute("DataRate", StringValue("3Mbps"));
    p2pBottleneck.SetChannelAttribute("Delay", StringValue("10ms"));

    if (queueType == "DropTail") {
        p2pBottleneck.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("100p"));
    } else if (queueType == "Red") {
        p2pBottleneck.SetQueue("ns3::RedQueue", "MaxSize", StringValue("100p"));
    }

    NetDeviceContainer sourceDevices[3], bottleneckDevices;
    for (int i = 0; i < 3; i++) {
        NodeContainer nc = NodeContainer(sources.Get(i), router.Get(0));
        sourceDevices[i] = p2pSource.Install(nc);
    }

    NodeContainer bottleneck = NodeContainer(router.Get(0), destination.Get(0));
    bottleneckDevices = p2pBottleneck.Install(bottleneck);

    InternetStackHelper stack;
    stack.Install(sources);
    stack.Install(router);
    stack.Install(destination);

    Ipv4AddressHelper address;
    Ipv4InterfaceContainer sourceInterfaces[3];
    for (int i = 0; i < 3; i++) {
        std::ostringstream network;
        network << "10.1." << (i+1) << ".0";
        address.SetBase(network.str().c_str(), "255.255.255.0");
        sourceInterfaces[i] = address.Assign(sourceDevices[i]);
    }

    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer bottleneckInterfaces = address.Assign(bottleneckDevices);

    ApplicationContainer sinkApps, sourceApps;
    Address sinkAddress(InetSocketAddress(bottleneckInterfaces.GetAddress(1), 9));

    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", sinkAddress);
    sinkApps = packetSinkHelper.Install(destination);
    sinkApps.Start(Seconds(1.0));
    sinkApps.Stop(Seconds(20.0));

    for (int i = 0; i < 3; i++) {
        BulkSendHelper source("ns3::TcpSocketFactory", sinkAddress);
        source.SetAttribute("MaxBytes", UintegerValue(0));
        sourceApps.Add(source.Install(sources.Get(i)));
    }
    sourceApps.Start(Seconds(2.0));
    sourceApps.Stop(Seconds(19.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    FlowMonitorHelper flowMonitor;
    Ptr<FlowMonitor> monitor = flowMonitor.InstallAll();

    p2pBottleneck.EnablePcapAll("part5_" + queueType);

    Simulator::Stop(Seconds(20.0));
    Simulator::Run();

    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowMonitor.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

    double totalThroughput = 0;
    uint32_t totalLostPackets = 0;
    uint32_t flowCount = 0;

    for (auto i = stats.begin(); i != stats.end(); ++i) {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(i->first);
        if (i->second.rxBytes > 100000) {
            double throughput = i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds()) / 1000000;
            totalThroughput += throughput;
            totalLostPackets += i->second.lostPackets;
            flowCount++;
        }
    }

    std::cout << "Débit total: " << totalThroughput << " Mbps\n";
    std::cout << "Paquets perdus totaux: " << totalLostPackets << "\n";
    double utilization = (totalThroughput / 3.0) * 100;
    std::cout << "Utilisation du lien goulot: " << utilization << "%\n";

    Simulator::Destroy();
    return 0;
}