#ifndef AGGREGATOR_API_H
#define AGGREGATOR_API_H

#include <sys/socket.h>
#include <sys/types.h>

#include "packet.h"

extern "C" {

/**
	Inits a new agregator, returns the aggregator handle
	it is used in all functions to specify the aggregator
*/
size_t InitAggregator(const char* address);

/**
	Configures the agent on \addres to send data to this aggregator
*/
void InitAgent(size_t agg_id, const char* address);

/**
	Process one incoming packet by aggregator.
	Pretends to be thread safe. (orly? TODO check)
*/
void Process(size_t agg_id);

/**
	Starts the loop, procesing all incoming packet in separate thread.
	Pretends to be thread safe. (orly? TODO check)
*/
void BackgroundProcess(size_t agg_id);

/**
	Add the \address to the global blacklist,
	IPv4 packed in uint32 currently.
	TODO: provide aliases for arrays/strings, if needed.
*/
void GlobalBlacklistAddress(size_t agg_id, const char* address);

/**
	Add/remove the sensor \id to the queue blacklist.
	By default all sensors are allowed.
*/
void QueueBlacklistId(size_t agg_id, uint16_t id);
void QueueUnblacklistId(size_t agg_id, uint16_t id);

/**
	average values will be recalculated for \sensor_id
	when any of single values change
	TODO check, may be very time consuming
*/
void QueueRegisterAverageId(size_t agg_id, uint16_t sensor_id);

/**
	speed values will be calculated for \sensor_id
*/
void QueueRegisterSpeedId(size_t agg_id, uint16_t sensor_id);

/**
	Allos/disallow the sensor \id to be collected in 300s buffer.
	By default all sensors are disallowed.
*/
void BufferAllowId(size_t agg_id, uint16_t id);
void BufferDisallowId(size_t agg_id, uint16_t id);

/**
	Set the change threshold \delta, when new sensor value will be
	added to the queue.
	Proirity of filters comes as follow:
*/
void SetDeltaAS(size_t agg_id, const char* address, uint16_t sensor_id
	, double delta);
void SetDeltaS(size_t agg_id, uint16_t sensor_id, double delta);
void SetDeltaA(size_t agg_id, const char* address, double delta);
void SetDelta(size_t agg_id, double delta);

/**
	Set the absolute threshold \abs_delta, when new sensor value will be
	added to the queue.
	Proirity of filters comes as follow:
*/
void SetAbsDeltaAS(size_t agg_id, const char* address, uint16_t sensor_id
	, double delta);
void SetAbsDeltaS(size_t agg_id, uint16_t sensor_id, double delta);
void SetAbsDeltaA(size_t agg_id, const char* address, double delta);
void SetAbsDelta(size_t agg_id, double delta);

/**
	Set the time threshold \max_interval, when new sensor value will be
	added to the queue.
	Proirity of filters comes as follow:
*/
void SetIntervalAS(size_t agg_id, const char* address, uint16_t sensor_id
	, int max_interval);
void SetIntervalA(size_t agg_id, const char* address, int max_interval);
void SetIntervalS(size_t agg_id, uint16_t sensor_id, int max_interval);
void SetInterval(size_t agg_id, int max_interval);

/**
	Returns \seconds interval of data from circular buffer, corresponding
	to \address and \id.
*/
SPacket* GetInterval(size_t agg_id, const char* address, uint16_t id
	, uint16_t num, size_t from, size_t upto, size_t* count);

/**
	Returns pointer to collected data, they will not be availiable
	throught this function, anymore.
	Returns array of \SPacket and writes the length into the \count.
*/
SPacket* GetAllData(size_t agg_id, size_t* count);
}

#endif
