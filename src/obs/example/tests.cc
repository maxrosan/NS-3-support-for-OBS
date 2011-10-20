/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/callback.h"
#include "ns3/assert.h"
#include "ns3/obs-channel.h"
#include "ns3/obs-device.h"
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/deprecated.h"
#include "ns3/trace-helper.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include <iostream>
#include <cstdio>
#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <utility>

using namespace ns3;
using std::cin;
using std::cout;

#define FUNC_OK fprintf(stderr, "%s OK\n", __func__)

static void
read_input() {
	uint32_t n_nodes, // number of nodes
	         i;
	std::string type, name, nA, nB, sep;
	NodeContainer nc;
	std::map<std::string, uint32_t> map_node;
	// [A][B] -> (Address, Device) => P/ enviar de A p/ B o destino tem endereço Address e o enlace de saída é Device
	std::map<std::string, std::map<std::string, 
	 std::pair<Mac48Address, Ptr<CoreDevice> > > > map_routing;
	NetDeviceContainer ndc;

	cin >> n_nodes;
	nc.Create(n_nodes);

	for (i = 0; i < n_nodes; i++) {
		Ptr<OBSSwitch> obs_switch;

		cin >> type >> name;
		NS_ASSERT(type == std::string("node"));
		
		map_node[name] = i;
		obs_switch = CreateObject<OBSSwitch>();
		NS_ASSERT(obs_switch != NULL);
		nc.Get(i)->AggregateObject(obs_switch);
	}

	LogComponentEnable("OBSDevice", LOG_LEVEL_INFO);

	cin >> name;
	NS_ASSERT(name == std::string("#"));

	// no link is repeated
	cin >> nA;
	while (nA != std::string("#")) {
		cin >> sep >> nB;
		cout << nA << " --- " << nB << std::endl;

		if (map_node.find(nA) == map_node.end() ||
		 map_node.find(nB) == map_node.end()) {
			cout << "Node node found" << std::endl;
			return;
		} else {
			Ptr<OBSFiber> obschannel;
			Ptr<CoreNodeDevice> dev1, dev2;
			Ptr<Node> node1, node2;
			Ptr<OBSSwitch> obss1, obss2;

			node1 = nc.Get(map_node[nA]);
			node2 = nc.Get(map_node[nB]);

			obss1 = node1->GetObject<OBSSwitch>();
			NS_ASSERT(obss1 != NULL);
			obss2 = node2->GetObject<OBSSwitch>();
			NS_ASSERT(obss2 != NULL);

			obschannel = CreateObject<OBSFiber>();
			NS_ASSERT(obschannel != NULL);
			obschannel->SetAttribute("NumOfWL", UintegerValue(5));
			obschannel->SetAttribute("Delay", TimeValue(Time("5ms")));
			obschannel->SetAttribute("DataRate", DataRateValue(DataRate("10Gbps")));		

			dev1 = CreateObject<CoreNodeDevice>();
			dev2 = CreateObject<CoreNodeDevice>();
			ndc.Add(dev1);
			ndc.Add(dev2);

			dev1->SetFiber(obschannel);
			dev1->SetAddress(Mac48Address::Allocate());
			dev1->SetSwitch(obss1);
			dev1->SetMtu(1 << 15);

			dev2->SetFiber(obschannel);
			dev2->SetAddress(Mac48Address::Allocate());
			dev2->SetSwitch(obss2);
			dev2->SetMtu(1 << 15);

			node1->AddDevice(dev1);
			obss1->AddDevice(dev1);
			obss1->AddRoute(Mac48Address::ConvertFrom(dev2->GetAddress()),
			 OBSRoutingTableTuple(dev1, 0));

			node2->AddDevice(dev2);
			obss2->AddDevice(dev2);
			obss2->AddRoute(Mac48Address::ConvertFrom(dev1->GetAddress()),
			 OBSRoutingTableTuple(dev2, 0));

			map_routing[nA][nB] = std::make_pair(Mac48Address::ConvertFrom(dev2->GetAddress()), dev1);
			map_routing[nB][nA] = std::make_pair(Mac48Address::ConvertFrom(dev1->GetAddress()), dev2);
		}

		cin >> nA;
	}

	cin >> type;
	while (type == std::string("path")) {
		std::stack<std::string> S;
		std::string target, last;
		Mac48Address addr;

		cin >> name;
		while (name != std::string(";")) {
			S.push(name);
			NS_ASSERT(map_node.find(name) != map_node.end());
			cin >> name;
		}

		last = target = S.top();
		S.pop();
		while (!S.empty()) {
			std::string nod;
			Ptr<Node> ptrnode;
			Ptr<OBSSwitch> sw;

			nod = S.top();
			if (last == target) {
				addr = Mac48Address::ConvertFrom(map_routing[nod][target].first);
			} else {
				Ptr<CoreDevice> dev;

				ptrnode = nc.Get(map_node[nod]);
				sw = ptrnode->GetObject<OBSSwitch>();
				NS_ASSERT(sw != NULL);
				dev = map_routing[nod][last].second;

				cout << nod << "[" << Mac48Address::ConvertFrom(dev->GetAddress()) << "] -> " << addr << std::endl;
				sw->AddRoute(addr, OBSRoutingTableTuple(dev, 0));
			}
	
			last = nod;
			S.pop();
		}

		cin >> type;
	}


	InternetStackHelper internet;
	internet.Install(nc);

	Ipv4AddressHelper ipv4;
	ipv4.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = ipv4.Assign(ndc);

	cin >> type;
	while (type != std::string("#")) {
		std::string source, sink, addr;
		uint32_t port;
		double start, end;

		cin >> source >> sink >> port >> start >> end;

		if (type == std::string("onoff")) {
			OnOffHelper onoff ("ns3::UdpSocketFactory", 
					Address (InetSocketAddress(
							interfaces.GetAddress (nc.Get(map_node[sink])->GetObject<OBSSwitch>()->GetFirstInterface()->GetIfIndex()), port)));
			onoff.SetAttribute ("OnTime", RandomVariableValue (ConstantVariable (1)));
			onoff.SetAttribute ("OffTime", RandomVariableValue (ConstantVariable (0)));

			ApplicationContainer app = onoff.Install (nc.Get(map_node[source]));
			// Start the application
			app.Start (Seconds (start));
			app.Stop (Seconds (end));
			
			PacketSinkHelper sinkh ("ns3::UdpSocketFactory",
					Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
			app = sinkh.Install (nc.Get(map_node[sink]));
			app.Start (Seconds (start));
			app.Stop (Seconds (end));
		}

		cin >> type;
	}

	cout << "Interfaces" << std::endl;
	for (
	 std::map<std::string, uint32_t>::iterator it = map_node.begin();
	 it != map_node.end();
	 it++
	) {
		Ptr<Node> node;
		uint32_t j;		

		cout << "Node " << it->first << std::endl;
		node = nc.Get(it->second);
		
		for (j = 0; j < node->GetNDevices(); j++) {
			cout << "IF " << Mac48Address::ConvertFrom(node->GetDevice(j)->GetAddress()) << std::endl;
		}

		cout << "Routing table" << std::endl;
		node->GetObject<OBSSwitch>()->PrintTable(cout);
	}

	cout << "Ready to go" << std::endl << std::endl << std::endl;

	Simulator::Run ();
	Simulator::Destroy ();
}

static void
verify_obschannel() {

	Ptr<OBSFiber> obschannel;
	Ptr<CoreDevice> cd1, cd2;
	NodeContainer nodes;
	Packet pkt, pkt1;
	PacketBurst pb;
	Ptr<OBSSwitch> obs_switch;

	nodes.Create(4);

	obschannel = CreateObject<OBSFiber>();
	NS_ASSERT(obschannel != NULL);
	obschannel->SetAttribute("NumOfWL", UintegerValue(5));
	obschannel->SetAttribute("Delay", TimeValue(Time("5ms")));
	obschannel->SetAttribute("DataRate", DataRateValue(DataRate("10Gbps")));

	cd1 = CreateObject<CoreDevice>();
	NS_ASSERT(cd1 != NULL);

	cd2 = CreateObject<CoreDevice>();
	NS_ASSERT(cd2 != NULL);

	obs_switch = CreateObject<OBSSwitch>();
	NS_ASSERT(obs_switch != NULL);

	LogComponentEnable("OBSDevice", LOG_LEVEL_INFO);

	cd1->SetFiber(obschannel);
	cd1->SetAddress(Mac48Address("11:22:33:44:55:66"));
	cd2->SetFiber(obschannel);
	cd2->SetAddress(Mac48Address("AA:BB:CC:33:44:55"));

	nodes.Get(0)->AddDevice(cd1);
	nodes.Get(1)->AddDevice(cd2);
	//
	pkt = Packet((uint8_t*) "ONE", 3);
	pkt1 = Packet((uint8_t*) "TWO", 3);

	pb.AddPacket(pkt.Copy());
	pb.AddPacket(pkt1.Copy());
	//
	cd1->ChangeRoute(1, cd2, 2);
	cd2->ChangeRoute(2, cd1, 1);

	cd1->SendBurst(1, pb.Copy());
	//
	cd1->SendControlPacket(Mac48Address::ConvertFrom(cd2->GetAddress()), 1, 10, 3);
	//
	Simulator::Run ();
	Simulator::Destroy ();

	std::cout << "Done. " << std::endl;

	FUNC_OK;
}

int main(int argc, char **argv) {

	read_input();

	return 0;
	
	verify_obschannel();
}
