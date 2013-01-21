#include <iostream>
#include <sstream>

using std :: cout;
using std :: endl;
using std :: stringstream;

#include "aggregator_api.h"
#include "connection.h"

int main(int argc, char** argv)
{
	int agg_id = InitAggregator("127.0.0.1");

	BufferAllowId(agg_id, 2160);
	BufferAllowId(agg_id, 1051);
	BufferAllowId(agg_id, 1052);

	SetDeltaS(agg_id, 1053, 0.02);
	SetIntervalS(agg_id, 1053, 10);

	InitAgent(agg_id, "127.0.0.1");

	BackgroundProcess(agg_id);
	sleep(30);

	size_t count = 0;
	SPacket* l = GetAllData(agg_id, &count);

	cout << "Got " << count << endl;

	cout << "First " << l[0].address << endl;

	if(l != nullptr)
		for(size_t i = 0; i < count; i++)
			cout << l[i].sensor_id << " " << l[i].address << " " << l[i].data_string << endl;

	cout << endl << "buffer" << endl;

	l = GetInterval(agg_id, "127.0.0.1", 1051, 1, 10, 0, &count);

	if(l != nullptr)
		for(size_t i = 0; i < count; i++)
			cout << i << " " << l[i].sensor_id << " " << l[i].agent_timestamp << " " << l[i].address << endl;
	else
		cout << "no data in buffer" << endl;

	return 0;
}

