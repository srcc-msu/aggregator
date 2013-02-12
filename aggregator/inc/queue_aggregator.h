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

	typedef std::unordered_map<uint32_t, SensorFilterMetainf> sens_info_map;
	typedef std::unordered_map<uint32_t, sens_info_map> addr_sens_info_map;

	addr_sens_info_map last_occurance;

	typedef std::unordered_map<uint16_t, SensorFilterMetainf> sens_info_param;
	typedef std::unordered_map<uint32_t, SensorFilterMetainf> addr_info_param;
	typedef std::unordered_map<uint32_t, sens_info_param> addr_sens_info_param;

/**
	Filtration checks the filter in following order (below).
	If requested \address and/or \sensor_id found - does not check next.
*/

//	address + sensor_id filter
	addr_sens_info_param filters;

//	address filter
	addr_info_param address_filters;

//	sensor_id filter
	sens_info_param sensor_id_filters;

//	default fiter, if noone of above match
	SensorFilterMetainf default_filter;

	AccessList<uint16_t> id_blacklist;
	AccessList<uint16_t> id_average;
	AccessList<uint16_t> id_speed;


// TODO: there must be 2*const
	int Filter(SPacketExt& ext_packet);

/**
	slow fnction, that checks and add single sensor packet using mutex
	more efficient to use \Filter + \UncheckedAdd (many packets)
*/
	void Add(SPacketExt& ext_packet);

/**
	Writes speed value to the packet, if it is in \id_speed access list
	speed is calculated by the current value and value from \last_occurance.
*/
	void AddSpeed(SPacketExt& ext_packet, double diff, int interval) const;

public:
/**
	allow calculating average across \sensor_id packets with
	different \sesnor_num
*/
	void RegisterAverageId(uint16_t sensor_id);
	bool IsAverageId(uint16_t sensor_id) const
	{
		return id_average.IsIn(sensor_id);
	}

/**
	Allow speed calculation for all \sensor_id packets
	if \sensor_id is no registered - speed will be 0
*/
	void RegisterSpeedId(uint16_t sensor_id);
	bool IsSpeedId(uint16_t sensor_id) const
	{
		return id_speed.IsIn(sensor_id);
	}

/**
	writes speed to the \ext_packet, if it is allowed
	TODO: change
*/
	void AddSpeed(SPacketExt& ext_packet) const;

/**
	Blacklisted packets will not be added anywhere, like they never existed
*/
	void BlacklistId(uint16_t sensor_id);
	void UnblacklistId(uint16_t sensor_id);

	bool IsAllowed(uint16_t sensor_id) const
	{
		return !id_blacklist.IsIn(sensor_id);
	}

/**
	Set the change threshold \delta, when new sensor value will be
	added to the queue.
	Proirity of filters comes as follow:
*/
	void SetDelta(uint32_t address, uint16_t sensor_id, double delta);
	void SetDelta(uint32_t address, double delta);
	void SetDelta(uint16_t sensor_id, double delta);
	void SetDelta(double delta);

/**
	Set the absolute threshold \abs_delta, when new sensor value will be
	added to the queue.
	Proirity of filters comes as follow:
*/
	void SetAbsDelta(uint32_t address, uint16_t sensor_id, double delta);
	void SetAbsDelta(uint32_t address, double delta);
	void SetAbsDelta(uint16_t sensor_id, double delta);
	void SetAbsDelta(double delta);

/**
	Set the time threshold \max_interval, when new sensor value will be
	added to the queue.
	Proirity of filters comes as follow:
*/
	void SetInterval(uint32_t address, uint16_t sensor_id, int max_interval);
	void SetInterval(uint32_t address, int max_interval);
	void SetInterval(uint16_t sensor_id, int max_interval);
	void SetInterval(int max_interval);


/**
	add multiple packets to the queue, without checking
	uses only one mutex block
*/
	void UncheckedAdd(SPacket* packets, size_t count);

/**
	get average \UValue for all \sensor_id, from \address
	count = max \sensor_num
*/
	UValue CalcAverage(uint32_t address, uint16_t sensor_id, int count);


/**
	checks if the packet is allowed to pass in queue
	0 - not; positives - yes
	1 - \max_interval exceed, 2 - \delta exceed, 3 - \abs_delta exceed
*/
	int Check(SPacketExt& ext_packet);

	SPacket* GetAllData(size_t* count)
		{ return queue.GetAll(count); }

	CQueueAggregator()
	{}
};

#endif
