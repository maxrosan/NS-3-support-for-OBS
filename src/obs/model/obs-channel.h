
#ifndef OBS_CHANNEL_H
#define OBS_CHANNEL_H

#include "ns3/channel.h"
#include "ns3/ptr.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/net-device.h"
#include "ns3/uinteger.h"
#include "ns3/packet-burst.h"

#include <vector>

namespace ns3 {

class Packet;

enum ChannelState {
	IDLE,
	TRANSMITTING,
	PROPAGATING,
};

class OBSFiber : public Channel {
private:
	DataRate m_bps;
	Time     m_delay;
	uint32_t m_num_wavelength;

	ChannelState *m_channel_state;
	uint8_t *m_current_sender;
	Ptr<PacketBurst> *m_current_burst;

	Ptr<Packet> m_current_ctl_pkt;
	
	void PropagationCompleteControlPacket();
	void PropagationCompleteBurstPacket(uint8_t i);
public:
	static TypeId GetTypeId(void);

	OBSFiber();
	virtual uint32_t GetNDevices(void) const;
	virtual Ptr<NetDevice> GetDevice(uint32_t i) const;

	bool TransmitStartControlPacket(Ptr<Packet> packet, uint8_t sender);
	void TransmitEndControlPacket();

	bool TransmitStartBurstPacket(Ptr<PacketBurst>, uint8_t wavelength, uint8_t sender);
	void TransmitEndBurstPacket(uint8_t wavelength);
};

};

#endif
