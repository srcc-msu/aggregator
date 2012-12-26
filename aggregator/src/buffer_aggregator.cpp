#include "buffer_aggregator.h"
#include "debug.h"

void CBufferAggregator :: AllowId(uint16_t sensor_id)
{
	allowed_id.Add(sensor_id);
}

void CBufferAggregator :: DisallowId(uint16_t sensor_id)
{
	allowed_id.Remove(sensor_id);
	
	// TODO add memory cleaning
}

void CBufferAggregator :: Add(const SPacketExt& ext_packet)
{
	const SPacket& packet = ext_packet.packet;

	if(!allowed_id.IsIn(packet.sensor_id)) 
		return;

	if(buffers[packet.address].count(packet.sensor_id) == 0)
	{
		CCircularBuffer<SPacket>* buf = new CCircularBuffer<SPacket>;
		buffers[packet.address][packet.sensor_id] = buf;
	}

	buffers[packet.address][packet.sensor_id]->Add(packet);

	DMSG2("added to circ. buffer, size is now " << buffers.size() << " and " << buffers[packet.address].size());
}

SPacket* CBufferAggregator :: GetInterval(uint32_t address, uint16_t sensor_id, int seconds)
{
	if(!allowed_id.IsIn(sensor_id)) 
		return nullptr;

	if(buffers[address].count(sensor_id) != 0)
		return buffers[address][sensor_id]->Get(seconds);

	return nullptr;
};

