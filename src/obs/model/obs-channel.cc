
#include "obs-channel.h"

#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

#include "obs-device.h"

NS_LOG_COMPONENT_DEFINE("OBSFiber");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(OBSFiber);

class OBSBaseDevice;

OBSWavelength::OBSWavelength() {
	m_state = IDLE;
	m_current_packet = Ptr<Packet>(NULL);
	m_current_src = 0;
	m_channel = Ptr<OBSFiber>(NULL);
}

OBSWavelength::OBSWavelength(Ptr<OBSFiber> channel, uint32_t id) {
	NS_ASSERT(channel != NULL);

	m_current_packet = Ptr<Packet>(NULL);
	m_current_src = 0;
	m_channel = channel;
	m_state = IDLE;
	m_id = id;
}

OBSWavelength::OBSWavelength(const OBSWavelength &c) {
	CopyFrom(c);
}

OBSWavelength
OBSWavelength::operator=(const OBSWavelength &c) {
	CopyFrom(c);
	return *this;
}

void
OBSWavelength::CopyFrom(const OBSWavelength &wv) {
	m_state = wv.m_state;
	m_current_packet = wv.m_current_packet;
	m_current_src = wv.m_current_src;
	m_channel = wv.m_channel;
	m_id = wv.m_id;
}

ChannelState
OBSWavelength::GetState() {
	return m_state;
}

bool
OBSWavelength::TransmitStart(Ptr<Packet> packet, uint32_t srcId) {
	Ptr<NetDevice> device;
	uint32_t len, i;

	if (m_state != IDLE) {
		return false;
	}

	// XXX: Is it necessary to check the status of sender?
	
	m_current_src = srcId;
	m_current_packet = packet;
	m_state = PROPAGATING;
	
	len = m_channel->GetNDevices();

	for (i = 0; i < len; i++) {
		if (i != srcId) {
			device = m_channel->GetDevice(i);

			Simulator::ScheduleWithContext(
				device->GetNode()->GetId(),
				m_channel->GetDelay(),
				&OBSBaseDevice::StartReceiving,
				m_channel->GetOBSDevice(i),
				packet,
				m_channel->GetOBSDevice(srcId),
				m_id);
		}
	}

	Simulator::Schedule(m_channel->GetDelay(), &OBSWavelength::PropagationCompleteEvent,
	  this);

	return true;
}

void
OBSWavelength::PropagationCompleteEvent() {
	NS_ASSERT(m_state == PROPAGATING);

	m_state = TRANSMITING;
}

bool
OBSWavelength::TransmitEnd() {
	uint32_t i, len;
	Ptr<OBSBaseDevice> device;

	NS_ASSERT(m_state == TRANSMITING);

	m_state = PROPAGATING;

	len = m_channel->GetNDevices();

	for (i = 0; i < len; i++) {
		if (i != m_current_src) {
			device = m_channel->GetOBSDevice(i);
			device->StopReceiving(m_channel->GetOBSDevice(m_current_src), m_id);
		}
	}

	PropagationCompleteEvent();

	return true;
}

void
OBSWavelength::SetChannel(Ptr<OBSFiber> channel) {
	m_channel = channel;
}

TypeId
OBSFiber::GetTypeId(void) {

	static TypeId tid = TypeId("ns3::OBSFiber")
		.SetParent<Channel>()
		.AddConstructor<OBSFiber>()
		.AddAttribute("DataRate",
			"Transmission rate",
			DataRateValue(DataRate("10Gbps")),
			MakeDataRateAccessor(&OBSFiber::m_bps),
			MakeDataRateChecker())
		.AddAttribute("Delay",
			"Transmission delay",
			// XXX: Hypothetical value. A more realistic value
			// should be used
			TimeValue(Time("10ns")),
			MakeTimeAccessor(&OBSFiber::m_delay),
			MakeTimeChecker())
		.AddAttribute("NumberOfWavelength",
			"...", // XXX: Some comments here
			TypeId::ATTR_GET,
			UintegerValue(5),
			MakeUintegerAccessor(&OBSFiber::m_num_wavelength),
			MakeUintegerChecker<uint32_t>(1, 5))
		;

	return tid;
}

OBSFiber::OBSFiber(): 
	Channel() 
{
	uint32_t i;

	NS_LOG_FUNCTION_NOARGS();
	m_devices.reserve(2);

	for (i = 0; i < m_num_wavelength; i++) {
		m_wavelength.push_back(OBSWavelength(Ptr<OBSFiber>(this, false), i));
	}
}

uint32_t
OBSFiber::GetNDevices(void) const {
	return m_devices.size();
}

Ptr<NetDevice>
OBSFiber::GetDevice(uint32_t i) const {
	return m_devices[i];
}

Ptr<OBSBaseDevice>
OBSFiber::GetOBSDevice(uint32_t i) const {
	return m_devices[i];
}

uint32_t
OBSFiber::Attach(Ptr<OBSBaseDevice> device) {
	NS_ASSERT(m_devices.size() < 2);
	m_devices.push_back(device);
	
	return (m_devices.size() - 1);
}

void
OBSFiber::Detach(uint32_t id) {
	Ptr<OBSBaseDevice> ptr;
	
	NS_ASSERT(id >= 0 && id < m_devices.size());

	m_devices.erase(m_devices.begin() + id);
}

void
OBSFiber::Detach(Ptr<OBSBaseDevice> device) {
	uint32_t i;

	for (i = 0; i < m_devices.size(); i++) {
		if (m_devices[i] == device) {
			uint32_t j;
			j = m_devices.size() + 1;
			Detach(i);
			i = j;
		}
	} 
}

Time
OBSFiber::GetDelay() {
	return m_delay;
}

bool
OBSFiber::TransmitStart(Ptr<Packet> packet, uint32_t srcId,
	uint32_t wavelength) {

	NS_ASSERT(wavelength < m_num_wavelength);
	NS_ASSERT(srcId < m_devices.size());

	return m_wavelength[wavelength].TransmitStart(packet, srcId);
}

bool
OBSFiber::TransmitEnd(uint32_t wavelength) {
	NS_ASSERT(wavelength < m_num_wavelength);

	return m_wavelength[wavelength].TransmitEnd();
}

};
