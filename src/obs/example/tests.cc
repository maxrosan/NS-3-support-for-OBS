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


	cd1 = CreateObject<CoreDevice>();
	NS_ASSERT(cd1 != NULL);

	cd2 = CreateObject<CoreDevice>();
	NS_ASSERT(cd2 != NULL);

	cd1->SetFiber(obschannel);
	cd2->SetFiber(obschannel);

	nodes.Get(0)->AddDevice(cd1);
	nodes.Get(1)->AddDevice(cd2);
	//
	pkt = Packet((uint8_t*) "ONE", 3);
	pkt1 = Packet((uint8_t*) "TWO", 3);

	pb.AddPacket(pkt.Copy());
	pb.AddPacket(pkt1.Copy());
	//		

	Simulator::Run ();
	Simulator::Destroy ();

	std::cout << "Done. " << Simulator::Now().GetSeconds() << std::endl;

	FUNC_OK;
}

int main(int argc, char **argv) {

	verify_obschannel();

	return 0;
}
