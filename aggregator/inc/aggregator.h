#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <sys/socket.h>
#include <sys/types.h>

#include <unordered_map>

#include <iostream>
#include <string>

using std :: string;
using std :: cout;
using std :: endl;

#include "queue.h"
#include "sensor_metainf.h"
#include "filter.h"
#include "packet.h"

/**
	Implements so called 'tristasecundnij buffer'
	stores all packets and set of sicular buffers
	implements, used std::unordered_map  TODO check performance
*/
class CBufferAggregator
{
private:
	std::unordered_map<uint32_t, std::unordered_map<uint16_t, CCircularBuffer<SPacket>* >> buffers;

public:

/// Add single packet to the buffer
	void Add(uint32_t address, uint16_t sensor_id, SPacket p);

/// Get the \seconds interval data for one sesnor on one node from the buffer
	SPacket* GetInterval(uint32_t address, uint16_t sensor_id, int seconds)
		{ assert(0); };
};

/**
	Listens the socket for all incoming packets and agregates them
	in the set of circualr buffers and one queue
	also provides access to them and manages filtering options(TODO).
*/
class CAggregator
{
private:
	sockaddr_in address;
	int socket;

	CSensorQueue<SPacket> queue;
	CBufferAggregator buffer_aggregator;

	Blacklister<uint32_t> address_blacklist;
	Blacklister<uint16_t> id_blacklist;

	CSensorFilter queue_filter;

/**
	Creates a socket, where data will be read from
*/
	void Init(int port);

public:
	void BlacklistAddress(uint32_t address)
		{ address_blacklist.Add(address); }

	void BlacklistId(uint16_t id)
		{ id_blacklist.Add(id); }

	SPacket* GetInterval(uint32_t address, uint16_t id, size_t seconds) 
		{ return buffer_aggregator.GetInterval(address, id, seconds); }

	SPacket* GetAllData(size_t* count) 
		{ return queue.GetAll(count); }

/**
	Recieve one message from the socket, converts it as needed
	and stores it into queues.
*/
	void Process();

	CAggregator(int port):
	socket(-1)
	{
		Init(port);
		InitMetainf();
	}

	~CAggregator()
	{
		close(socket);
	}
};

#endif
