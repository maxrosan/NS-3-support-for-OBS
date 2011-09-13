/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/callback.h"
#include "ns3/assert.h"
#include "ns3/obs-channel.h"
#include <iostream>

using namespace ns3;

static void
verify_obschannel() {
	Ptr<OBSFiber> obschannel;

	obschannel = CreateObject<OBSFiber>();
	NS_ASSERT(obschannel != NULL);

}

int main(int argc, char **argv) {

	verify_obschannel();

	return 0;
}
