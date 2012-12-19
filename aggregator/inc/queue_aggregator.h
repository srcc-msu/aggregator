#ifndef QUEUE_AGGREGATOR_H
#define QUEUE_AGGREGATOR_H

#include <sys/types.h>

#include <unordered_map>

#include "queue.h"
#include "sensor_metainf.h"
#include "filter.h"
#include "packet.h"

const double DEFAULT_DELTA = 0.01;
const double DEFAULT_MAX_INTERVAL = 30;

/**
	stores metainformation about each sensor on each address
*/
struct SensorFilterMetainf
{
	double delta;

/*NIY*/
	//int abs_delta; // should be used if \delta == 0

	uint32_t max_interval; // interval in seconds, when new packet will be sent regardless anyting

	SPacketExt last;

	SensorFilterMetainf(double delta = DEFAULT_DELTA, int max_interval = DEFAULT_MAX_INTERVAL):
	delta(delta),
	max_interval(max_interval)
	{}
};


/**
	provides extended filtering options for sensors, basing on values,
	ids and addresses
*/
class CQueueAggregator
{
private:
	CSensorQueue<SPacket> queue;
	
	std::unordered_map<uint32_t, std::unordered_map<uint16_t, SensorFilterMetainf>> filters;

	AccessList<uint16_t> id_blacklist;

	bool FilterOut(const SPacketExt& ext_packet);

public:
	void BlacklistId(uint16_t sensor_id)
		{ id_blacklist.Add(sensor_id); }

	void UnblacklistId(uint16_t sensor_id)
		{ id_blacklist.Remove(sensor_id); }


//	TODO works wrong, filter change required
	void SetDelta(uint32_t address, uint16_t sensor_id, double delta);
	void SetDelta(uint16_t sensor_id, double delta); // TODO
	void SetDelta(double delta); // TODO

	void SetInterval(uint32_t address, uint16_t sensor_id, int max_interval);
	void SetInterval(uint16_t sensor_id, int max_interval); // TODO
	void SetInterval(int max_interval); // TODO

	void Add(const SPacketExt& ext_packet);

	SPacket* GetAllData(size_t* count) 
		{ return queue.GetAll(count); }

	CQueueAggregator()
	{}
};

#endif
