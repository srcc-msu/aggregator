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


	std::unordered_map<uint32_t, std::unordered_map<uint16_t, SensorFilterMetainf>> last_occurance;

//
//	Filtration checks the filter in following order (below).
//	If requested \address and/or \sensor_id found - does not check next.

//	address + sensor_id filter
	std::unordered_map<uint32_t, std::unordered_map<uint16_t, SensorFilterMetainf>> filters;

//	address filter
	std::unordered_map<uint32_t, SensorFilterMetainf> address_filters;

//	sensor_id filter
	std::unordered_map<uint16_t, SensorFilterMetainf> sensor_id_filters;

	SensorFilterMetainf default_filter;

	AccessList<uint16_t> id_blacklist;

	bool FilterOut(const SPacketExt& ext_packet);

public:
	void BlacklistId(uint16_t sensor_id);
	void UnblacklistId(uint16_t sensor_id);
	
	void SetDelta(uint32_t address, uint16_t sensor_id, double delta);
	void SetDelta(uint32_t address, double delta);
	void SetDelta(uint16_t sensor_id, double delta);
	void SetDelta(double delta);

	void SetInterval(uint32_t address, uint16_t sensor_id, int max_interval);
	void SetInterval(uint32_t address, int max_interval);
	void SetInterval(uint16_t sensor_id, int max_interval);
	void SetInterval(int max_interval);

	void Add(const SPacketExt& ext_packet);

	SPacket* GetAllData(size_t* count) 
		{ return queue.GetAll(count); }

	CQueueAggregator()
	{}
};

#endif
