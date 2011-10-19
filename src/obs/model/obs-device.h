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

namespace ns3 {

enum TxMachineState
{
	READY,   /**< The transmitter is ready to begin transmission of a packet */
	BUSY,    /**< The transmitter is busy transmitting a packet */
	//GAP,      /**< The transmitter is in the interframe gap time */
	BACKOFF      /**< The transmitter is waiting for the channel to be free */
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

	Time              m_delay_to_process;
	Time              m_delay_to_conv_ele_opt;

	uint32_t          m_id;

	void SendBurstAfterConverting(uint32_t wavelength, Ptr<PacketBurst> pkt);
	void TransmitComplete(uint32_t wavelength);
	void FinishTransmitting();
	void ControlPacketConverted(Ptr<Packet> pkt);
	Ptr<OBSSwitch>     m_obs_switch;
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
	bool SendControlPacket(Mac48Address dst, uint32_t burst_id, uint32_t burst_size, uint32_t offset);

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

class CoreNodeDevice : public CoreDevice {
public:
	CoreNodeDevice();
};

class BorderNodeDevice : public CoreDevice {
public:
	BorderNodeDevice();
};

};

#endif
