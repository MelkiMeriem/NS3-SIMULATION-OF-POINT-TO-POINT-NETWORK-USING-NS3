# TP COMPLET - PROTOCOLES RÉSEAUX MOBILES

## Informations générales

* **Université**: Université de Carthage - INSAT
* **Niveau**: RT4
* **Enseignante**: Dr Mériem Afif
* **Année**: 2025/2026
* **Sujet**: Simulation réseau avec NS-3

## Objectifs du TP

* Installer et configurer l'environnement NS-3
* Comprendre et simuler les protocoles UDP et TCP
* Maîtriser le routage IP et la gestion de congestion
* Analyser les performances réseau avec FlowMonitor et Wireshark
* Observer les différences entre les protocoles et les stratégies de gestion de buffer

## Installation et configuration

### Installation des dépendances système

```bash
sudo apt update
sudo apt install g++ python3 python3-venv python3-pip cmake ninja-build git \
gir1.2-goocanvas-2.0 python3-gi python3-gi-cairo python3-pygraphviz gir1.2-gtk-3.0 \
ipython3 tcpdump wireshark sqlite3 libsqlite3-dev qtbase5-dev qtchooser qt5-qmake \
qtbase5-dev-tools openmpi-bin openmpi-common openmpi-doc libopenmpi-dev doxygen graphviz \
graphviz-dev imagemagick python3-sphinx dia texlive dvipng latexmk texlive-extra-utils \
texlive-latex-extra texlive-font-utils libeigen3-dev gsl-bin libgsl-dev libgslcblas0 \
libxml2 libxml2-dev libgtk-3-dev lxc-utils lxc-templates vtun uml-utilities ebtables \
bridge-utils libboost-all-dev ccache
```

### Configuration Python

```bash
mkdir -p ~/.config/pip
echo "[global]" >> ~/.config/pip/pip.conf
echo "break-system-packages = true" >> ~/.config/pip/pip.conf
pip3 install cppyy cppyy-backend pygraphviz kiwi
```

### Installation de NS-3

```bash
cd ~/Documents/ns-allinone-3.45/ns-3.45
./ns3 configure --enable-python-bindings --enable-examples --enable-tests --disable-modules=netsimulyzer
./ns3 build
```

### Test de l'installation

```bash
./ns3 run first
./ns3 run first.py
```

## Partie A : Exemple de simulation de base

### `first.cc` / `first.py`

**But**: Simuler une communication UDP simple entre deux nœuds reliés par un lien point-à-point.
**Explication détaillée**:

* NodeContainer: Crée les nœuds du réseau.
* PointToPointHelper: Définit le lien avec un débit et un délai spécifique.
* InternetStackHelper: Installe la pile IP sur les nœuds.
* Ipv4AddressHelper: Assigne les adresses IP sur le lien.
* UdpEchoServerHelper et UdpEchoClientHelper: Créent un serveur et un client UDP.
* Simulator: Gère le temps et l'exécution de la simulation.

**Commande**:

```bash
./ns3 run "examples/tutorial/first.cc"
./ns3 run "examples/tutorial/first.py"
```

## Partie B : Simulations réseau avancées

### 1. Communication bidirectionnelle UDP (`part1_bidirectional.cc`)

**But**: Échange UDP bidirectionnel entre deux nœuds.
**Différence avec `first.cc`**: Trafic bidirectionnel, chaque nœud est client et serveur.

**Commande**:

```bash
./ns3 run "scratch/part1_bidirectional"
```

### 2. Topologie linéaire (`part2_linear.cc`)

**But**: Trois nœuds connectés linéairement pour étudier le passage de paquets via un nœud intermédiaire.

**Commande**:

```bash
./ns3 run "scratch/part2_linear"
```

### 3. Routage IP (`part3_routing.cc`)

**But**: Ajouter le routage global IP pour permettre aux paquets de traverser plusieurs nœuds automatiquement.

**Commande**:

```bash
./ns3 run "scratch/part3_routing"
```

### 4. Communication TCP (`part4_tcp.cc`)

**But**: Simuler TCP fiable entre deux nœuds, mesurer débit et pertes.

**Commande**:

```bash
./ns3 run "scratch/part4_tcp"
```

### 5. Gestion de buffer (`part5_buffer.cc`)

**But**: Comparer DropTail et RED sur un lien goulot, observer la congestion et l’équité.

**Commandes**:

```bash
# Test DropTail
./ns3 run "scratch/part5_buffer" -- --queueType=DropTail

# Test RED
./ns3 run "scratch/part5_buffer" -- --queueType=Red
```

## Analyse des résultats

* UDP vs TCP: UDP est non fiable, TCP s’adapte à la congestion et garantit la livraison.
* Routage IP: Les tables de routage permettent aux paquets de traverser plusieurs nœuds automatiquement.
* Congestion et gestion de buffer: RED améliore l’équité et réduit les pertes comparé à DropTail.
* Outils d’analyse: Wireshark pour visualiser le trafic, FlowMonitor pour statistiques détaillées.

## Résumé des acquis

* Maîtrise de NS-3 et simulation réseau.
* Programmation C++ pour simulation des réseaux.
* Analyse des protocoles UDP et TCP.
* Compréhension du routage IP et de la gestion de congestion.
* Utilisation d’outils professionnels pour l’analyse réseau (Wireshark, FlowMonitor).
