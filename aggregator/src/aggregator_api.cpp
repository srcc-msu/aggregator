#include "aggregator_api.h"

#include <iostream>
#include <vector>

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

void BlacklistAddress(int agg_id, const char* address)
{
	aggregators[agg_id]->Connection().BlacklistAddress(inet_addr(address));
}

void BlacklistId(int agg_id, uint16_t id)
{
	aggregators[agg_id]->BlacklistId(id);
}

SPacket* GetInterval(int agg_id, const char* address, uint16_t id, size_t seconds)
{
	return aggregators[agg_id]->GetInterval(inet_addr(address), id, seconds);
}

SPacket* GetAllData(int agg_id, size_t* count)
{
	return aggregators[agg_id]->GetAllData(count);
}