// On importe les modules nécessaires de NS-3
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

using namespace ns3;

int main(int argc, char *argv[]) {

    // --- 1. Création de deux ordinateurs (nœuds) ---
    NodeContainer nodes;
    nodes.Create(2);  // node0 et node1

    // --- 2. Création d’un lien point-à-point entre eux ---
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps")); // vitesse : 5 Mégabits par seconde
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));     // délai de transmission : 2 millisecondes

    // --- 3. Installation du lien entre les deux nœuds ---
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    // --- 4. Installation de la pile Internet (IP, UDP, etc.) sur chaque nœud ---
    InternetStackHelper stack;
    stack.Install(nodes);

    // --- 5. Attribution d’adresses IP ---
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");   // sous-réseau utilisé
    Ipv4InterfaceContainer interfaces = address.Assign(devices);
    // node0 reçoit l’adresse 10.1.1.1
    // node1 reçoit l’adresse 10.1.1.2

    // --- 6. Serveur UDP sur node0 (port 9) ---
    UdpEchoServerHelper echoServer1(9);
    ApplicationContainer serverApps1 = echoServer1.Install(nodes.Get(0)); // installe le serveur sur node0
    serverApps1.Start(Seconds(1.0));  // démarre à 1 seconde
    serverApps1.Stop(Seconds(10.0));  // s’arrête à 10 secondes

    // --- 7. Serveur UDP sur node1 (port 10) ---
    UdpEchoServerHelper echoServer2(10);
    ApplicationContainer serverApps2 = echoServer2.Install(nodes.Get(1)); // installe le serveur sur node1
    serverApps2.Start(Seconds(1.0));
    serverApps2.Stop(Seconds(10.0));

    // --- 8. Client sur node1 qui envoie un message à node0 ---
    UdpEchoClientHelper echoClient1(interfaces.GetAddress(0), 9); // destination : node0 (10.1.1.1), port 9
    echoClient1.SetAttribute("MaxPackets", UintegerValue(1));      // envoie 1 seul message
    echoClient1.SetAttribute("PacketSize", UintegerValue(1024));   // taille du message : 1024 octets
    ApplicationContainer clientApps1 = echoClient1.Install(nodes.Get(1)); // installé sur node1
    clientApps1.Start(Seconds(2.0));  // commence à envoyer à 2 secondes
    clientApps1.Stop(Seconds(10.0));  // s’arrête à 10 secondes

    // --- 9. Client sur node0 qui envoie un message à node1 ---
    UdpEchoClientHelper echoClient2(interfaces.GetAddress(1), 10); // destination : node1 (10.1.1.2), port 10
    echoClient2.SetAttribute("MaxPackets", UintegerValue(1));      // envoie 1 seul message
    echoClient2.SetAttribute("PacketSize", UintegerValue(512));    // taille du message : 512 octets
    ApplicationContainer clientApps2 = echoClient2.Install(nodes.Get(0)); // installé sur node0
    clientApps2.Start(Seconds(4.0));  // commence à envoyer à 4 secondes
    clientApps2.Stop(Seconds(10.0));  // s’arrête à 10 secondes

    // --- 10. Enregistre les échanges dans un fichier PCAP pour Wireshark ---
    pointToPoint.EnablePcapAll("part1_bidirectional");

    // --- 11. Exécution de la simulation ---
    Simulator::Run();      // démarre la simulation
    Simulator::Destroy();  // libère la mémoire à la fin

    return 0;
}
