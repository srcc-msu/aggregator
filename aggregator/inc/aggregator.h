#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <sys/socket.h>
#include <sys/types.h>

#include <unordered_map>

#include <iostream>
#include <string>
#include <algorithm>
#include <thread>

using std :: string;
using std :: cout;
using std :: endl;

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
	long stat_allow_val;
	long stat_filtered_out;
	long stat_filtered_blacklist;
	long stat_added;

	void Stat()
	{
		cout << endl << " ---- Aggregator stat ---- " << endl;

		cout << "blacklisted  : " << stat_filtered_blacklist << endl;
		cout << "filtered out : " << stat_filtered_out << endl;
		cout << "added        : " << stat_added << " = time : " <<
			stat_allow_time << " + val : " << stat_allow_val << endl;

		cout << " ------------------------- " << endl << endl;

		stat_filtered_blacklist = 0;
		stat_filtered_out = 0;
		stat_allow_time = 0;
		stat_allow_val = 0;
		stat_added = 0;
	}

	void AgentsStat()
	{
		cout << endl << " ---- Agents stat ---- " << endl;

		connection.AgentStats();
		connection.PokeAgents();

		cout << " --------------------- " << endl << endl;
	}

	void BackgroundAgentsStat(int sleep_time)
	{
	    std::thread t(BackgroundAgentHelper, this, sleep_time);
	    t.detach();
	}

	void BackgroundStat(int sleep_time)
	{
	    std::thread t(BackgroundStatHelper, this, sleep_time);
	    t.detach();
	}

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
