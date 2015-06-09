#ifndef AGGREGATOR_H
#define AGGREGATOR_H

#include <unordered_map>
#include <string>
#include <algorithm>
#include <thread>
#include <vector>

using namespace std;

#include <sys/socket.h>
#include <sys/types.h>

#include "nm_control.h"

#include "sensor_queue.h"
#include "sensor_metainf.h"
#include "filter.h"
#include "packet.h"
#include "connection.h"
#include "queue_aggregator.h"
#include "avg_aggregator.h"

nm_data_hdr_t* MapHeaderNtoH(unsigned char* buffer);

/**
	stores pointers and service info about packets
	can be consturcted from raw packet data
*/
struct SDecodedPacket
{
	size_t bytes_read;

	nm_data_hdr_t* header;
	size_t sens_offset;

	unsigned char* sens_data;
	size_t data_size;

	timeval current_time;

	SDecodedPacket(unsigned char* _buffer, size_t _bytes_read):
		bytes_read(_bytes_read)
	{
		header = MapHeaderNtoH(_buffer);
		sens_offset = sizeof(nm_data_hdr_t);
		sens_data = _buffer + sens_offset;
		data_size = bytes_read - sizeof(nm_data_hdr_t);

		gettimeofday(&current_time, NULL);
	}

	SDecodedPacket() = delete;
};

/**
	Listens the socket for all incoming packets and agregates them
	in the set of circualr buffers and one queue
	also provides access to them and manages filtering options
*/
class CAggregator
{
private:
	CConnectionManager connection;

	CQueueAggregator queue_aggregator;
	CAvgAggregator avg_aggregator;

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
	void Stat(int sleep_time);

/**
	print stat about active agents, reinit notresponding agents
*/
	void AgentsStat(int sleep_time);

/**
	start a thread, which will print \Stat() every \sleep_time
*/
	void BackgroundAgentsStat(int sleep_time);
	void BackgroundAgentHelper(int sleep_time);

/**
	start a thread, which will print \AgentStat() every \sleep_time
*/
	void BackgroundStat(int sleep_time);
	void BackgroundStatHelper(int sleep_time);

/**
	start a thread, which will run processing in background
*/
	void BackgroundProcessHelper();
	void BackgroundProcess();

/**
	>.> <.<
*/
	CConnectionManager& Connection()
		{ return connection; }

	CQueueAggregator& QueueAggregator()
		{ return queue_aggregator; }

/**
	accumulate statistics about one packet, basing on
	\result of its filtration
*/
	void AccumulateStat(int result);

/**
	process sensor chunk and add all sensors to \packets_buffer
*/
	void ProcessChunk(const SDecodedPacket& packet
		, uint16_t sensor_id, size_t count, size_t val_size
		, unsigned char* sens_data
		, vector<SPacket>& packets_buffer);

/**
	Recieve one message from the socket, convert it as needed
	and stores it into queues.
*/
	void Process();

	void ProcessPacket(const SDecodedPacket& packet);

	std::vector<NodeEntry> GetAvgData()
	{
		return avg_aggregator.GetAvgData();
	}

	CAggregator(int port, uint32_t address):
		connection(port, address),
		avg_aggregator(300)
	{
		InitMetainf();
	}

	~CAggregator()
	{}
};

#endif
