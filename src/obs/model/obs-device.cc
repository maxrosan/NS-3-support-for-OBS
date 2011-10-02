
#include "obs-device.h"
#include "obs-channel.h"

#include <ns3/simulator.h>

namespace ns3 {

WavelengthReceiver::WavelengthReceiver() {
	wavelength = 0;
	state = READY;
}

void
WavelengthReceiver::CopyFrom(const WavelengthReceiver &wr) {
	wavelength = wr.wavelength;
	state      = wr.state;
}

WavelengthReceiver::WavelengthReceiver(const WavelengthReceiver &wr) {
	CopyFrom(wr);
}

WavelengthReceiver
WavelengthReceiver::operator=(const WavelengthReceiver &wr) {
	CopyFrom(wr);
	return *this;
}

TypeId
CoreDevice::GetTypeId() {
	static TypeId type = TypeId("ns3::CoreDevice")
	 .SetParent<NetDevice>()
	.AddAttribute("ProcessingDelay",
		"Processing delay",
		// XXX: Hypothetical value. A more realistic value
		// should be used
		TimeValue(Time("300ns")),
		MakeTimeAccessor(&CoreDevice::m_delay_to_process),
		MakeTimeChecker())
	.AddAttribute("ConvertingEletricToOptical",
		"Delay for converting electrical signal into an optical one",
		TimeValue(Time("100ns")), //XXX: Unreal value
		MakeTimeAccessor(&CoreDevice::m_delay_to_conv_ele_opt),
		MakeTimeChecker())
	;
	
	return type;
}

CoreDevice::CoreDevice() {
	
}

void
CoreDevice::ChangeRoute(uint32_t wavelength, Ptr<CoreDevice> cd, uint32_t wavelength_dst) {
	NS_ASSERT(m_fiber != NULL);

	m_mux[wavelength] = std::make_pair(cd, wavelength);
}

//It is called by the channel as soon as the transmission is finished
void
CoreDevice::ReceiveBurstStart(uint32_t wavelength) {
	NS_ASSERT(m_fiber != NULL);
	
	m_wr_state[wavelength].state = BUSY;
}

//It is called by the channel as soon as the transmission is finished
void
CoreDevice::ReceiveBurstEnd(uint32_t wavelength, Ptr<PacketBurst> pkt) {
	NS_ASSERT(m_fiber != NULL);

	if (m_callback_burst != NULL) {
		Simulator::Schedule(m_delay_to_process, m_callback_burst, pkt->Copy());
	}
}

void
CoreDevice::SetFiber(Ptr<OBSFiber> fiber) {
	uint32_t i;
	WavelengthReceiver wr;

	NS_ASSERT(fiber != NULL);
	NS_ASSERT(fiber->AddDevice(this, m_id) == true);

	m_fiber = fiber;

	for (i = 0; i < fiber->GetNumChannels(); i++) {
		wr.wavelength = i+1;
		wr.state = READY;
		m_wr_state.push_back(wr);

		m_mux.push_back(std::make_pair(Ptr<CoreDevice>(NULL), 0));
	}
}


void
CoreDevice::TransmitComplete(uint32_t wavelength) {

}

void
CoreDevice::SendBurstAfterConverting(uint32_t wavelength, Ptr<PacketBurst> pkt) {
	NS_ASSERT(m_fiber != NULL);
	NS_ASSERT(wavelength > 0 && wavelength <= m_fiber->GetNumChannels());

	if (m_fiber->GetState(wavelength) == IDLE) {
		Time transmit_time;
		DataRate data_rate;

		m_fiber->TransmitStartBurstPacket(pkt, wavelength, m_id);
		data_rate = m_fiber->GetDataRate();
		transmit_time = Seconds(data_rate.CalculateTxTime(pkt->GetSize()));

		Simulator::Schedule(transmit_time, &CoreDevice::TransmitComplete, this, wavelength);
	} else {
		m_wr_state[wavelength].state = READY;
	}
}

//XXX: It doesn't avoid collisions
bool
CoreDevice::SendBurst(uint32_t wavelength, Ptr<PacketBurst> packet) {
	NS_ASSERT(m_fiber != NULL);
	NS_ASSERT(wavelength > 0 && wavelength <= m_fiber->GetNumChannels());

	Ptr<PacketBurst> copy_pkt = packet->Copy();

	if (m_wr_state[wavelength].state == READY && m_fiber->GetState(wavelength) == IDLE) {
		Simulator::Schedule(m_delay_to_conv_ele_opt,
		 &CoreDevice::SendBurstAfterConverting,
		 this, wavelength, copy_pkt);

		m_wr_state[wavelength].state = BUSY;
	} else
		return false;

	return true;
}

void
CoreDevice::SetIfIndex(const uint32_t index) {
	m_ifindex = index;
}

uint32_t
CoreDevice::GetIfIndex(void) const {
	return m_ifindex;
}

Ptr<Channel>
CoreDevice::GetChannel(void) const {
	return Ptr<Channel>(m_fiber);
}

void
CoreDevice::SetAddress(Address address) {
	m_addr = Mac48Address::ConvertFrom(address);
}

Address
CoreDevice::GetAddress(void) const {
	return m_addr;
}

bool
CoreDevice::SetMtu (const uint16_t mtu) {
	m_mtu = mtu;
	return true;
}

uint16_t
CoreDevice::GetMtu (void) const {
	return m_mtu;
}

bool
CoreDevice::IsLinkUp(void) const {
	return true;
}

void
CoreDevice::AddLinkChangeCallback(Callback<void> callback) {
	m_link_change_cb = callback;
}

bool
CoreDevice::IsBroadcast(void) const {
	return false;
}

Address
CoreDevice::GetBroadcast(void) const {
	return Mac48Address ("ff:ff:ff:ff:ff:ff");
}

bool
CoreDevice::IsMulticast (void) const {
	return false;
}

Address
CoreDevice::GetMulticast (Ipv4Address multicastGroup) const {
	return Mac48Address::GetMulticast(multicastGroup);
}

Address
CoreDevice::GetMulticast (Ipv6Address multicastGroup) const {
	return Mac48Address::GetMulticast(multicastGroup);
}

bool
CoreDevice::IsBridge(void) const {
	return false;
}

bool
CoreDevice::IsPointToPoint(void) const {
	return true;
}

Ptr<Node>
CoreDevice::GetNode(void) const {
	return m_node;
}

void
CoreDevice::SetNode(Ptr<Node> node) {
	m_node = node;
}

bool
CoreDevice::NeedsArp (void) const {
	return true;
}

void
CoreDevice::SetReceiveCallback(ReceiveCallback cb) {
	m_rcv_cb = cb;
}

void
CoreDevice::SetPromiscReceiveCallback(PromiscReceiveCallback cb) {
	m_pms_rcv_cb = cb;
}

bool
CoreDevice::SupportsSendFrom(void) const {
	return true;
}

bool
CoreDevice::Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber) {

	return false;
}


bool
CoreDevice::SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber) {

	return false;

}


};
