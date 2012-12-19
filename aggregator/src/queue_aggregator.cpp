#include "queue_aggregator.h"

#include "debug.h"

bool CQueueAggregator :: FilterOut(const SPacketExt& ext_packet)
{
	const SPacket& packet = ext_packet.packet; // short
	
	SensorFilterMetainf& filter = filters[packet.address][packet.sensor_id];

	bool filter_out = true;

	uint32_t diff = packet.agent_timestamp - filter.last.packet.agent_timestamp;

	double delta = fabs(GetDiv(ext_packet.value, filter.last.value, 
		ext_packet.info.type, filter.last.info.msg_length) - 1.0);
	
	if(diff > filter.max_interval || filter.last.packet.agent_timestamp == 0)
	{
		DMSG2("diff " << diff << " let it pass!");

		filter_out = false;
	}
	else if(delta > filter.delta)
	{
		DMSG2("delta " << delta << " let it pass!");

		filter_out = false;
	}

	if(!filter_out)
		filter.last = ext_packet;

	return filter_out;
}

void CQueueAggregator :: Add(const SPacketExt& ext_packet)
{
	if(!id_blacklist.IsIn(ext_packet.packet.sensor_id) && 
		!FilterOut(ext_packet))
	{
		queue.Add(ext_packet.packet);
		DMSG2(ext_packet.packet.sensor_id << " packet added");
	}
	DBLOCK2(
		else DMSG2(ext_packet.packet.sensor_id << " is in blacklist");
	)
}

void CQueueAggregator :: SetDelta(uint32_t address, uint16_t sensor_id, double delta)
{ 
	filters[address][sensor_id].delta = delta; 
}

void CQueueAggregator :: SetDelta(uint16_t sensor_id, double delta)
{ 
	//for(auto addr : filters)
	//	addr[sensor_id].delta = delta; 
}

void CQueueAggregator :: SetDelta(double delta)
{ 
	//for(auto addr : filters)
	//	for(auto filter : addr)
	//		filter[sensor_id].delta = delta; 
}

void CQueueAggregator :: SetInterval(uint32_t address, uint16_t sensor_id, int max_interval)
{ 
	filters[address][sensor_id].max_interval = max_interval; 
}

void CQueueAggregator :: SetInterval(uint16_t sensor_id, int max_interval)
{ 
	//for(auto addr : filters)
	//	addr[sensor_id].max_interval = max_interval; 
}

void CQueueAggregator :: SetInterval(int max_interval)
{ 
	//for(auto addr : filters)
	//	for(auto filter : addr)
	//		filter[sensor_id].max_interval = max_interval; 
}
