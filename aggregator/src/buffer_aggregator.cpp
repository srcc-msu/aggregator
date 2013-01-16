#include "buffer_aggregator.h"
#include "debug.h"

void CBufferAggregator :: AllowId(uint16_t sensor_id)
{
	DMSG1("sensor " << sensor_id << " will be collected in buffer");
	allowed_id.Add(sensor_id);
}

void CBufferAggregator :: DisallowId(uint16_t sensor_id)
{
	DMSG1("sensor " << sensor_id << " will not be collected in buffer");
	allowed_id.Remove(sensor_id);
}

void CBufferAggregator :: Add(const SPacketExt& ext_packet)
{
	const SPacket& packet = ext_packet.packet;

	uint32_t buff_value = packet.sensor_id << 16 | packet.sensor_num;

	if(!allowed_id.IsIn(packet.sensor_id)) 
		return;

	if(buffers[packet.address].count(buff_value) == 0)
	{
		CCircularBuffer<SPacket>* buf = new CCircularBuffer<SPacket>;
		buffers[packet.address][buff_value] = buf;
	}

	buffers[packet.address][buff_value]->Add(packet);

    DMSG2(packet.sensor_id << "." << (int)packet.sensor_num << " aka " << buff_value 
    	<< " added to circ. buffer, size is now " << buffers.size() << " and " << buffers[packet.address].size());

}

SPacket* CBufferAggregator :: GetInterval(uint32_t address, uint16_t sensor_id, uint16_t sensor_num, size_t from, size_t upto, size_t* count)
{
	if(!allowed_id.IsIn(sensor_id)) 
	{
		DMSG1("requested sensor" << sensor_id << " is not collected to circular buffer ");
		return nullptr;
	}

	uint32_t buff_value = sensor_id << 16 | sensor_num;

	if(buffers[address].count(buff_value) != 0)
		return buffers[address][buff_value]->Get(from, upto, count);

	DMSG1("circular buffer is empty for sensor " << sensor_id << "." << sensor_num << " aka " << buff_value);

	return nullptr;
};

