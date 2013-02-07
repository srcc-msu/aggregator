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
int InitAggregator(const char* address);
	
/**
	Configures the agent on \addres to send data to this aggregator
*/
void InitAgent(int agg_id, const char* address);

/**
	Process one incoming packet by aggregator.
	Pretends to be thread safe. (orly? TODO check)
*/
void Process(int agg_id);

/**
	Starts the loop, procesing all incoming packet in separate thread.
	Pretends to be thread safe. (orly? TODO check)
*/
void BackgroundProcess(int agg_id);

/**
	Add the \address to the global blacklist,
	IPv4 packed in uint32 currently.
	TODO: provide aliases for arrays/strings, if needed.
*/
void GlobalBlacklistAddress(int agg_id, const char* address);

/**
	Add/remove the sensor \id to the queue blacklist.
	By default all sensors are allowed.
*/
void QueueBlacklistId(int agg_id, uint16_t id);
void QueueUnblacklistId(int agg_id, uint16_t id);

/**
	average values will be recalculated for \sensor_id
	when any of single values change
	TODO check, may be very time consuming
*/
void QueueRegisterAverageId(int agg_id, uint16_t sensor_id);

/**
	speed values will be calculated for \sensor_id
*/
void QueueRegisterSpeedId(int agg_id, uint16_t sensor_id);

/**
	Allos/disallow the sensor \id to be collected in 300s buffer.
	By default all sensors are disallowed.
*/
void BufferAllowId(int agg_id, uint16_t id);
void BufferDisallowId(int agg_id, uint16_t id);

/**
	Set the change threshold \delta, when new sensor value will be added to the queue.
	Proirity of filters comes as follow:
*/
void SetDeltaAS(int agg_id, const char* address, uint16_t sensor_id, double delta);
void SetDeltaS(int agg_id, uint16_t sensor_id, double delta);
void SetDeltaA(int agg_id, const char* address, double delta);
void SetDelta(int agg_id, double delta);

/**
	Set the absolute threshold \abs_delta, when new sensor value will be added to the queue.
	Proirity of filters comes as follow:
*/
void SetAbsDeltaAS(int agg_id, const char* address, uint16_t sensor_id, double delta);
void SetAbsDeltaS(int agg_id, uint16_t sensor_id, double delta);
void SetAbsDeltaA(int agg_id, const char* address, double delta);
void SetAbsDelta(int agg_id, double delta);

/**
	Set the time threshold \max_interval, when new sensor value will be added to the queue.
	Proirity of filters comes as follow:
*/
void SetIntervalAS(int agg_id, const char* address, uint16_t sensor_id, int max_interval);
void SetIntervalA(int agg_id, const char* address, int max_interval);
void SetIntervalS(int agg_id, uint16_t sensor_id, int max_interval);
void SetInterval(int agg_id, int max_interval);

/**
	Returns \seconds interval of data from circular buffer, corresponding to
	\address and \id.
*/
SPacket* GetInterval(int agg_id, const char* address, uint16_t id, uint16_t num, size_t from, size_t upto, size_t* count);

/**
	Returns pointer to collected data. And they will not be availiable throught
	this function, anymore.
	(!) When you call this 2nd time, data from previous call are deleted.
	(!) Also they will be deleted, when the librarys is unloaded.
	Returns array of \SPacket and writes the length into the \count.
*/
SPacket* GetAllData(int agg_id, size_t* count);
}

#endif
