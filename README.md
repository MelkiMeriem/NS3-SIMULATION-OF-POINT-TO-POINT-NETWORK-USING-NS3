# TP COMPLET - PROTOCOLES RÉSEAUX MOBILES

## Informations générales
- Université: Université de Carthage - INSAT  
- Niveau: RT4  
- Enseignante: Dr Mériem Afif  
- Année: 2025/2026  
- Sujet: Simulation réseau avec NS-3

## Objectifs du TP
- Installer et configurer l'environnement NS-3  
- Comprendre et simuler les protocoles UDP et TCP  
- Maîtriser le routage IP et la gestion de congestion  
- Analyser les performances réseau avec FlowMonitor et Wireshark  

## Installation et configuration
### Installation des dépendances système
sudo apt update  
sudo apt install g++ python3 python3-venv python3-pip cmake ninja-build git gir1.2-goocanvas-2.0 python3-gi python3-gi-cairo python3-pygraphviz gir1.2-gtk-3.0 ipython3 tcpdump wireshark sqlite3 libsqlite3-dev qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools openmpi-bin openmpi-common openmpi-doc libopenmpi-dev doxygen graphviz graphviz-dev imagemagick python3-sphinx dia texlive dvipng latexmk texlive-extra-utils texlive-latex-extra texlive-font-utils libeigen3-dev gsl-bin libgsl-dev libgslcblas0 libxml2 libxml2-dev libgtk-3-dev lxc-utils lxc-templates vtun uml-utilities ebtables bridge-utils libboost-all-dev ccache

### Configuration Python
mkdir -p ~/.config/pip  
echo "[global]" >> ~/.config/pip/pip.conf  
echo "break-system-packages = true" >> ~/.config/pip/pip.conf  
pip3 install cppyy cppyy-backend pygraphviz kiwi

### Installation de NS-3
cd ~/Documents/ns-allinone-3.45/ns-3.45  
./ns3 configure --enable-python-bindings --enable-examples --enable-tests --disable-modules=netsimulyzer  
./ns3 build

### Test de l'installation
./ns3 run first  
./ns3 run first.py

## Partie A : Exemple de simulation de base

### first.cc / first.py
**But:** Simuler une communication UDP simple entre deux nœuds reliés par un lien point-à-point. La simulation crée deux nœuds, installe la pile TCP/IP, configure un serveur UDP sur le nœud 1 et un client UDP sur le nœud 0. Le client envoie un seul paquet pour tester la communication.  
**Commande:** ./ns3 run "examples/tutorial/first.cc" ou ./ns3 run "examples/tutorial/first.py"  
**Explication détaillée:**  
- Création de deux nœuds avec `NodeContainer`.  
- Configuration d’un lien point-à-point avec débit 5 Mbps et délai 2 ms.  
- Installation de la pile Internet (`InternetStackHelper`).  
- Attribution des adresses IP avec `Ipv4AddressHelper`.  
- Configuration du serveur UDP avec `UdpEchoServerHelper` et démarrage à 1 seconde.  
- Configuration du client UDP avec `UdpEchoClientHelper`, envoi d’un paquet de 1024 octets à 2 secondes.  
- Lancement de la simulation avec `Simulator::Run()` et destruction avec `Simulator::Destroy()`.

## Partie B : Simulations réseau avancées

### 1. Communication bidirectionnelle UDP (part1_bidirectional.cc)
**But:** Simuler un échange UDP bidirectionnel entre deux nœuds. Deux serveurs et deux clients sont configurés pour envoyer et recevoir des paquets à différents intervalles. Permet l’observation du trafic réseau via les fichiers PCAP générés.  
**Commande:** ./ns3 run "scratch/part1_bidirectional"  
**Explication:** Permet d’étudier comment UDP peut échanger des données dans les deux sens simultanément, et de visualiser le trafic réseau.

### 2. Topologie linéaire (part2_linear.cc)
**But:** Créer trois nœuds connectés linéairement (n0-n1-n2) et simuler le passage de paquets UDP via le nœud intermédiaire n1. Chaque lien a une adresse IP différente pour comprendre le routage simple.  
**Commande:** ./ns3 run "scratch/part2_linear"  
**Explication:** Étudie la propagation des paquets à travers un réseau à plusieurs nœuds et la configuration IP par lien.

### 3. Routage IP (part3_routing.cc)
**But:** Ajouter le routage global IP avec `Ipv4GlobalRoutingHelper` pour que les paquets traversent automatiquement plusieurs nœuds. Permet de comprendre l’impact du routage dynamique sur la communication UDP.  
**Commande:** ./ns3 run "scratch/part3_routing"  
**Explication:** Montre l’importance des tables de routage dans les réseaux multi-nœuds.

### 4. Communication TCP (part4_tcp.cc)
**But:** Simuler une communication TCP fiable entre deux nœuds. Installer un serveur TCP et un client TCP, mesurer le débit et les pertes avec FlowMonitor, et comparer la fiabilité avec UDP.  
**Commande:** ./ns3 run "scratch/part4_tcp"  
**Explication:** TCP garantit la livraison des paquets et ajuste le débit selon la congestion, contrairement à UDP.

### 5. Gestion de buffer (part5_buffer.cc)
**But:** Comparer DropTail et RED sur un lien goulot avec plusieurs sources TCP. Étudier l’effet de la congestion sur le débit, les pertes et l’équité des flux TCP. Analyse des résultats avec FlowMonitor et Wireshark.  
**Commandes:**  
./ns3 run "scratch/part5_buffer" -- --queueType=DropTail  
./ns3 run "scratch/part5_buffer" -- --queueType=Red  
**Explication:** RED permet de limiter la congestion et d’améliorer l’équité entre les flux, alors que DropTail peut saturer le lien.

## Analyse des résultats
- **UDP vs TCP:** UDP est simple et non fiable, TCP s’adapte à la congestion et garantit la livraison.  
- **Routage IP:** Les tables de routage permettent aux paquets de traverser plusieurs nœuds automatiquement.  
- **Congestion et gestion de buffer:** RED améliore l’équité et réduit les pertes comparé à DropTail.  
- **Outils d’analyse:** Wireshark pour visualiser le trafic, FlowMonitor pour statistiques détaillées par flux.

## Résumé des acquis
- Maîtrise de NS-3 et simulation réseau.  
- Programmation C++ et Python pour la simulation des réseaux.  
- Analyse des protocoles UDP et TCP.  
- Compréhension du routage IP et de la gestion de congestion.  
- Utilisation d’outils professionnels pour l’analyse réseau (Wireshark, FlowMonitor).  
