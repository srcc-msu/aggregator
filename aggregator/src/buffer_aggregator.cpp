#include "buffer_aggregator.h"

#include "debug.h"
#include "error.h"

void CBufferAggregator :: AllowId(uint16_t sensor_id)
{
DMSG1("sensor " << sensor_id << " will be collected in buffer");
	allowed_id.Add(sensor_id);
}

//--------------------------------

void CBufferAggregator :: DisallowId(uint16_t sensor_id)
{
DMSG1("sensor " << sensor_id << " will not be collected in buffer");
	allowed_id.Remove(sensor_id);
}

//--------------------------------

void CBufferAggregator :: Add(const SPacket& packet)
{
	uint32_t buff_value = SENS_UID(packet.sensor_id, packet.sensor_num);

	if(!allowed_id.IsIn(packet.sensor_id))
		return;

// if it is a first message - create new buffer
	if(buffers[packet.address.b4[0]].count(buff_value) == 0)
	{
		CCircularBuffer<SPacket>* buf = new CCircularBuffer<SPacket>;
		buffers[packet.address.b4[0]][buff_value] = buf;
	}

	buffers[packet.address.b4[0]][buff_value]->Add(packet);

DMSG2(packet.sensor_id << "." << (int)packet.sensor_num
    	<< " aka " << buff_value
    	<< " added to circ. buffer, size is now " << buffers.size()
    	<< " and " << buffers[packet.address.b4[0]].size());
}

//--------------------------------

SPacket* CBufferAggregator :: GetInterval(uint32_t address
	, uint16_t sensor_id, uint16_t sensor_num, size_t from
	, size_t upto, size_t* count)
{
	throw CException("GetInterval is NIY");

	if(!allowed_id.IsIn(sensor_id))
	{
DMSG1("requested sensor" << sensor_id
			<< " is not collected to circular buffer ");
		return nullptr;
	}

	uint32_t buff_value = SENS_UID(sensor_id, sensor_num);

	if(buffers[address].count(buff_value) != 0)
		return buffers[address][buff_value]->Get(from, upto, count);

DMSG1("circular buffer is empty for sensor " << sensor_id
		<< "." << sensor_num << " aka " << buff_value);

	return nullptr;
};
