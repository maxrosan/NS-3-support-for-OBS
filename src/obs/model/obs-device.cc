
#include "obs-device.h"
#include "obs-channel.h"

#include <ns3/simulator.h>

NS_LOG_COMPONENT_DEFINE("OBSDevice");

namespace ns3 {

OBSControlHeader::OBSControlHeader() {
	m_preamble = 0x7f;
}

OBSControlHeader::~OBSControlHeader() {
}

void
OBSControlHeader::SetSource(const Mac48Address &src) {
	src.CopyTo(m_src);
}

Mac48Address
OBSControlHeader::GetSource(void) {
	Mac48Address src;
	src.CopyFrom(m_src);
	return src;
}

void
OBSControlHeader::SetDestination(const Mac48Address &dst) {
	dst.CopyTo(m_dst);
}

Mac48Address
OBSControlHeader::GetDestination(void) {
	Mac48Address dst;
	dst.CopyFrom(m_dst);
	return dst;
}

void
OBSControlHeader::SetBurstID(uint32_t burst_id) {
	m_burst_id = burst_id;
}

uint32_t
OBSControlHeader::GetBurstID(void) {
	return m_burst_id;
}

void
OBSControlHeader::SetBurstSize(uint32_t burst_size) {
	m_burst_size = burst_size;
}

uint32_t
OBSControlHeader::GetBurstSize(void) {
	return m_burst_size;
}

void
OBSControlHeader::SetOffset(uint32_t offset) {
	m_offset  = offset;
}

uint32_t
OBSControlHeader::GetOffset(void) {
	return m_offset;
}

void
OBSControlHeader::SetChannel(uint32_t channel) {
	m_channel = channel;
}

uint32_t
OBSControlHeader::GetChannel(void) {
	return m_channel;
}

void
OBSControlHeader::SetNow(uint32_t now) {
	m_now = now;
}

uint32_t
OBSControlHeader::GetNow(void) {
	return m_now;
}

void
OBSControlHeader::SetDuration(uint32_t duration) {
	m_duration = duration;
}

uint32_t
OBSControlHeader::GetDuration(void) {
	return m_duration;
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
	start.WriteHtonU32(m_channel);
	start.WriteHtonU32(m_now);
	start.WriteHtonU32(m_duration);
}

uint32_t
OBSControlHeader::Deserialize(Buffer::Iterator start) {
	m_preamble = start.ReadU8();
	start.Read(m_src, sizeof m_src);
	start.Read(m_dst, sizeof m_dst);
	m_burst_id = start.ReadNtohU32();
	m_burst_size = start.ReadNtohU32();
	m_offset = start.ReadNtohU32();
	m_channel = start.ReadNtohU32();
	m_now = start.ReadNtohU32();
	m_duration = start.ReadNtohU32();

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
		(sizeof m_offset) +
		(sizeof m_channel) +
		(sizeof m_now) + 
		(sizeof m_duration)
		
	);
}

OBSPacketHeader::OBSPacketHeader() {
	m_proto_number = 0;
}

OBSPacketHeader::~OBSPacketHeader() {
}

void
OBSPacketHeader::SetProtocolNumber(uint32_t proto) {
	m_proto_number  = proto;
}

uint32_t
OBSPacketHeader::GetProtocolNumber(void) {
	return m_proto_number;
}

TypeId
OBSPacketHeader::GetTypeId(void) {
	static TypeId tid = TypeId("ns3::OBSPacketHeader")
		.SetParent<Header>()
		.AddConstructor<OBSPacketHeader>()
	;

	return tid;
}

TypeId
OBSPacketHeader::GetInstanceTypeId(void) const {
	return GetTypeId();
}

void
OBSPacketHeader::Print(std::ostream &os) const {
	os << "[ proto = " << m_proto_number << " ]";
}

void
OBSPacketHeader::Serialize(Buffer::Iterator start) const {
	start.WriteHtonU32(m_proto_number);
}

uint32_t
OBSPacketHeader::Deserialize(Buffer::Iterator start) {
	m_proto_number = start.ReadNtohU32();

	return GetSerializedSize();
}

uint32_t
OBSPacketHeader::GetSerializedSize(void) const {
	return (
		(sizeof m_proto_number)
		
	);
}

//

OBSRoutingTableTuple::OBSRoutingTableTuple() {
	m_metric = 0;
}

OBSRoutingTableTuple::OBSRoutingTableTuple(Ptr<CoreDevice> out, uint32_t metric) {
	m_out = out;
	m_metric = metric;
}

OBSRoutingTableTuple::OBSRoutingTableTuple(const OBSRoutingTableTuple &tuple) {
	m_out = tuple.m_out;
	m_metric = tuple.m_metric;
}

OBSRoutingTableTuple
OBSRoutingTableTuple::operator=(const OBSRoutingTableTuple &t) {
	m_out = t.m_out;
	m_metric = t.m_metric;

	return *this;
}

Ptr<CoreDevice>
OBSRoutingTableTuple::getDevice(void) {
	return m_out;
}

uint32_t
OBSRoutingTableTuple::getMetric(void) {
	return m_metric;
}

OBSSwitch::OBSSwitch() {

}

TypeId
OBSSwitch::GetTypeId(void) {
	static TypeId tid = TypeId("ns3::OBSwitch")
		.SetParent<Object>()
		.AddConstructor<OBSSwitch>();

	return tid;
}

void
OBSSwitch::AddRoute(Mac48Address addr, OBSRoutingTableTuple out) {
	m_routing_table[addr] = out;
}

void
OBSSwitch::ReceiveControlPacket(Ptr<Packet> control_pkt) {
	NS_LOG_INFO("Switch: control pkt received");

}

void
OBSSwitch::AddDevice(Ptr<CoreDevice> device) {
	m_devices.push_back(device);
}

void
OBSSwitch::PrintTable(std::ostream &os) {
	for (
	 std::map<Mac48Address, OBSRoutingTableTuple>::iterator it = 
	  m_routing_table.begin();
	 it != m_routing_table.end();
	 it++
	) {
		
		os << it->first << " => [" 
		 << Mac48Address::ConvertFrom(it->second.getDevice()->GetAddress())
		 << " ] " << std::endl;

	}
}

Ptr<CoreDevice>
OBSSwitch::GetFirstInterface() {
	NS_ASSERT(m_devices.empty() == false);
	return *m_devices.begin();
}

uint32_t
OBSSwitch::GetN() {
	return m_devices.size();
}


static double global_stop_time = -1.;

void
OBSSwitch::SetStopTime(double stop_time) {
	NS_ASSERT(stop_time > 0.);

	global_stop_time = stop_time;
}

double
OBSSwitch::GetStopTime(void) {
	return global_stop_time;
}

bool
OBSSwitch::GetTuple(Mac48Address dest, OBSRoutingTableTuple &t) {
	if (m_routing_table.find(dest) != m_routing_table.end()) {
		t = m_routing_table[dest];
		return true;
	}
	return false;
}

//

WavelengthReceiver::WavelengthReceiver() {
	wavelength = 0;
	state = OBS_READY;
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
		TimeValue(Time("30ms")),
		MakeTimeAccessor(&CoreDevice::m_delay_to_process),
		MakeTimeChecker())
	.AddAttribute("ConvertingEletricToOptical",
		"Delay for converting electrical signal into an optical one",
		TimeValue(Time("10ms")), //XXX: Unreal value
		MakeTimeAccessor(&CoreDevice::m_delay_to_conv_ele_opt),
		MakeTimeChecker())
	;
	
	return type;
}

CoreDevice::CoreDevice() {
}

void
CoreDevice::SetSwitch(Ptr<OBSSwitch> sw) {
	NS_ASSERT(sw != NULL);
	
	m_obs_switch = sw;
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
	
	m_wr_state[wavelength].state = OBS_BUSY;

	NS_LOG_INFO(m_addr << ": Receiving burst...");
}

//It is called by the channel as soon as the transmission is finished
void
CoreDevice::ReceiveBurstEnd(uint32_t wavelength, Ptr<PacketBurst> pkt) {
	NS_ASSERT(m_fiber != NULL);

	m_wr_state[wavelength].state = OBS_READY;

	if (m_callback_burst != NULL) {
		Simulator::Schedule(m_delay_to_process, m_callback_burst,
		 pkt->Copy());

		NS_LOG_INFO(m_addr << ": Burst transmission finished...");
	} else {
		NS_LOG_INFO(m_addr << ": Burst lost, no callback defined");
	}
}

void
CoreDevice::SetFiber(Ptr<OBSFiber> fiber) {
	uint32_t i;
	WavelengthReceiver wr;

	NS_ASSERT(fiber != NULL);
	NS_ASSERT(fiber->AddDevice(this, m_id) == true);

	m_fiber = fiber;

	NS_LOG_INFO(m_addr << ": ID = " << m_id);	
	NS_LOG_INFO(m_addr << ": Number of channels = " << fiber->GetNumChannels());

	for (i = 0; i < fiber->GetNumChannels(); i++) {
		wr.wavelength = i+1;
		wr.state = OBS_READY;
		m_wr_state.push_back(wr);

		m_mux.push_back(std::make_pair(Ptr<CoreDevice>(NULL), 0));
	}
}


void
CoreDevice::TransmitComplete(uint32_t wavelength) {
	NS_LOG_INFO(m_addr << ": Transmission complete for wavelength " << wavelength);

	m_fiber->TransmitEndBurstPacket(wavelength);
	m_wr_state[wavelength].state = OBS_READY;
}

// What I am supposed to do when the fiber gets busy as soon as the packet is converted and ready to go?
void
CoreDevice::SendBurstAfterConverting(uint32_t wavelength, Ptr<PacketBurst> pkt) {
	NS_ASSERT(m_fiber != NULL);
	NS_ASSERT(wavelength > 0 && wavelength <= m_fiber->GetNumChannels());

	NS_LOG_INFO(m_addr << ": Packet converted");

	if (m_fiber->GetState(wavelength) == OBS_IDLE) {
		Time transmit_time;
		Time time_for_propagation;
		DataRate data_rate;

		time_for_propagation = m_fiber->GetDelay();

		NS_LOG_INFO(m_addr << ": Fiber idle. Sending packet..."); 

		m_fiber->TransmitStartBurstPacket(pkt, wavelength, m_id);
		data_rate = m_fiber->GetDataRate();
		transmit_time = Seconds(data_rate.CalculateTxTime(pkt->GetSize())) + Seconds(time_for_propagation);

		NS_LOG_INFO(m_addr << ": Time to transmit : " << transmit_time.GetSeconds() << "s");

		Simulator::Schedule(transmit_time, &CoreDevice::TransmitComplete, this, wavelength);
	} else {
		NS_LOG_INFO(m_addr << ": Fiber is not idle");
		m_wr_state[wavelength].state = OBS_READY;
	}
}

//XXX: It doesn't avoid collisions
bool
CoreDevice::SendBurst(uint32_t wavelength, Ptr<PacketBurst> packet) {
	NS_ASSERT(m_fiber != NULL);
	NS_ASSERT(wavelength > 0 && wavelength <= m_fiber->GetNumChannels());

	Ptr<PacketBurst> copy_pkt = packet->Copy();

	NS_LOG_INFO(m_addr << ": Sending burst");

	if (m_wr_state[wavelength].state == OBS_READY && m_fiber->GetState(wavelength) == OBS_IDLE) {
		// Delay to convert eletric information into optical one
		Simulator::Schedule(m_delay_to_conv_ele_opt,
		 &CoreDevice::SendBurstAfterConverting,
		 this, wavelength, copy_pkt);

		m_wr_state[wavelength].state = OBS_BUSY;
	
		NS_LOG_INFO(m_addr << ": Converting packet... [" << m_delay_to_conv_ele_opt << "]");
	
	} else {
		NS_LOG_INFO(m_addr << ": Wavelength channel was not ready to go");
		return false;
	}

	return true;
}

uint32_t
CoreDevice::GetID(void) {
	return m_id;
}

void
CoreDevice::ControlPacketConverted(Ptr<Packet> pkt) {
	NS_ASSERT(pkt != NULL);
	NS_ASSERT(m_fiber != NULL);

	OBSControlHeader obsch;
	Time gap_value = Seconds(0.1);
	NS_ASSERT(pkt->PeekHeader(obsch) > 0);

	NS_LOG_INFO("Control packet:");
	NS_LOG_INFO("Src " << obsch.GetSource());
	NS_LOG_INFO("Dst " << obsch.GetDestination());
	NS_LOG_INFO("ID  " << obsch.GetBurstID());
	NS_LOG_INFO("Siz " << obsch.GetBurstSize());
	NS_LOG_INFO("Tim " << obsch.GetOffset());
	NS_LOG_INFO("Cnn " << obsch.GetChannel());
	NS_LOG_INFO("Now " << obsch.GetNow());

	if (m_obs_switch != NULL) {
		if (obsch.GetDestination() == m_addr) {
			NS_LOG_INFO(m_addr << ": This packet is to me");

			if (!m_callback_process_ctrl.IsNull()) {
				m_callback_process_ctrl(pkt);
			}
		} else {
			NS_LOG_INFO(m_addr << ": This packet is not to me");
			//m_obs_switch->ReceiveControlPacket(pkt);
			if (!m_callback_route_ctrl.IsNull()) {
				m_callback_route_ctrl(pkt);
			}
		}
	} else {
		NS_LOG_INFO("Switch not defined");
	}

}

void
CoreDevice::ReceiveControlPacket(Ptr<Packet> pkt) {
	NS_ASSERT(m_fiber != NULL);
	NS_ASSERT(pkt != NULL);

	Time delay_total = m_delay_to_conv_ele_opt + m_delay_to_process;

	NS_LOG_INFO(Simulator::Now().GetSeconds() << "s : " << m_addr << ": Control pkt received");

	Simulator::Schedule(delay_total, &CoreDevice::ControlPacketConverted, this, pkt);
}

void
CoreDevice::FinishTransmitting() {
	NS_ASSERT(m_fiber != NULL);

	m_fiber->TransmitEndControlPacket();

	if (!m_ctrlpkt_queue.empty()) {
		Ptr<Packet> pkt_ptr = m_ctrlpkt_queue.front();
		if (m_fiber->TransmitStartControlPacket(pkt_ptr, GetID())) {
			NS_LOG_INFO("Packet in queue was sent");
			m_ctrlpkt_queue.pop();
			TransmitPacketControl(pkt_ptr);
		} else {
			NS_LOG_INFO("Packet in queue failed to be sent");
		}
	}
}

bool
CoreDevice::SendControlPacket(Mac48Address dst, uint32_t burst_id, uint32_t burst_size, uint32_t offset, uint32_t channel, uint32_t now, uint32_t duration) {

	OBSControlHeader ch;

	ch.SetSource(m_addr);
	ch.SetDestination(dst);
	ch.SetBurstID(burst_id);
	ch.SetBurstSize(burst_size);
	ch.SetOffset(offset);
	ch.SetChannel(channel);
	ch.SetNow(now);
	ch.SetDuration(duration);

	return SendControlPacket(ch);
}

void
CoreDevice::TransmitPacketControl(Ptr<Packet> pkt) {
	Time transmit_time;
	DataRate data_rate;
	Time time_for_converting;

	NS_LOG_INFO(m_addr << ": Sending control packet");

	time_for_converting = m_delay_to_conv_ele_opt;
	data_rate = m_fiber->GetDataRate();
	transmit_time = Seconds(data_rate.CalculateTxTime(pkt->GetSize())) + time_for_converting;

	Simulator::Schedule(transmit_time,
	 &CoreDevice::FinishTransmitting,
	 this);	
}

void
CoreDevice::TryToRetransmit(void) {
	if (!m_ctrlpkt_queue.empty()) {
		Ptr<Packet> pkt_ptr = m_ctrlpkt_queue.front();
		if (m_fiber->TransmitStartControlPacket(pkt_ptr, GetID())) {
			NS_LOG_INFO("Packet in queue was sent");
			TransmitPacketControl(pkt_ptr);
			m_ctrlpkt_queue.pop();
		} else {
			double v;
			NS_LOG_INFO(Simulator::Now().GetSeconds() << "s#: Packet in queue failed to be sent");

			v = 0.1 + ((double) (rand() % 100))/1000.;
			Simulator::Schedule(Seconds(v), &CoreDevice::TryToRetransmit, this);
		}
	}
}

bool
CoreDevice::SendControlPacket(const OBSControlHeader &header) {

	static bool rand_gen = false;

	NS_ASSERT(m_fiber != NULL);

	if (!rand_gen) {
		rand_gen = true;
		srand(time(NULL));
	}

	Ptr<Packet> pkt_ptr;
	Packet pkt = Packet((uint8_t*) "1", 1);
	pkt.AddHeader(header);

	pkt_ptr = pkt.Copy();

	if (m_fiber->TransmitStartControlPacket(pkt_ptr, GetID())) {
		TransmitPacketControl(pkt_ptr);
		return true;
	} else {
		double v;

		NS_LOG_INFO(m_addr << ": Failed to send control packet. Pkt in queue...");
		m_ctrlpkt_queue.push(pkt_ptr);
		
		v = ((double) (rand() % 200))/1000.;
		Simulator::Schedule(Seconds(v), &CoreDevice::TryToRetransmit, this);
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
	return true;
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
CoreDevice::Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber)
{

	NS_LOG_INFO(m_addr << ": Sending a packet to " << Mac48Address::ConvertFrom(dest));

	return false;
}


bool
CoreDevice::SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber) {

	NS_LOG_INFO(m_addr << ": Sending(From) a packet to " << Mac48Address::ConvertFrom(dest));

	return false;

}


void
CoreDevice::RemoveSchedule(uint32_t channel, uint32_t id) {

}

bool
CoreDevice::Schedule(Time start, Time offset, uint32_t channel, Ptr<CoreDevice> dev, uint32_t &id) {
	return false;
}

CNDTuple::CNDTuple() {
}

CNDTuple::CNDTuple(const CNDTuple &t) {
	m_start = t.m_start;
	m_offset = t.m_offset;
	m_dev = t.m_dev;
	m_channel = t.m_channel;
	m_id = t.m_id;
}

CNDTuple
CNDTuple::operator=(const CNDTuple &t) {
	m_start = t.m_start;
	m_offset = t.m_offset;
	m_dev = t.m_dev;
	m_channel = t.m_channel;
	m_id = t.m_id;

	return *this;
}

CoreNodeDevice::CoreNodeDevice():
	CoreDevice()
{
	m_callback_route_ctrl = MakeCallback(&CoreNodeDevice::RouteControlPacket, this);
}

void
CoreNodeDevice::FreeChannel(CNDTuple tuple) {

	if (tuple.m_dev != NULL) {
		NS_LOG_INFO(Simulator::Now().GetSeconds() << "s " << Mac48Address::ConvertFrom(GetAddress()) << ": " <<
		 "Liberando " << Mac48Address::ConvertFrom(tuple.m_dev->GetAddress()) << ", " << m_map_channel[tuple.m_id].second);
	}

	RemoveSchedule(tuple.m_channel, tuple.m_id);
	m_map_channel.erase(tuple.m_id);
}

void
CoreNodeDevice::AllocChannel(CNDTuple tuple) {
	Simulator::Schedule(tuple.m_offset,
	 &CoreNodeDevice::FreeChannel, this, tuple);

	//XXX: Tem que verificar se o dispositivo vai receber e repassar para outro dispositivo ou somente receber
	//XXX: É só checar se tuple.m_dev == NULL

	if (tuple.m_dev != NULL) {
		NS_LOG_INFO(Simulator::Now().GetSeconds() << "s : " << Mac48Address::ConvertFrom(GetAddress()) << " "
		":  Repassar pacotes de [" << tuple.m_channel << "] para " << Mac48Address::ConvertFrom(tuple.m_dev->GetAddress()) <<
		"[" << m_map_channel[tuple.m_id].second << "]");
	}
}

void
CoreNodeDevice::RemoveSchedule(uint32_t channel, uint32_t id) {
	std::list<CNDTuple> &l = m_map_schedule[channel];
	std::list<CNDTuple>::iterator it;

	for (it = l.begin(); it != l.end(); it++) {
		if (it->m_id == id) {
			l.erase(it);
			break;
		}
	}
}

bool
CoreNodeDevice::Schedule(Time start, Time offset, uint32_t channel, Ptr<CoreDevice> dev, uint32_t &id) {
	NS_ASSERT(m_fiber != NULL);
	NS_ASSERT(channel > 0 && channel <= m_fiber->GetNumChannels());

	std::list<CNDTuple> &l = m_map_schedule[channel];
	std::list<CNDTuple>::iterator it, curr;
	CNDTuple tuple;
	bool inserted = false, ok_to_insert = true;
	static int id_schedule = 0;

	id_schedule++;

	if (Simulator::Now() > start)
		return false;

	tuple.m_start = start;
	tuple.m_offset = offset;
	tuple.m_dev = dev;
	id = tuple.m_id  = id_schedule;
	tuple.m_channel = channel;

	if (!l.empty()) {
		for (it = l.begin(); it != l.end() && !inserted && ok_to_insert; it++) {
			if ((start + offset) <=  it->m_start) {
				l.insert(it, tuple);
				inserted = true;
			} else {
				if (not (start >= (it->m_start + it->m_offset))) {
					ok_to_insert = false;
					NS_LOG_INFO("Reason: " << start.GetSeconds() << " >= " << it->m_start.GetSeconds() <<
					 " + " << it->m_offset.GetSeconds());
				}
			}
		}

		if (!inserted && ok_to_insert) {
			l.push_back(tuple);
			inserted = true;
		}
	} else {
		l.push_back(tuple);
		inserted = true;
	}

	if (inserted)
		Simulator::Schedule(Seconds((start - Simulator::Now()).GetSeconds()),
		 &CoreNodeDevice::AllocChannel, this, tuple);

	return inserted;
}

void
CoreNodeDevice::RouteControlPacket(Ptr<Packet> control_pkt) {
	OBSControlHeader header;

	NS_ASSERT(control_pkt != NULL);
	NS_ASSERT(m_obs_switch != NULL);

	if (control_pkt->PeekHeader(header) > 0) {
		uint32_t now = Simulator::Now().GetSeconds() * 1000.,
		         packet_start_time = header.GetNow();

		NS_LOG_INFO("Control packet");
		NS_LOG_INFO("Now " << now);
		NS_LOG_INFO("PktNow " << packet_start_time);
		NS_LOG_INFO("Offset " << header.GetOffset());
		NS_LOG_INFO("Duration " << header.GetDuration());
		NS_LOG_INFO("Destination " << header.GetDestination());

		// it checks if it is possible to receive the packet
		if ((packet_start_time + header.GetOffset()) > now) {
			OBSRoutingTableTuple tuple;

			if (m_obs_switch->GetTuple(header.GetDestination(), tuple)) {
				Time start = Seconds((packet_start_time + header.GetOffset()) / 1000.),
				     end   = Seconds(start.GetSeconds() + (header.GetDuration()/1000.));
				Time offset = end - start;

				uint32_t id1 = 0, id2 = 0;

				NS_LOG_INFO("Out found : " << start.GetSeconds() << " " << end.GetSeconds());
				NS_LOG_INFO("Out : " << Mac48Address::ConvertFrom(tuple.getDevice()->GetAddress()));
				//XXX: TENTAR ALOCAR INTERVALO DE TEMPO
				//XXX: Se conseguiu alocar tempo, faça:
				if (Schedule(start, offset, header.GetChannel(), tuple.getDevice(), id1)) {
					uint32_t channel;
					bool channel_found = false;
					// it looks for a channel that is able to send the burst as soon as possible
					for (channel = 1; (channel <= m_fiber->GetNumChannels()) && !channel_found; channel++) {
						if (tuple.getDevice()->Schedule(start, offset, header.GetChannel(), Ptr<CoreDevice>(NULL), id2)) {
							header.SetChannel(channel);
							tuple.getDevice()->SendControlPacket(header);
							channel_found = true;
						} else {
							tuple.getDevice()->RemoveSchedule(channel, id2);
						}
					}

					if (!channel_found) {
						NS_LOG_INFO("It was not possible to find a channel to receive the burst [1]");
						RemoveSchedule(header.GetChannel(), id1);
					} else {
						m_map_channel[id1] = std::make_pair(tuple.getDevice(), channel);
						NS_LOG_INFO("Ok. Scheduled");
					}
				} else {
					NS_LOG_INFO("It was not possible to find a channel to receive the burst [2]");
					RemoveSchedule(header.GetChannel(), id1);
				}
			} else
				NS_LOG_INFO("It is not possible to reach destination");
		} else {
			NS_LOG_INFO("Time expired to receive the packet");
		}
	}
}

bool
CoreNodeDevice::Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber) {
	
	NS_LOG_INFO("Unsupported operation for a core node");
	
	return false;
}

bool
CoreNodeDevice::SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber) {

	NS_LOG_INFO("Unsupported operation for a core node");

	return false;
}

BNDScheduleEntity::BNDScheduleEntity() {

}

BNDScheduleEntity::BNDScheduleEntity(uint32_t id, Time start, Time end, Ptr<PacketBurst> pkt, Mac48Address dest) {
	m_start  = start;
	m_end    = end;
	m_packet = pkt->Copy();
	m_id     = id;
	m_dest   = dest;
}

BNDScheduleEntity::BNDScheduleEntity(const BNDScheduleEntity &c) {
	m_start  = c.m_start;
	m_end    = c.m_end;
	m_packet = c.m_packet->Copy();
	m_id     = c.m_id;
	m_dest   = c.m_dest;
}

BNDScheduleEntity BNDScheduleEntity::operator=(const BNDScheduleEntity &c) {
	m_start  = c.m_start;
	m_end    = c.m_end;
	m_packet = c.m_packet->Copy();
	m_id     = c.m_id;
	m_dest   = c.m_dest;

	return *this;
}

BorderNodeDevice::BorderNodeDevice():
	CoreDevice()
{
	m_time_to_stop = -1.;
	m_fap_interval = 3.;

	m_callback_process_ctrl = MakeCallback(&BorderNodeDevice::ProcessControlPacketReceived, this);

	Simulator::Schedule(Seconds(m_fap_interval), &BorderNodeDevice::FAPCheck, this);
}

bool
BorderNodeDevice::ReceivePacket(Ptr<NetDevice>, Ptr<const Packet>, uint16_t, const Address&) {

	NS_LOG_INFO("PACKET RECEIVED");

	return false;
}

void
BorderNodeDevice::AddPort(Ptr<NetDevice> nd) {
	nd->SetReceiveCallback(MakeCallback(&BorderNodeDevice::ReceivePacket, this));
}

bool
BorderNodeDevice::Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber) {

	Mac48Address addr;

	addr = Mac48Address::ConvertFrom(dest);
	m_queue[addr].push(std::make_pair(packet->Copy(), protocolNumber));
	
	return false;
}

bool
BorderNodeDevice::SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber) {

	NS_LOG_INFO("Unsupported operation for a border node");

	return false;
}

void
BorderNodeDevice::SetStopTime(double time) {
	m_time_to_stop = time;	
}

void
BorderNodeDevice::SetFAPInterval(double interval) {
	m_fap_interval = interval;
}


void
BorderNodeDevice::BurstScheduling(BNDScheduleEntity e, uint32_t channel) {
	
	std::list<BNDScheduleEntity> &bursts = m_schedule[channel];
	std::list<BNDScheduleEntity>::iterator it;
	bool found = false;

	NS_LOG_INFO("Sending packet at " << e.m_start << " ; id = " << e.m_id);

	for (it = bursts.begin(); (it != bursts.end()) && !found; it++) {
		if (it->m_id == e.m_id) {
			found = true;
			bursts.erase(it);
		}
	}
}

void
BorderNodeDevice::ProcessControlPacketReceived(Ptr<Packet> pkt) {
	NS_LOG_INFO("Processing packet");
}

void
BorderNodeDevice::ScheduleBurst(Mac48Address dest, Ptr<PacketBurst> pb) {

	NS_ASSERT(m_fiber != NULL);

	uint32_t i;
	Time transmit_time;
	DataRate data_rate;
	Time GAP = Seconds(0.8);
	static uint32_t id = 1;
	BNDScheduleEntity new_burst;
	uint32_t curr_channel = 0;
	Time start_time = Seconds(10000000L); //XXX: !!
	uint32_t after_id = 0;
	
	new_burst.m_id     = id++;
	new_burst.m_packet = pb->Copy();
	new_burst.m_dest   = dest;

	data_rate = m_fiber->GetDataRate();
	transmit_time = Seconds(data_rate.CalculateTxTime(pb->GetSize())) + m_delay_to_conv_ele_opt + GAP + GAP; // [ Other burster ]__ [ GAP ] [ Transmitting packet + delay ] [ GAT ] __ [ Other burst ]


	//NS_LOG_INFO("Delay = " << m_delay_to_conv_ele_opt.GetSeconds());
	//NS_LOG_INFO("PKT SIZE = " << pb->GetSize());
	//NS_LOG_INFO("Transmit time = " << transmit_time.GetSeconds());

	for (i = 1; i <= m_fiber->GetNumChannels(); i++) {
		std::list<BNDScheduleEntity>::iterator it;
		std::list<BNDScheduleEntity> &l = m_schedule[i];

		//NS_LOG_INFO("Verifying channel " << i);
		
		if (l.empty()) {
			Time m_start;
			
			m_start = Seconds(Simulator::Now().GetSeconds());
			if (m_start < start_time) {
				start_time = m_start;
				curr_channel = i;
				after_id = 0;
			}

			//NS_LOG_INFO("Empty channel " << curr_channel << " " << m_start.GetSeconds() << " " << m_start.GetSeconds());

		} else {
			bool found = false;

			for (it = l.begin(); it != l.end();) {
				std::list<BNDScheduleEntity>::iterator curr;

				if (it->m_end < Simulator::Now()) {
					it++;
					continue;
				}

				curr = it;
				it++;
				found = true;

				if (it != l.end()) {
					if ((it->m_start - curr->m_end) >= transmit_time) {
						if (start_time > curr->m_end) {
							curr_channel = i;
							start_time = curr->m_end;
							it = l.end();
							after_id = curr->m_id;
						}
					}
				} else {

					//NS_LOG_INFO("Here " << start_time.GetSeconds() << " " << curr->m_end.GetSeconds());

					if (start_time > curr->m_end) {
						curr_channel = i;
						start_time = curr->m_end;
						after_id = curr->m_id;
					}
				}
			}

			if (!found) {
				Time m_start;

				m_start = Simulator::Now();
				if (m_start < start_time) {
					start_time = m_start;
					curr_channel = i;
					after_id = 0;
				}
			}
		}
	}


	{
		std::list<BNDScheduleEntity>::iterator it, end_it;

		new_burst.m_start = start_time + GAP;
		new_burst.m_end = Seconds(new_burst.m_start.GetSeconds() + transmit_time.GetSeconds() - GAP.GetSeconds());

		//NS_LOG_INFO("BURST [ " << new_burst.m_start.GetSeconds() << " " << new_burst.m_end.GetSeconds() << "]{" << transmit_time.GetSeconds() << "} [" << curr_channel << "] # " << after_id << " #");

		end_it = m_schedule[curr_channel].end();
		it = m_schedule[curr_channel].begin();

		if (after_id == 0)
			m_schedule[curr_channel].push_back(new_burst);
		else {
			for (; it != end_it; it++) {
				if (it->m_id == after_id) {
					it++;
					if (it == end_it) {
						m_schedule[curr_channel].push_back(new_burst);
						//NS_LOG_INFO("At last position");
					} else {
						m_schedule[curr_channel].insert(it, new_burst);
						//NS_LOG_INFO("After " << after_id);
						break;
					}
				}
			}
		}
	}

	NS_LOG_INFO(__func__ <<
	 " channel = " << curr_channel << "\n" <<
	 " now    = " << Simulator::Now().GetSeconds() << "s\n" <<
	 " start  = " << new_burst.m_start.GetSeconds() << "s\n"
	 " schedule at " << ((new_burst.m_start) - Simulator::Now()).GetSeconds()
	);

	GenerateControlPacket(curr_channel, new_burst);

	Simulator::Schedule(Seconds((new_burst.m_start - Simulator::Now()).GetSeconds()), &BorderNodeDevice::BurstScheduling, this, new_burst, curr_channel);
}


void
BorderNodeDevice::GenerateControlPacket(uint32_t channel, BNDScheduleEntity e) {
	NS_ASSERT(m_obs_switch != NULL);

	OBSRoutingTableTuple tuple;

	if (m_obs_switch->GetTuple(e.m_dest, tuple)) {
		uint32_t pkttime, now, duration;
		NS_ASSERT(tuple.getDevice() != NULL);

		pkttime = (uint32_t) (((e.m_start) - Simulator::Now()).GetSeconds() * 1000.);
		now     = (uint32_t) (Simulator::Now().GetSeconds() * 1000.);
		duration = (uint32_t) ((e.m_end - e.m_start).GetSeconds() * 1000.);

		NS_LOG_INFO("Route found to " << e.m_dest);
		
		tuple.getDevice()->SendControlPacket(e.m_dest, e.m_id, e.m_packet->GetSize(), pkttime, channel, now, duration);
	} else {
		NS_LOG_INFO("Route not found to " << e.m_dest);
	}
}

void
BorderNodeDevice::FAPCheck(void) {
	std::map<Mac48Address, std::queue<std::pair<Ptr<Packet>, uint16_t> > >::iterator it;

	NS_LOG_INFO(Mac48Address::ConvertFrom(GetAddress()) << ": Checking FAP");

	for (it = m_queue.begin(); it != m_queue.end(); it++) {
		uint32_t total_size = 0;
		Ptr<PacketBurst> pb = CreateObject<PacketBurst>();
		NS_LOG_INFO(it->first << " has packet to receive from me : " << it->second.size());
		while (!it->second.empty() &&
		 (it->second.front().first->GetSize() + total_size) < m_fap_size_limit) {
			Ptr<Packet> pkt;
			OBSPacketHeader obsh;
			total_size += it->second.front().first->GetSize();
			obsh.SetProtocolNumber(it->second.front().second);
			pkt = it->second.front().first;
			pkt->AddHeader(obsh);
			NS_LOG_INFO("Burst: + " << pkt->GetSize());
			it->second.pop();
			pb->AddPacket(pkt->Copy());
		}
		if (total_size > 0) {
			ScheduleBurst(it->first, pb);
		}
	}

	if (Simulator::Now() <= Seconds(m_time_to_stop)) {
		Simulator::Schedule(Seconds(m_fap_interval), &BorderNodeDevice::FAPCheck, this);
	}
}

void
BorderNodeDevice::SetFAPSizeLimit(double size) {
	m_fap_size_limit = size;
}

};
