#ifndef BUFFER_AGGREGATOR_H
#define BUFFER_AGGREGATOR_H

#include <sys/types.h>

#include <unordered_map>

#include "queue.h"
#include "filter.h"
#include "packet.h"

/**
	Implements so called 'trehsotsecundnij buffer'
	stores all packets in set of circular buffers.
	Implemented using std::unordered_map. TODO check performance
*/
class CBufferAggregator
{
private:
	std::unordered_map<uint16_t, std::unordered_map<uint32_t, CCircularBuffer<SPacket>* >> buffers;
	AccessList<uint16_t> allowed_id;

public:

/**
	Allows packets with such \sensor_id to be stored in circualr buffers.
	If buffer did not exist before - creates the buffer.
*/
	void AllowId(uint16_t sensor_id);

/**
	Disallows packets with such \sensor_id to be stored in circualr buffers.
	If buffer exists - delete the buffer.
*/
	void DisallowId(uint16_t sensor_id);

//	Add single packet to the buffer
	void Add(const SPacketExt& ext_packet);

//	Get the \seconds interval data for one sesnor on one node from the buffer
	SPacket* GetInterval(uint32_t address, uint16_t sensor_id, int seconds);

	CBufferAggregator()
	{}

	~CBufferAggregator()
	{

		// TODO
	/*	std::for_each(buffers.begin(), buffers.end(), [](auto addr){
			std::for_each(buffers.begin(), buffers.end(), [](auto val){
				delete val;
			});
		})*/

/*		for(auto addr : buffers)
			for(auto buffer : addr)
				delete buffer;*/
	}
};

#endif