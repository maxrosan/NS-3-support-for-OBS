
#include "obs-device.h"
#include "obs-channel.h"

#include <ns3/simulator.h>

NS_LOG_COMPONENT_DEFINE("OBSDevice");

namespace ns3 {

OBSControlHeader::OBSControlHeader() {
}

OBSControlHeader::~OBSControlHeader() {
}

void
OBSControlHeader::SetSource(const Mac48Address &src) {
	src.CopyTo(m_src);
}

void
OBSControlHeader::SetDestination(const Mac48Address &dst) {
	dst.CopyTo(m_dst);
}

void
OBSControlHeader::SetBurstID(uint32_t burst_id) {
	m_burst_id = burst_id;
}

void
OBSControlHeader::SetBurstSize(uint32_t burst_size) {
	m_burst_size = burst_size;
}

void
OBSControlHeader::SetOffset(uint32_t offset) {
	m_offset  = m_offset;
}

TypeId
OBSControlHeader::GetTypeId(void) {
	static TypeId tid = TypeId("ns3::OBSControlHeader")
		.SetParent<Header>()
		.AddConstructor<OBSControlHeader>()
	;

	return tid;
}

TypeId
OBSControlHeader::GetInstanceTypeId(void) const {
	return GetTypeId();
}

void
OBSControlHeader::Print(std::ostream &os) const {
	os << "[ src = " << m_src << " | dst = " << m_dst << " | id = " << m_burst_id << " | burst size = " << m_burst_size << " | offset = " << m_offset << " ]";
}

void
OBSControlHeader::Serialize(Buffer::Iterator start) const {
	start.WriteU8(m_preamble);
	start.Write(m_src, sizeof m_src);
	start.Write(m_dst, sizeof m_dst);
	start.WriteHtonU32(m_burst_id);
	start.WriteHtonU32(m_burst_size);
	start.WriteHtonU32(m_offset);
}

uint32_t
OBSControlHeader::Deserialize(Buffer::Iterator start) {
	m_preamble = start.ReadU8();
	start.Read(m_src, sizeof m_src);
	start.Read(m_dst, sizeof m_dst);
	m_burst_id = start.ReadNtohU32();
	m_burst_size = start.ReadNtohU32();
	m_offset = start.ReadNtohU32();

	return GetSerializedSize();
}

uint32_t
OBSControlHeader::GetSerializedSize(void) const {
	return (
		(sizeof m_preamble) +
		(sizeof m_src) +
		(sizeof m_dst) +
		(sizeof m_burst_id) +
		(sizeof m_burst_size) +
		(sizeof m_offset)
		
	);
}

//

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

	m_mux[wavelength] = std::make_pair(cd, wavelength_dst);

	NS_LOG_INFO("Changing Route: (Node " << GetID() << ")[WL " << wavelength << "] ---> (Node " << cd->GetID() << ")[WL " << wavelength_dst << "]");
}

//It is called by the channel as soon as the transmission is started
void
CoreDevice::ReceiveBurstStart(uint32_t wavelength) {
	NS_ASSERT(m_fiber != NULL);
	
	m_wr_state[wavelength].state = BUSY;

	NS_LOG_INFO("Receiving burst...");
}

//It is called by the channel as soon as the transmission is finished
void
CoreDevice::ReceiveBurstEnd(uint32_t wavelength, Ptr<PacketBurst> pkt) {
	NS_ASSERT(m_fiber != NULL);

	m_wr_state[wavelength].state = READY;

	if (m_callback_burst != NULL) {
		Simulator::Schedule(m_delay_to_process, m_callback_burst,
		 pkt->Copy());

		NS_LOG_INFO("Burst transmission finished...");
	} else {
		NS_LOG_INFO("Burst lost, no callback defined");
	}
}

void
CoreDevice::SetFiber(Ptr<OBSFiber> fiber) {
	uint32_t i;
	WavelengthReceiver wr;

	NS_ASSERT(fiber != NULL);
	NS_ASSERT(fiber->AddDevice(this, m_id) == true);

	m_fiber = fiber;

	NS_LOG_INFO("ID = " << m_id);	
	NS_LOG_INFO("Number of channels = " << fiber->GetNumChannels());

	for (i = 0; i < fiber->GetNumChannels(); i++) {
		wr.wavelength = i+1;
		wr.state = READY;
		m_wr_state.push_back(wr);

		m_mux.push_back(std::make_pair(Ptr<CoreDevice>(NULL), 0));
	}
}


void
CoreDevice::TransmitComplete(uint32_t wavelength) {
	NS_LOG_INFO(GetID() << ": Transmission complete for wavelength " << wavelength);

	m_fiber->TransmitEndBurstPacket(wavelength);
	m_wr_state[wavelength].state = READY;
}

// What I am supposed to do when the fiber gets busy as soon as the packet is converted and ready to go?
void
CoreDevice::SendBurstAfterConverting(uint32_t wavelength, Ptr<PacketBurst> pkt) {
	NS_ASSERT(m_fiber != NULL);
	NS_ASSERT(wavelength > 0 && wavelength <= m_fiber->GetNumChannels());

	NS_LOG_INFO("Packet converted");

	if (m_fiber->GetState(wavelength) == IDLE) {
		Time transmit_time;
		Time time_for_propagation;
		DataRate data_rate;

		time_for_propagation = m_fiber->GetDelay();

		NS_LOG_INFO(GetID() << ": Fiber idle. Sending packet..."); 

		m_fiber->TransmitStartBurstPacket(pkt, wavelength, m_id);
		data_rate = m_fiber->GetDataRate();
		transmit_time = Seconds(data_rate.CalculateTxTime(pkt->GetSize())) + Seconds(time_for_propagation);

		NS_LOG_INFO(GetID() << ": Time to transmit : " << transmit_time.GetSeconds() << "s");

		Simulator::Schedule(transmit_time, &CoreDevice::TransmitComplete, this, wavelength);
	} else {
		NS_LOG_INFO(GetID() << ": Fiber is not idle");
		m_wr_state[wavelength].state = READY;
	}
}

//XXX: It doesn't avoid collisions
bool
CoreDevice::SendBurst(uint32_t wavelength, Ptr<PacketBurst> packet) {
	NS_ASSERT(m_fiber != NULL);
	NS_ASSERT(wavelength > 0 && wavelength <= m_fiber->GetNumChannels());

	Ptr<PacketBurst> copy_pkt = packet->Copy();

	NS_LOG_INFO("Sending burst");

	if (m_wr_state[wavelength].state == READY && m_fiber->GetState(wavelength) == IDLE) {
		// Delay to convert eletric information into optical one
		Simulator::Schedule(m_delay_to_conv_ele_opt,
		 &CoreDevice::SendBurstAfterConverting,
		 this, wavelength, copy_pkt);

		m_wr_state[wavelength].state = BUSY;
	
		NS_LOG_INFO("Converting packet...");
	
	} else {
		NS_LOG_INFO("Wavelength channel was not ready to go");
		return false;
	}

	return true;
}

uint32_t
CoreDevice::GetID(void) {
	return m_id;
}

void
CoreDevice::ReceiveControlPacket(Ptr<Packet> pkt) {
	NS_ASSERT(m_fiber != NULL);
	NS_ASSERT(pkt != NULL);

	NS_LOG_INFO("Control pkt received");
}

void
CoreDevice::FinishTransmitting() {
	NS_ASSERT(m_fiber != NULL);

	m_fiber->TransmitEndControlPacket();
}

bool
CoreDevice::SendControlPacket(Mac48Address dst, uint32_t burst_id, uint32_t burst_size, uint32_t offset) {

	OBSControlHeader ch;

	ch.SetSource(m_addr);
	ch.SetDestination(dst);
	ch.SetBurstID(burst_id);
	ch.SetBurstSize(burst_size);
	ch.SetOffset(offset);

	return SendControlPacket(ch);
}

bool
CoreDevice::SendControlPacket(const OBSControlHeader &header) {

	NS_ASSERT(m_fiber != NULL);

	Ptr<Packet> pkt_ptr;
	Packet pkt = Packet((uint8_t*) "1", 1);
	pkt.AddHeader(header);

	pkt_ptr = pkt.Copy();

	if (m_fiber->TransmitStartControlPacket(pkt_ptr, GetID())) {
		Time transmit_time;
		DataRate data_rate;
		Time time_for_converting;

		NS_LOG_INFO(GetID() << ": Sending control packet");

		time_for_converting = m_delay_to_conv_ele_opt;
		data_rate = m_fiber->GetDataRate();
		transmit_time = Seconds(data_rate.CalculateTxTime(pkt_ptr->GetSize())) + Seconds(time_for_converting);
		
		Simulator::Schedule(transmit_time,
		 &CoreDevice::FinishTransmitting,
		 this);	

		return true;
	} else {
		NS_LOG_INFO(GetID() << ": Failed to send control packet");
	}

	return false;
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
