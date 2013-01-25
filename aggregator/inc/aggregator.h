#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <sys/socket.h>
#include <sys/types.h>

#include <unordered_map>

#include <iostream>
#include <string>
#include <algorithm>
#include <thread>

using std::string;
using std::cout;
using std::endl;

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
	also provides access to them and manages filtering options(TODO).
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

	void Stat();

	void AgentsStat();

	void BackgroundAgentsStat(int sleep_time);

	void BackgroundStat(int sleep_time);

	CConnectionManager& Connection()
		{ return connection; }

	CBufferAggregator& BufferAggregator()
		{ return buffer_aggregator; }

	CQueueAggregator& QueueAggregator()
		{ return queue_aggregator; }

/**
	Recieve one message from the socket, converts it as needed
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
