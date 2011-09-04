
#ifndef OBS_CHANNEL_H
#define OBS_CHANNEL_H

#include "ns3/channel.h"
#include "ns3/ptr.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"

#include <vector>

namespace ns3 {

class Packet;

class OBSDeviceRec {
public:
};

enum ChannelState {
	IDLE,
	TRANSMITING,
};

class OBSChannel;

class OBSSubChannel { 
private:
	OBSChannel   *m_obs;
	ChannelState m_state;
public:
	OBSSubChannel();
	void setOBSChannel(OBSChannel *obs);
};

class OBSChannel : public Channel {
private:
	DataRate                   m_bps;
	Time                       m_delay;
	uint32_t                   m_nsubchannels;
	std::vector<OBSSubChannel> m_subchannels;
public:
	static TypeId GetTypeId();
	
	OBSChannel();
};

};

#endif
