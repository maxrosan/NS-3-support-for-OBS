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
	Mac48Address   m_address;
	uint32_t       m_ifindex;
	bool           m_link_up;
	uint32_t       m_mtu;
	Callback<void> m_link_change_cb;
	Ptr<OBSFiber>  m_fiber;
	Ptr<Node>      m_node;
public:

	virtual void StartReceiving (Ptr<Packet> p, Ptr<OBSBaseDevice> sender, uint32_t wavelength)=0;
	virtual void StopReceiving (Ptr<OBSBaseDevice> sender, uint32_t wavelength)=0;
	//
	virtual void SetIfIndex (const uint32_t index);
	virtual uint32_t GetIfIndex (void) const;
	virtual Ptr<Channel> GetChannel (void) const;
	virtual void SetAddress (Address address);
	virtual Address	GetAddress (void) const;
	virtual bool SetMtu (const uint16_t mtu);
	virtual uint16_t GetMtu (void) const;
	virtual bool IsLinkUp (void) const;
	virtual void AddLinkChangeCallback (Callback<void> callback);
	virtual bool IsBroadcast (void) const;
	virtual Address	GetBroadcast (void) const;
	virtual bool IsMulticast (void) const;
	virtual Address	GetMulticast (Ipv4Address multicastGroup) const;
	//Make and return a MAC multicast address using the provided multicast group.
	virtual Address	GetMulticast (Ipv6Address addr) const;
	//Get the MAC multicast address corresponding to the IPv6 address provided.
	virtual bool IsBridge (void) const;
	//Return true if the net device is acting as a bridge.
	virtual bool IsPointToPoint (void) const;
	//Return true if the net device is on a point-to-point link.
	virtual bool Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber);
	virtual bool SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber);
	virtual Ptr<Node> GetNode (void) const;
	virtual void SetNode (Ptr< Node > node);
	virtual bool NeedsArp (void) const;
	virtual void SetReceiveCallback (ReceiveCallback cb);
	virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
	virtual bool SupportsSendFrom (void) const;
};

class OBSCoreDevice: public OBSBaseDevice {
public:
	OBSCoreDevice();
	virtual void StartReceiving (Ptr<Packet> p, Ptr<OBSBaseDevice> sender, uint32_t wavelength);
	virtual void StopReceiving (Ptr<OBSBaseDevice> sender, uint32_t wavelength);
	// Send and SendFrom send packets through the wavelength 0, which is used to
	// send control messages only
	virtual bool Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber);
	virtual bool SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber);

};

};
