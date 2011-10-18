
#include "obs-channel.h"

#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

#include "obs-device.h"

NS_LOG_COMPONENT_DEFINE("OBSFiber");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(OBSFiber);

TypeId
OBSFiber::GetTypeId(void) {

	static TypeId tid = TypeId("ns3::OBSFiber")
		.SetParent<Channel>()
		.AddConstructor<OBSFiber>()
		.AddAttribute("DataRate", "Transmission rate",
			DataRateValue(DataRate("10Gbps")),
			MakeDataRateAccessor(&OBSFiber::m_bps),
			MakeDataRateChecker())
		.AddAttribute("Delay", "Transmission delay",
			// XXX: Hypothetical value. A more realistic value
			// should be used
			TimeValue(Time("50ms")),
			MakeTimeAccessor(&OBSFiber::m_delay),
			MakeTimeChecker())
		.AddAttribute("NumOfWL", "...",
			// XXX: Some comments here
			TypeId::ATTR_SET | TypeId::ATTR_GET,
			UintegerValue(5),
			MakeUintegerAccessor(&OBSFiber::m_num_wavelength),
			MakeUintegerChecker<uint32_t>(1, 5))
		;

	return tid;
}

OBSFiber::OBSFiber(): 
	Channel() 
{
	uint8_t i;

	NS_LOG_FUNCTION_NOARGS();

	// wavelength 0 is reserved for control channel
	m_channel_state = new ChannelState[m_num_wavelength + 1];
	m_current_sender = new uint8_t[m_num_wavelength + 1];
	m_current_burst = new Ptr<PacketBurst>[m_num_wavelength + 1]; // 0 is not used

	for (i = 0; i <= m_num_wavelength; i++) {
		m_channel_state[i] = IDLE;
	}

	m_points[0] = m_points[1] = NULL;

	m_id_gen = 0;
}

uint32_t
OBSFiber::GetNDevices(void) const {
	return 0;
}

Ptr<NetDevice>
OBSFiber::GetDevice(uint32_t i) const {
	return NULL;
}

bool
OBSFiber::TransmitStartControlPacket(Ptr<Packet> packet, uint8_t sender) {
	
	if (m_channel_state[0] != IDLE) {
		return false;
	}

	m_channel_state[0] = TRANSMITTING;
	m_current_sender[0] = sender;
	m_current_ctl_pkt = packet;

	return true;
}

void
OBSFiber::PropagationCompleteControlPacket() {
	NS_ASSERT(m_channel_state[0] == PROPAGATING);

	m_channel_state[0] = IDLE;

	if (m_current_sender[0] == 0) {
		m_points[1]->ReceiveControlPacket(m_current_ctl_pkt);
	} else {
		m_points[0]->ReceiveControlPacket(m_current_ctl_pkt);
	}
}

void
OBSFiber::TransmitEndControlPacket() {

	NS_ASSERT(m_channel_state[0] == TRANSMITTING);
	
	m_channel_state[0] = PROPAGATING;

	// XXX: for each node is not the sender receives the control packet after propagation delay

	Simulator::Schedule(m_delay, &OBSFiber::PropagationCompleteControlPacket, this);
	
}

bool
OBSFiber::TransmitStartBurstPacket(Ptr<PacketBurst> burst, uint8_t wavelength, uint8_t sender) {

	NS_ASSERT(wavelength > 0 && wavelength <= m_num_wavelength);
	NS_ASSERT(sender < 2);

	uint32_t wavelength_rcv;

	if (m_channel_state[wavelength] != IDLE) {
		return false;
	}

	m_channel_state[wavelength] = PROPAGATING;
	m_current_sender[wavelength] = sender;
	m_current_burst[wavelength] = burst;

	// for each node is not the sender receives a notification after propagation delay
	if (sender == 0)
		wavelength_rcv = 1;
	else
		wavelength_rcv = 0;

	Simulator::Schedule(m_delay, &CoreDevice::ReceiveBurstStart,
	  m_points[wavelength_rcv], wavelength);

	Simulator::Schedule(m_delay, &OBSFiber::PropagationCompleteBurstPacket,
	  this, wavelength);

	return true;
}

void
OBSFiber::PropagationCompleteBurstPacket(uint8_t wavelength) {
	NS_ASSERT(wavelength > 0 && wavelength <= m_num_wavelength);
	NS_ASSERT(m_channel_state[wavelength] == PROPAGATING);

	m_channel_state[wavelength] = TRANSMITTING;
}

void
OBSFiber::TransmitEndBurstPacket(uint8_t wavelength) {
	NS_ASSERT(m_channel_state[wavelength] == TRANSMITTING);
	NS_ASSERT(wavelength > 0 && wavelength <= m_num_wavelength);

	uint32_t sender, dst;

	sender = m_current_sender[wavelength];

	// for each node is not the sender receives a notification about transmission end
	if (sender == 0) dst = 1;
	else dst = 0;

	m_points[dst]->ReceiveBurstEnd(wavelength, m_current_burst[wavelength]);

	m_channel_state[wavelength] = IDLE;
}

uint32_t
OBSFiber::GetNumChannels() {
	return m_num_wavelength;
}

uint32_t
OBSFiber::GetState(uint32_t wavelength) {
	return m_channel_state[wavelength];
}

bool
OBSFiber::AddDevice(Ptr<CoreDevice> dev, uint32_t &id) {
	NS_ASSERT(dev != NULL);

	if (m_id_gen == 2)
		return false;

	m_points[m_id_gen] = dev;
	id = m_id_gen;
	m_id_gen++;

	return true;
}

DataRate
OBSFiber::GetDataRate() {
	return m_bps;
}

Time
OBSFiber::GetDelay() {
	return m_delay;
}

};
