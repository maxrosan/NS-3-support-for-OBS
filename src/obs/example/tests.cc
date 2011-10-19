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
#include <iostream>
#include <cstdio>
#include <stdint.h>
#include <vector>
#include <string>
#include <map>

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

			dev1->SetFiber(obschannel);
			dev1->SetAddress(Mac48Address::Allocate());
			dev1->SetSwitch(obss1);

			dev2->SetFiber(obschannel);
			dev2->SetAddress(Mac48Address::Allocate());
			dev2->SetSwitch(obss2);

			node1->AddDevice(dev1);
			obss1->AddDevice(dev1);

			node2->AddDevice(dev2);
			obss2->AddDevice(dev2);
		}

		cin >> nA;
	}
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
