#include "aggregator_api.h"

#include <iostream>

using std :: cout;
using std :: endl;

#include "queue.h"
#include "aggregator.h"

int test(int port)
{
	CCircularBuffer<int> buffer(10);

	for(int i = 0; i < 13; i++)
		buffer.Add(i);

	int* t = buffer.Get(5);

	for(int i = 0; i < 5; i++)
		cout << t[i] << " ";

	cout << endl;

// -------------------------------------------------------

	CSensorQueue<int> squeue;

	for(int i = 0; i < 13; i++)
		squeue.Add(i);

	size_t count = 0;
	int* t2 = squeue.GetAll(&count);

	for(int i = 0; i < 5; i++)
		cout << t2[i] << " ";

	cout << endl;
	squeue.RemoveOld();

// -------------------------------------------------------

	CAggregator agg(port);

	agg.Process();

	SPacket* packets = agg.GetAllData(&count);

	for(int i = 0; i < 10; i++)
		cout << packets[i].address << " " << packets[i].data_string << endl;

	return 0;
}

CAggregator* aggregator = nullptr;

void InitAggregator(int port)
{
	if(aggregator != nullptr)
		delete aggregator;

	aggregator = new CAggregator(port);
}

void Process()
{
	aggregator->Process();
}

void BlacklistAddress(uint32_t address)
{
	aggregator->BlacklistAddress(address);
}

void BlacklistId(uint16_t id)
{
	aggregator->BlacklistId(id);
}

SPacket* GetInterval(uint32_t address, uint16_t id, size_t seconds)
{
	return aggregator->GetInterval(address, id, seconds);
}

SPacket* GetAllData(size_t* count)
{
	return aggregator->GetAllData(count);
}