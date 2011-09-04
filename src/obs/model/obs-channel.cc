
#include "obs-channel.h"

#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("OBSChannel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(OBSChannel);

OBSSubChannel::OBSSubChannel() {

}

void
OBSSubChannel::setOBSChannel(OBSChannel *obs) {
	m_obs = obs;
}

TypeId
OBSChannel::GetTypeId(void) {

	static TypeId tid = TypeId("ns3::OBSChannel")
		.SetParent<Channel>()
		.AddConstructor<OBSChannel>()
		.AddAttribute("DataRate",
			"Transmission rate",
			DataRateValue(DataRate("10Gbps")),
			MakeDataRateAccessor(&OBSChannel::m_bps),
			MakeDataRateChecker())
		.AddAttribute("Delay",
			"Transmission delay",
			TimeValue(Seconds(0)),
			MakeTimeAccessor(&OBSChannel::m_delay),
			MakeTimeChecker());

	return tid;
}

OBSChannel::OBSChannel(): 
	Channel() 
{
	NS_LOG_FUNCTION_NOARGS();
	
}

};
