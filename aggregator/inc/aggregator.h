#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <sys/socket.h>
#include <sys/types.h>

#include <unordered_map>

#include <iostream>
#include <string>
#include <algorithm>

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
