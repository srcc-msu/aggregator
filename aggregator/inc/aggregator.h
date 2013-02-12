#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <unordered_map>
#include <string>
#include <algorithm>
#include <thread>

using namespace std;

#include <sys/socket.h>
#include <sys/types.h>

#include "nm_control.h"

#include "queue.h"
#include "sensor_metainf.h"
#include "filter.h"
#include "packet.h"
#include "connection.h"
#include "buffer_aggregator.h"
#include "queue_aggregator.h"

class CAggregator;

void BackgroundStatHelper(CAggregator* agg, int sleep_time);

void BackgroundAgentHelper(CAggregator* agg, int sleep_time);

/**
	Listens the socket for all incoming packets and agregates them
	in the set of circualr buffers and one queue
	also provides access to them and manages filtering options
*/
class CAggregator
{
private:
	CConnectionManager connection;

	CBufferAggregator buffer_aggregator;
	CQueueAggregator queue_aggregator;

public:
	long stat_allow_time;
	long stat_allow_delta;
	long stat_allow_abs_delta;
	long stat_filtered_out;
	long stat_filtered_blacklist;
	long stat_added;


/**
	print stat about flitration, discards old values
*/
	void Stat();

/**
	print stat about active agents, reinit notresponding agents
*/
	void AgentsStat();

/**
	start a thread, which will print \Stat() every \sleep_time
*/
	void BackgroundAgentsStat(int sleep_time);

/**
	start a thread, which will print \AgentStat() every \sleep_time
*/
	void BackgroundStat(int sleep_time);


/**
	>.> <.<
*/
	CConnectionManager& Connection()
		{ return connection; }

	CBufferAggregator& BufferAggregator()
		{ return buffer_aggregator; }

	CQueueAggregator& QueueAggregator()
		{ return queue_aggregator; }

/**
	accumulate statistics about one packet, basing on
	\result of its filtration
*/
	void AccumulateStat(int result);

/**
	some terrible function, that form \packets_buffer from
	incoming \sens_data
	extracting it from \Process did not go well...
	TODO rework
*/
	void ProcessSensor(unsigned int sensor_id, nm_data_hdr_t* header,
		unsigned char* sens_data, size_t val_size, size_t count,
		const timeval& current_time, SPacket* packets_buffer
		, int& packets_count);

/**
	Recieve one message from the socket, convert it as needed
	and stores it into queues.
*/
	void Process();

	CAggregator(int port, uint32_t address):
	connection(port, address)
	{
		InitMetainf();
	}

	~CAggregator()
	{}
};

#endif
