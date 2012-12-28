#include "aggregator_api.h"

#include <iostream>
#include <vector>
#include <thread>

using std :: cout;
using std :: endl;

#include "queue.h"
#include "aggregator.h"

static const int AGENT_CONTROL_PORT = 4259;

void __aggregator()
{
	cout << "peace and love, bro!" << endl;
}

std :: vector<CAggregator*> aggregators;

int InitAggregator(const char* address)
{
	int n = aggregators.size();

	aggregators.push_back(new CAggregator(AGENT_CONTROL_PORT, inet_addr(address)));

	return n;
}

void InitAgent(int agg_id, const char* address)
{
	aggregators[agg_id]->Connection().InitAgent(inet_addr(address));
}

void Process(int agg_id)
{
	aggregators[agg_id]->Process();
}

void BackgroundProcessHelper(int agg_id)
{
    while(1)
    	aggregators[agg_id]->Process();
}

void BackgroundProcess(int agg_id)
{
	static bool started = false;

	if(started == true)
		return;

	started = true;

    std::thread t1(BackgroundProcessHelper, agg_id);

    t1.detach();
}

void GlobalBlacklistAddress(int agg_id, const char* address)
{
	aggregators[agg_id]->Connection().BlacklistAddress(inet_addr(address));
}


void QueueBlacklistId(int agg_id, uint16_t id)
{
	aggregators[agg_id]->QueueAggregator().BlacklistId(id);
}

void QueueUnblacklistId(int agg_id, uint16_t id)
{
	aggregators[agg_id]->QueueAggregator().UnblacklistId(id);
}

void BufferAllowId(int agg_id, uint16_t id)
{
	aggregators[agg_id]->BufferAggregator().AllowId(id);
}

void BufferDisallowId(int agg_id, uint16_t id)
{
	aggregators[agg_id]->BufferAggregator().DisallowId(id);
}

// --------------------------

void SetDeltaAS(int agg_id, const char* address, uint16_t sensor_id, double delta)
{
	aggregators[agg_id]->QueueAggregator().SetDelta(inet_addr(address), sensor_id, delta);
}

void SetDeltaS(int agg_id, uint16_t sensor_id, double delta)
{
	aggregators[agg_id]->QueueAggregator().SetDelta(sensor_id, delta);
}

void SetDeltaA(int agg_id, const char* address, double delta)
{
	aggregators[agg_id]->QueueAggregator().SetDelta(inet_addr(address), delta);
}

void SetDelta(int agg_id, double delta)
{
	aggregators[agg_id]->QueueAggregator().SetDelta(delta);
}

// --------------------------

void SetIntervalAS(int agg_id, const char* address, uint16_t sensor_id, int max_interval)
{
	aggregators[agg_id]->QueueAggregator().SetInterval(inet_addr(address), sensor_id, max_interval);
}

void SetIntervalA(int agg_id, const char* address, int max_interval)
{
	aggregators[agg_id]->QueueAggregator().SetInterval(inet_addr(address), max_interval);
}

void SetIntervalS(int agg_id, uint16_t sensor_id, int max_interval)
{
	aggregators[agg_id]->QueueAggregator().SetInterval(sensor_id, max_interval);
}

void SetInterval(int agg_id, int max_interval)
{
	aggregators[agg_id]->QueueAggregator().SetInterval(max_interval);
}

// --------------------------

SPacket* GetInterval(int agg_id, const char* address, uint16_t id, size_t from, size_t upto)
{
	return aggregators[agg_id]->BufferAggregator().GetInterval(inet_addr(address), id, from, upto);
}

SPacket* GetAllData(int agg_id, size_t* count)
{
	return aggregators[agg_id]->QueueAggregator().GetAllData(count);
}