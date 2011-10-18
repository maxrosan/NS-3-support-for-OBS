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

using namespace ns3;

#define FUNC_OK fprintf(stderr, "%s OK\n", __func__)

static void
verify_obschannel() {

	Ptr<OBSFiber> obschannel;
	Ptr<CoreDevice> cd1, cd2;
	NodeContainer nodes;
	Packet pkt, pkt1;
	PacketBurst pb;

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

	verify_obschannel();

	return 0;
}
