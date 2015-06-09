#include "queue_aggregator.h"

#include "debug.h"

//--------------------------------

void CQueueAggregator :: RegisterAverageId(uint16_t sensor_id)
{
DMSG1("added to the queue average list: " << sensor_id);

	id_average.Add(sensor_id);
}

//--------------------------------

void CQueueAggregator :: RegisterSpeedId(uint16_t sensor_id)
{
DMSG1("added to the queue speed list: " << sensor_id);

	id_speed.Add(sensor_id);
}

//--------------------------------

UValue CQueueAggregator :: CalcAverage(uint32_t address
	, uint16_t sensor_id, int count)
{
	UValue sum;

// all that packet must have the same type and we will use last in the end
	SPacket packet;

	for(int i = 0; i < count; i++)
	{
		uint32_t buff_value = SENS_UID(sensor_id, i + 1);

		packet = last_occurance[address][buff_value].last;
		sum = GetSum(sum, packet.value, packet.type);
	}

	return MultValue(sum, packet.type, 1.0 / count);
}

//--------------------------------

void CQueueAggregator :: AddSpeed(SPacket& packet, double diff
	, int interval) const
{
	if(!IsSpeedId(packet.sensor_id))
		return;

	packet.speed = (interval != 0) ? diff / interval : 0;
}

void CQueueAggregator :: AddSpeed(SPacket& packet) const
{
	if(!IsSpeedId(packet.sensor_id))
	{
		packet.speed = 0.0;
		return;
	}

	auto it_add = last_occurance.find(packet.address.b4[0]);

	if(it_add == last_occurance.end())
		return;

	uint32_t buff_value = SENS_UID(packet.sensor_id, packet.sensor_num);

	auto it_sens = it_add->second.find(buff_value);

	if(it_sens == it_add->second.end())
		return;

	const SensorFilterMetainf& last_filter = it_sens->second;

// time diff
	int interval = packet.agent_timestamp
		- last_filter.last.agent_timestamp;

	double diff = GetDiff(packet.value, last_filter.last.value
		, packet.type);

	packet.speed = diff / interval;
}

//--------------------------------

void CQueueAggregator :: BlacklistId(uint16_t sensor_id)
{
DMSG1("added to the queue blacklist: " << sensor_id);

	id_blacklist.Add(sensor_id);
}

void CQueueAggregator :: UnblacklistId(uint16_t sensor_id)
{
DMSG1("removed from the queue blacklist: " << sensor_id);

	id_blacklist.Remove(sensor_id);
}

//--------------------------------

int CQueueAggregator :: Filter(SPacket& packet)
{
	uint32_t buff_value = SENS_UID(packet.sensor_id, packet.sensor_num);

	int allow_res = 0;

// get appropriate filter
	SensorFilterMetainf filter = default_filter;

	SensorFilterMetainf& last_filter =
		last_occurance[packet.address.b4[0]][buff_value];

 	if(filters[packet.address.b4[0]].find(packet.sensor_id)
 		!= filters[packet.address.b4[0]].end())
 	{
DMSG2(packet.address.b4[0] << " and " << packet.sensor_id
 			<< " present in filter")
 		filter = filters[packet.address.b4[0]][packet.sensor_id];
 	}

	else if(address_filters.find(packet.address.b4[0])
		!= address_filters.end())
	{
DMSG2(packet.address.b4[0] << " presents in filter")
 		filter = address_filters[packet.address.b4[0]];
	}

	else if(sensor_id_filters.find(packet.sensor_id)
		!= sensor_id_filters.end())
 	{
DMSG2(packet.sensor_id << " presents in filter")
 		filter = sensor_id_filters[packet.sensor_id];
	}

// time diff
	int interval = packet.agent_timestamp
		- last_filter.last.agent_timestamp;

// relative delta
	double delta = fabs(GetDiv(packet.value, last_filter.last.value,
		packet.type) - 1.0);

	double diff = GetDiff(packet.value, last_filter.last.value,
		packet.type);

// abs delta
	double abs_delta = fabs(diff);

	if(filter.max_interval > 0 && (interval >= filter.max_interval
		|| last_filter.last.agent_timestamp == 0))
		allow_res = 1;

	else if(filter.delta > 0 && delta > filter.delta)
		allow_res = 2;

	else if(filter.abs_delta > 0 && abs_delta > filter.abs_delta)
		allow_res = 3;

DMSG2("interval " << interval << "      max_int " << filter.max_interval
	<< "      delta " << delta << "      filter.delta " << filter.delta
	<< "      abs_delta " << abs_delta << "      filter.abs_delta "
	<< filter.abs_delta
	<< (allow_res ? "      let it pass!" : "         drop it!")
	<< " " << (unsigned long long)packet.value.b8[0]
	<< " " << (unsigned long long)last_filter.last.value.b8[0])

	if(allow_res)
	{
		AddSpeed(packet, diff, interval);
		last_filter.last = packet;
	}

	return allow_res;
}

int CQueueAggregator :: Check(SPacket& packet)
{
	return Filter(packet);
}

void CQueueAggregator :: UncheckedAdd(std::vector<SPacket>& packets_buffer)
{
	queue.Add(packets_buffer);
}

//--------------------------------

void CQueueAggregator :: SetDelta(uint32_t address, uint16_t sensor_id
	, double delta)
{
	filters[address][sensor_id].delta = delta;
}

void CQueueAggregator :: SetDeltaS(uint16_t sensor_id, double delta)
{
	sensor_id_filters[sensor_id].delta = delta;
}

void CQueueAggregator :: SetDeltaA(uint32_t address, double delta)
{
	address_filters[address].delta = delta;
}

void CQueueAggregator :: SetDelta(double delta)
{
	default_filter.delta = delta;
}

//--------------------------------

void CQueueAggregator :: SetAbsDelta(uint32_t address
	, uint16_t sensor_id, double abs_delta)
{
	filters[address][sensor_id].abs_delta = abs_delta;
}

void CQueueAggregator :: SetAbsDeltaS(uint16_t sensor_id, double abs_delta)
{
	sensor_id_filters[sensor_id].abs_delta = abs_delta;
}

void CQueueAggregator :: SetAbsDeltaA(uint32_t address, double abs_delta)
{
	address_filters[address].abs_delta = abs_delta;
}

void CQueueAggregator :: SetAbsDelta(double abs_delta)
{
	default_filter.abs_delta = abs_delta;
}

//--------------------------------

void CQueueAggregator :: SetInterval(uint32_t address
	, uint16_t sensor_id, int max_interval)
{
	filters[address][sensor_id].max_interval = max_interval;
}

void CQueueAggregator :: SetIntervalS(uint16_t sensor_id, int max_interval)
{
	sensor_id_filters[sensor_id].max_interval = max_interval;
}

void CQueueAggregator :: SetIntervalA(uint32_t address, int max_interval)
{
	address_filters[address].max_interval = max_interval;
}

void CQueueAggregator :: SetInterval(int max_interval)
{
	default_filter.max_interval = max_interval;
}
