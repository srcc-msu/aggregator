#ifndef QUEUE_AGGREGATOR_H
#define QUEUE_AGGREGATOR_H

#include <sys/types.h>

#include <unordered_map>

#include "queue.h"
#include "sensor_metainf.h"
#include "filter.h"
#include "packet.h"

static const double DEFAULT_DELTA = 0.01;
static const double DEFAULT_ABS_DELTA = 100000.0;
static const int DEFAULT_MAX_INTERVAL = 30;

/**
	stores metainformation about each sensor on each address
*/
struct SensorFilterMetainf
{
/**	
	max relative value change, bypassing that new packet will be sent
	if less than 0 - not used
*/
	double delta;

/**	
	max absolute value change, bypassing that new packet will be sent
	if less than 0 - not used
*/
	double abs_delta;

/**	
	interval in seconds, when new packet will be sent regardless of anyting
	if less than 0 - not used
*/
	int max_interval; 

	SPacketExt last; // last packet, stored to compare

	SensorFilterMetainf(double _delta = DEFAULT_DELTA, double _abs_delta = DEFAULT_ABS_DELTA,
		int _max_interval = DEFAULT_MAX_INTERVAL):
	delta(_delta),
	abs_delta(_abs_delta),
	max_interval(_max_interval)
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

	std::unordered_map<uint32_t, std::unordered_map<uint32_t, SensorFilterMetainf>> last_occurance;

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
	AccessList<uint16_t> id_average;
	AccessList<uint16_t> id_speed;

	int Filter(const SPacketExt& ext_packet);

	void Add(const SPacketExt& ext_packet);

public:
	void RegisterAverageId(uint16_t sensor_id);
	bool IsAverageId(uint16_t sensor_id);

	void RegisterSpeedId(uint16_t sensor_id);
	bool IsSpeedId(uint16_t sensor_id);

	void BlacklistId(uint16_t sensor_id);
	void UnblacklistId(uint16_t sensor_id);
	
	void SetDelta(uint32_t address, uint16_t sensor_id, double delta);
	void SetDelta(uint32_t address, double delta);
	void SetDelta(uint16_t sensor_id, double delta);
	void SetDelta(double delta);

	void SetAbsDelta(uint32_t address, uint16_t sensor_id, double delta);
	void SetAbsDelta(uint32_t address, double delta);
	void SetAbsDelta(uint16_t sensor_id, double delta);
	void SetAbsDelta(double delta);

	void SetInterval(uint32_t address, uint16_t sensor_id, int max_interval);
	void SetInterval(uint32_t address, int max_interval);
	void SetInterval(uint16_t sensor_id, int max_interval);
	void SetInterval(int max_interval);

	void UncheckedAdd(SPacket* packets, size_t count);

	UValue CalcAverage(uint32_t address, uint16_t sensor_id, int count);

	int Check(const SPacketExt& ext_packet);

	SPacket* GetAllData(size_t* count) 
		{ return queue.GetAll(count); }

	CQueueAggregator()
	{}
};

#endif
