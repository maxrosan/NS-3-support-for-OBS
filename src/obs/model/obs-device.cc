
#include "obs-device.h"
#include "obs-channel.h"

namespace ns3 {

class OBSFiber;

void
OBSBaseDevice::SetIfIndex(uint32_t index) {
	m_ifindex = index;
}

uint32_t
OBSBaseDevice::GetIfIndex() const {
	return m_ifindex;
}

};
