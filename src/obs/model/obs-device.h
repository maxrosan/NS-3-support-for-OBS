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

struct WavelengthReceiver {
	uint32_t       wavelength;
	TxMachineState state;

	WavelengthReceiver();
	WavelengthReceiver(const WavelengthReceiver &wr);
	void CopyFrom(const WavelengthReceiver &wr);
	WavelengthReceiver operator=(const WavelengthReceiver &wr);
};

class OBSFiber;
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
public:

	static TypeId GetTypeId(void);

	CoreDevice();

	void ChangeRoute(uint32_t wavelength, Ptr<CoreDevice> cd, uint32_t wavelength_dst);
	void ReceiveBurstStart(uint32_t wavelength);
	void ReceiveBurstEnd(uint32_t wavelength, Ptr<PacketBurst> pb);
	void SetFiber(Ptr<OBSFiber> fiber);
	bool SendBurst(uint32_t wavelength, Ptr<PacketBurst> pkt);

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



};

#endif
