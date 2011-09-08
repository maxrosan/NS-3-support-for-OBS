
#ifndef OBS_CHANNEL_H
#define OBS_CHANNEL_H

#include "ns3/channel.h"
#include "ns3/ptr.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/net-device.h"
#include "ns3/uinteger.h"

#include <vector>

namespace ns3 {

class Packet;
class OBSBaseDevice;

enum ChannelState {
	IDLE,
	TRANSMITING,
	PROPAGATING,
};

class OBSFiber;
class OBSWavelength {
private:
	ChannelState    m_state;
	Ptr<Packet>     m_current_packet;
	uint32_t        m_current_src;
	Ptr<OBSFiber>   m_channel;
	uint32_t        m_id;

	void CopyFrom(const OBSWavelength &wl);
public:
	OBSWavelength();
	OBSWavelength(Ptr<OBSFiber> channel, uint32_t id);
	OBSWavelength(const OBSWavelength &c);
	OBSWavelength operator=(const OBSWavelength &c);
	void SetState(ChannelState state);
	ChannelState GetState();
	bool TransmitStart(Ptr<Packet> packet, uint32_t srcId);
	bool TransmitEnd();
	void PropagationCompleteEvent();
	void SetChannel(Ptr<OBSFiber> channel);
	void SetId(uint32_t id);
	uint32_t GetId();
};

class OBSFiber : public Channel {
private:
	DataRate m_bps;
	Time     m_delay;
	std::vector<Ptr<OBSBaseDevice> > m_devices;
	uint32_t m_num_wavelength;
	std::vector<OBSWavelength> m_wavelength;
public:
	static TypeId GetTypeId();
	OBSFiber();
	virtual uint32_t GetNDevices(void) const;
	virtual Ptr<NetDevice> GetDevice(uint32_t i) const;
	Ptr<OBSBaseDevice> GetOBSDevice(uint32_t i) const;
	uint32_t Attach(Ptr<OBSBaseDevice> device);
	void Detach(uint32_t id);
	void Detach(Ptr<OBSBaseDevice> device);
	Time GetDelay();
	bool TransmitStart(Ptr<Packet> packet, uint32_t srcId, uint32_t wavelength);
	bool TransmitEnd(uint32_t wavelength);
};

};

#endif
