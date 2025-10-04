try:
    from ns import ns
except ModuleNotFoundError:
    raise SystemExit(
        "Error: ns3 Python module not found;"
        " Python bindings may not be enabled"
        " or your PYTHONPATH might not be properly configured"
    )

# Activer le logging pour suivre les paquets
ns.LogComponentEnable("UdpEchoClientApplication", ns.LOG_LEVEL_INFO)
ns.LogComponentEnable("UdpEchoServerApplication", ns.LOG_LEVEL_INFO)

# Création de 2 nœuds
nodes = ns.NodeContainer()
nodes.Create(2)

# Création du lien point-à-point
pointToPoint = ns.PointToPointHelper()
pointToPoint.SetDeviceAttribute("DataRate", ns.StringValue("5Mbps"))
pointToPoint.SetChannelAttribute("Delay", ns.StringValue("2ms"))

devices = pointToPoint.Install(nodes)

# Installation de la pile réseau IP
stack = ns.InternetStackHelper()
stack.Install(nodes)

# Attribution des adresses IP
address = ns.Ipv4AddressHelper()
address.SetBase(ns.Ipv4Address("10.1.1.0"), ns.Ipv4Mask("255.255.255.0"))
interfaces = address.Assign(devices)

# Création d'un serveur UDP
echoServer = ns.UdpEchoServerHelper(9)
serverApps = echoServer.Install(nodes.Get(1))
serverApps.Start(ns.Seconds(1))
serverApps.Stop(ns.Seconds(10))

# Création d'un client UDP
address = interfaces.GetAddress(1).ConvertTo()
echoClient = ns.UdpEchoClientHelper(address, 9)
echoClient.SetAttribute("MaxPackets", ns.UintegerValue(1))
echoClient.SetAttribute("Interval", ns.TimeValue(ns.Seconds(1)))
echoClient.SetAttribute("PacketSize", ns.UintegerValue(1024))

clientApps = echoClient.Install(nodes.Get(0))
clientApps.Start(ns.Seconds(2))
clientApps.Stop(ns.Seconds(10))

# Lancer la simulation
ns.Simulator.Run()
ns.Simulator.Destroy()
