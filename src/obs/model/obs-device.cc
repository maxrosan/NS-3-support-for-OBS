
#include "obs-device.h"
#include "obs-channel.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(OBSBaseDevice);

class OBSFiber;

void
OBSBaseDevice::SetIfIndex(uint32_t index) {
	m_ifindex = index;
}

Ptr<Channel>
OBSBaseDevice::GetChannel(void) const {
	return m_fiber;
}

uint32_t
OBSBaseDevice::GetIfIndex() const {
	return m_ifindex;
}

void
OBSBaseDevice::SetAddress(Address address) {
	m_address = Mac48Address::ConvertFrom(address);
}

Address
OBSBaseDevice::GetAddress(void) const {
	return m_address;
}

bool
OBSBaseDevice::SetMtu(const uint16_t mtu) {
	m_mtu = mtu;
	
	return true; // XXX: no checking?
}

uint16_t
OBSBaseDevice::GetMtu(void) const {
	return m_mtu;
}

bool
OBSBaseDevice::IsLinkUp(void) const {
	return m_link_up;
}

void
OBSBaseDevice::AddLinkChangeCallback(Callback<void> cb) {
	m_link_change_cb = cb;
}

bool
OBSBaseDevice::IsBroadcast(void) const {
	return true;
}

Address
OBSBaseDevice::GetBroadcast(void) const {
	return Mac48Address("ff:ff:ff:ff:ff:ff");
}

bool
OBSBaseDevice::IsMulticast(void) const {
	return true;
}

Address
OBSBaseDevice::GetMulticast(Ipv4Address multicastGroup) const {
	return Mac48Address::GetMulticast(multicastGroup);
}

Address
OBSBaseDevice::GetMulticast(Ipv6Address multicastGroup) const {
	return Mac48Address::GetMulticast(multicastGroup);
}

bool
OBSBaseDevice::IsBridge (void) const {
	return false;
}

bool
OBSBaseDevice::IsPointToPoint (void) const {
	return true;
}

bool
OBSBaseDevice::Send (Ptr< Packet > packet, const Address &dest, uint16_t protocolNumber) {
	return false;
}

bool
OBSBaseDevice::SendFrom (Ptr< Packet > packet, const Address &source, const Address &dest, uint16_t protocolNumber) {

	NS_ASSERT(packet != NULL);

	return false;
}

Ptr<Node>
OBSBaseDevice::GetNode (void) const {
	return m_node;
}

void
OBSBaseDevice::SetNode (Ptr< Node > node) {
	NS_ASSERT(node != NULL);

	m_node = node;
}

bool
OBSBaseDevice::NeedsArp (void) const {
	return true;
}

void
OBSBaseDevice::SetReceiveCallback (ReceiveCallback cb) {
}


void
OBSBaseDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb) {
}

bool
OBSBaseDevice::SupportsSendFrom (void) const {
	return false;
}

OBSCoreDevice::OBSCoreDevice() {
	m_link_up = false;
}

void
OBSCoreDevice::StartReceiving (Ptr<Packet> p, Ptr<OBSBaseDevice> sender, uint32_t wavelength) {

}

void
OBSCoreDevice::StopReceiving (Ptr<OBSBaseDevice> sender, uint32_t wavelength) {

}

};
