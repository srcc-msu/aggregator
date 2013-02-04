#include <iostream>
#include <sstream>

using std :: cout;
using std :: endl;

#include "aggregator_api.h"
#include "connection.h"

int main(int argc, char** argv)
{
	int agg_id = InitAggregator("10.0.248.2");

	BufferAllowId(agg_id, 2160);
	BufferAllowId(agg_id, 1051);
	BufferAllowId(agg_id, 1052);

	SetDeltaS(agg_id, 1053, 0.02);
	SetIntervalS(agg_id, 1053, 10);

	BackgroundProcess(agg_id);
	InitAgent(agg_id, "10.0.80.19");

	sleep(10);

	size_t count = 0;
	SPacket* l = GetAllData(agg_id, &count);

	printf("Got " << count );

	printf("First " << (l[0].address.b4[0]) );

	if(l != nullptr)
		for(size_t i = 0; i < count; i++)
			printf((l[i].sensor_id), " " << (l[i].address.b4[0]) << " ", (l[i].data_string) );

	printf(endl << "buffer" );

	l = GetInterval(agg_id, "127.0.0.1", 1051, 1, 10, 0, &count);

	if(l != nullptr)
		for(size_t i = 0; i < count; i++)
			printf(i, " " << l[i].sensor_id << " " << l[i].agent_timestamp << " ", l[i].address.b4[0] );
	else
		printf("no data in buffer" );

	return 0;
}

