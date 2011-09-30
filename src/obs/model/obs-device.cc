
#include "obs-device.h"
#include "obs-channel.h"

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

CoreDevice::CoreDevice() {
}

void
CoreDevice::MuxMap(uint32_t wavelength, Ptr<CoreDevice> cd, uint32_t wavelength_dst) {

	m_mux[wavelength] = std::make_pair(cd, wavelength);

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

};
