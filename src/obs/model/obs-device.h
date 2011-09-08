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

namespace ns3 {

class OBSFiber;

class OBSBaseDevice : public NetDevice {
protected:
	Mac48Address m_address;
	uint32_t     m_ifindex;
public:
	virtual void SetIfIndex (const uint32_t index);
	virtual uint32_t GetIfIndex (void) const;
	virtual Ptr<Channel> GetChannel (void) const =0;
	virtual void SetAddress (Address address)=0;
	virtual Address	GetAddress (void) const =0;
	virtual bool SetMtu (const uint16_t mtu)=0;
	virtual uint16_t GetMtu (void) const =0;
	virtual bool IsLinkUp (void) const =0;
	virtual void AddLinkChangeCallback (Callback<void> callback)=0;
	virtual bool IsBroadcast (void) const =0;
	virtual Address	GetBroadcast (void) const =0;
	virtual bool IsMulticast (void) const =0;
	virtual Address	GetMulticast (Ipv4Address multicastGroup) const =0;
	//Make and return a MAC multicast address using the provided multicast group.
	virtual Address	GetMulticast (Ipv6Address addr) const =0;
	//Get the MAC multicast address corresponding to the IPv6 address provided.
	virtual bool IsBridge (void) const =0;
	//Return true if the net device is acting as a bridge.
	virtual bool IsPointToPoint (void) const =0;
	//Return true if the net device is on a point-to-point link.
	virtual bool Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber)=0;
	virtual bool SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber)=0;
	virtual Ptr<Node> GetNode (void) const =0;
	virtual void SetNode (Ptr< Node > node)=0;
	virtual bool NeedsArp (void) const =0;
	virtual void SetReceiveCallback (ReceiveCallback cb)=0;
	virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb)=0;
	virtual bool SupportsSendFrom (void) const =0;
};

};
