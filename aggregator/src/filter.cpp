#include "filter.h"

#include <iostream>

using namespace std;

bool CSensorFilter :: FilterOut(SPacket& packet, unsigned char* raw_buffer)
{
	SPacketExt ext_packet(packet, raw_buffer);
	packet = ext_packet.packet;
	
	SensorFilterMetainf& filter = filters[packet.address][packet.sensor_id];

	bool filter_out = true;


	uint32_t diff = packet.agent_timestamp - filter.last.packet.agent_timestamp;
	double delta = fabs(GetDiv(ext_packet.value, filter.last.value, 
		ext_packet.info.type, filter.last.info.msg_length) - 1.0);
	

	if(diff > filter.max_interval || filter.last.packet.agent_timestamp == 0)
	{
		cout << "diff " << diff << " let it pass!" << endl;

		filter_out = false;
	}
	else if(delta > filter.delta)
	{
		cout << "delta " << delta << " let it pass!" << endl;

		filter_out = false;
	}

	if(!filter_out)
		filter.last = ext_packet;

	return filter_out;
}