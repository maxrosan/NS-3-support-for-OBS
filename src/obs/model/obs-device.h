#ifndef OBSDEVICE_H
#define OBSDEVICE_H

#include "ns3/node.h"
#include "ns3/backoff.h"
#include "ns3/address.h"
#include "ns3/net-device.h"
#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/ptr.h"
#include "ns3/random-variable.h"
#include "ns3/mac48-address.h"
#include "ns3/packet-burst.h"
#include "ns3/object-factory.h"
#include "ns3/log.h"
#include "ns3/header.h"

#include <map>
#include <utility>
#include <queue>

namespace ns3 {

enum TxMachineState
{
	OBS_READY,   /**< The transmitter is ready to begin transmission of a packet */
	OBS_BUSY,    /**< The transmitter is busy transmitting a packet */
	//GAP,      /**< The transmitter is in the interframe gap time */
	OBS_BACKOFF      /**< The transmitter is waiting for the channel to be free */
};

// Packets

class OBSControlHeader : public Header {
private:
	uint8_t      m_preamble;
	uint8_t      m_src[6];
	uint8_t      m_dst[6];
	uint32_t     m_burst_id;
	uint32_t     m_burst_size;
	uint32_t     m_offset; // time needed for the burst get the router (in ms)
	uint32_t     m_channel;
	uint32_t     m_now;
public:
	OBSControlHeader();
	virtual ~OBSControlHeader();

	void SetSource(const Mac48Address &src);
	Mac48Address GetSource(void);
	void SetDestination(const Mac48Address &dst);
	Mac48Address GetDestination(void);
	void SetBurstID(uint32_t burst_id);
	uint32_t GetBurstID(void);
	void SetBurstSize(uint32_t burst_size);
	uint32_t GetBurstSize(void);
	void SetOffset(uint32_t offset);
	uint32_t GetOffset(void);
	void SetChannel(uint32_t channel);
	uint32_t GetChannel(void);
	void SetNow(uint32_t now);
	uint32_t GetNow(void);

	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	virtual void Print (std::ostream &os) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);
	virtual uint32_t GetSerializedSize (void) const;

};


class OBSPacketHeader : public Header {
private:
	uint32_t     m_proto_number;
public:
	OBSPacketHeader();
	virtual ~OBSPacketHeader();

	void SetProtocolNumber(uint32_t offset);
	uint32_t GetProtocolNumber(void);

	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	virtual void Print (std::ostream &os) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);
	virtual uint32_t GetSerializedSize (void) const;

};

//

class CoreDevice;
class OBSRoutingTableTuple {
private:
	Ptr<CoreDevice> m_out;
	uint32_t        m_metric;
public:
	OBSRoutingTableTuple();
	OBSRoutingTableTuple(Ptr<CoreDevice> out, uint32_t metric);
	OBSRoutingTableTuple(const OBSRoutingTableTuple &tuple);
	OBSRoutingTableTuple operator=(const OBSRoutingTableTuple &t);
	Ptr<CoreDevice> getDevice(void);
	uint32_t        getMetric(void);
};

class OBSFiber;
class OBSSwitch : public Object {
private:
	std::map<Mac48Address, OBSRoutingTableTuple> m_routing_table;
	std::list<Ptr<CoreDevice> > m_devices;
public:
	static TypeId GetTypeId(void);

	OBSSwitch();
	void AddRoute(Mac48Address, OBSRoutingTableTuple out);
	void ReceiveControlPacket(Ptr<Packet> control_pkt);
	void AddDevice(Ptr<CoreDevice> device);
	void PrintTable(std::ostream &os);
	Ptr<CoreDevice> GetFirstInterface();
	uint32_t GetN(void);
	static void SetStopTime(double stop_time);
	static double GetStopTime(void);
	bool GetTuple(Mac48Address, OBSRoutingTableTuple &t);
};

struct WavelengthReceiver {
	uint32_t       wavelength;
	TxMachineState state;

	WavelengthReceiver();
	WavelengthReceiver(const WavelengthReceiver &wr);
	void CopyFrom(const WavelengthReceiver &wr);
	WavelengthReceiver operator=(const WavelengthReceiver &wr);
};

class CoreDevice : public NetDevice{
protected:
	// m_mux : Wavelength -> (Device, Wavelength)
	std::vector<std::pair<Ptr<CoreDevice>, uint32_t> > m_mux;
	
	uint32_t          m_ifindex;
	Ptr<OBSFiber>     m_fiber;
	Mac48Address      m_addr;
	uint16_t          m_mtu;
	Callback<void>    m_link_change_cb;
	Ptr<Node>         m_node;
	ReceiveCallback   m_rcv_cb;
	PromiscReceiveCallback m_pms_rcv_cb;
	std::vector<WavelengthReceiver> m_wr_state;
	void (*m_callback_burst) (Ptr<PacketBurst>);
	std::queue<Ptr<Packet> > m_ctrlpkt_queue;

	Time              m_delay_to_process;
	Time              m_delay_to_conv_ele_opt;

	uint32_t          m_id;

	void SendBurstAfterConverting(uint32_t wavelength, Ptr<PacketBurst> pkt);
	void TransmitComplete(uint32_t wavelength);
	void FinishTransmitting();
	void ControlPacketConverted(Ptr<Packet> pkt);
	Ptr<OBSSwitch>     m_obs_switch;
	void TransmitPacketControl(Ptr<Packet> pkt);
	void TryToRetransmit(void);

	Callback<void, Ptr<Packet> > m_callback_route_ctrl;
	Callback<void, Ptr<Packet> > m_callback_process_ctrl;
public:

	static TypeId GetTypeId(void);

	CoreDevice();

	void SetSwitch(Ptr<OBSSwitch> sw);
	void ChangeRoute(uint32_t wavelength, Ptr<CoreDevice> cd, uint32_t wavelength_dst);
	void ReceiveBurstStart(uint32_t wavelength);
	void ReceiveBurstEnd(uint32_t wavelength, Ptr<PacketBurst> pb);
	void SetFiber(Ptr<OBSFiber> fiber);
	bool SendBurst(uint32_t wavelength, Ptr<PacketBurst> pkt);
	uint32_t GetID();
	void ReceiveControlPacket(Ptr<Packet> pkt);
	bool SendControlPacket(const OBSControlHeader &header);
	bool SendControlPacket(Mac48Address dst, uint32_t burst_id, uint32_t burst_size, uint32_t offset, uint32_t channel, uint32_t now);

	bool ScheduleBurst(Ptr<PacketBurst> burst, double &time);

	virtual void SetIfIndex (const uint32_t index);
	virtual uint32_t GetIfIndex (void) const;
	virtual Ptr<Channel> GetChannel (void) const;
	virtual void SetAddress (Address address);
	virtual Address GetAddress (void) const;
	virtual bool SetMtu (const uint16_t mtu);
	virtual uint16_t GetMtu (void) const;
	virtual bool IsLinkUp (void) const;
	virtual void AddLinkChangeCallback (Callback<void> callback);
	virtual bool IsBroadcast (void) const;
	virtual Address GetBroadcast (void) const;
	virtual bool IsMulticast (void) const;
	virtual Address GetMulticast (Ipv4Address multicastGroup) const;
	virtual Address	GetMulticast (Ipv6Address addr) const;
	virtual bool IsBridge (void) const;
	virtual bool IsPointToPoint (void) const;
	virtual bool Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber);
	virtual bool SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber);
	virtual Ptr<Node> GetNode (void) const;
	virtual void SetNode (Ptr< Node > node);
	virtual bool NeedsArp (void) const;
	virtual void SetReceiveCallback (ReceiveCallback cb);
	virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
	virtual bool SupportsSendFrom (void) const;

};

struct CNDTuple {
	Time            m_start;
	Time            m_offset;
	Ptr<CoreDevice> m_dev;

	CNDTuple();
	CNDTuple(const CNDTuple &t);
	CNDTuple operator=(const CNDTuple &t);
};

class CoreNodeDevice : public CoreDevice {
private:
	void RouteControlPacket(Ptr<Packet> control_pkt);
	std::map<uint32_t, std::list<CNDTuple> > m_map_schedule;

	void FreeChannel(CNDTuple tuple);
	void AllocChannel(CNDTuple tuple);
	bool Schedule(Time start, Time offset, uint32_t channel, Ptr<CoreDevice> dev);
public:
	CoreNodeDevice();
	virtual bool Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber);
	virtual bool SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber);
};

struct BNDScheduleEntity{
	Time             m_start,    
	                 m_end;
	Ptr<PacketBurst> m_packet;
	Mac48Address     m_dest;
	uint32_t         m_id;

	BNDScheduleEntity();
	BNDScheduleEntity(uint32_t id, Time start, Time end, Ptr<PacketBurst> pkt, Mac48Address dest);
	BNDScheduleEntity(const BNDScheduleEntity &c);
	BNDScheduleEntity operator=(const BNDScheduleEntity &c);
};

class BorderNodeDevice : public CoreDevice {
private:
	bool ReceivePacket(Ptr<NetDevice>, Ptr<const Packet>, uint16_t, const Address&);
	std::map<Mac48Address, std::queue<std::pair<Ptr<Packet>,uint16_t> > > m_queue;
	std::map<uint32_t, std::list<BNDScheduleEntity> > m_schedule; // wavelength -> burst
	double m_time_to_stop;
	double m_fap_interval;
	double m_fap_size_limit;
	void FAPCheck(void);
	void BurstScheduling(BNDScheduleEntity e, uint32_t channel);
	void GenerateControlPacket(uint32_t channel, BNDScheduleEntity e);
	void ProcessControlPacketReceived(Ptr<Packet> pkt);
public:
	BorderNodeDevice();
	void AddPort(Ptr<NetDevice> nd);
	virtual bool Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber);
	virtual bool SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber);
	void SetStopTime(double time);
	void SetFAPInterval(double interval);
	void SetFAPSizeLimit(double size);
	void ScheduleBurst(Mac48Address dest, Ptr<PacketBurst> pb);
};

};

#endif
