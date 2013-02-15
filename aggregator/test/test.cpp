#include <iostream>
#include <sstream>

using namespace std;

#include "aggregator_api.h"
#include "connection.h"

int main(int argc, char** argv)
{
	int agg_id = InitAggregator("10.0.248.2");

	InitAgent(agg_id, "10.0.80.0");
	InitAgent(agg_id, "10.0.80.1");
	InitAgent(agg_id, "10.0.80.2");
	InitAgent(agg_id, "10.0.80.3");
	InitAgent(agg_id, "10.0.80.4");
	InitAgent(agg_id, "10.0.80.5");
	InitAgent(agg_id, "10.0.80.6");
	InitAgent(agg_id, "10.0.80.7");
	InitAgent(agg_id, "10.0.80.8");
	InitAgent(agg_id, "10.0.80.9");
	InitAgent(agg_id, "10.0.81.0");
	InitAgent(agg_id, "10.0.81.1");
	InitAgent(agg_id, "10.0.81.2");
	InitAgent(agg_id, "10.0.81.3");
	InitAgent(agg_id, "10.0.81.4");
	InitAgent(agg_id, "10.0.81.5");
	InitAgent(agg_id, "10.0.81.6");
	InitAgent(agg_id, "10.0.81.7");
	InitAgent(agg_id, "10.0.81.8");
	InitAgent(agg_id, "10.0.81.9");

	InitAgent(agg_id, "10.0.82.0");
	InitAgent(agg_id, "10.0.82.1");
	InitAgent(agg_id, "10.0.82.2");
	InitAgent(agg_id, "10.0.82.3");
	InitAgent(agg_id, "10.0.82.4");
	InitAgent(agg_id, "10.0.82.5");
	InitAgent(agg_id, "10.0.82.6");
	InitAgent(agg_id, "10.0.82.7");
	InitAgent(agg_id, "10.0.82.8");
	InitAgent(agg_id, "10.0.82.9");
	InitAgent(agg_id, "10.0.83.0");
	InitAgent(agg_id, "10.0.83.1");
	InitAgent(agg_id, "10.0.83.2");
	InitAgent(agg_id, "10.0.83.3");
	InitAgent(agg_id, "10.0.83.4");
	InitAgent(agg_id, "10.0.83.5");
	InitAgent(agg_id, "10.0.83.6");
	InitAgent(agg_id, "10.0.83.7");
	InitAgent(agg_id, "10.0.83.8");
	InitAgent(agg_id, "10.0.83.9");

	InitAgent(agg_id, "10.0.84.0");
	InitAgent(agg_id, "10.0.84.1");
	InitAgent(agg_id, "10.0.84.2");
	InitAgent(agg_id, "10.0.84.3");
	InitAgent(agg_id, "10.0.84.4");
	InitAgent(agg_id, "10.0.84.5");
	InitAgent(agg_id, "10.0.84.6");
	InitAgent(agg_id, "10.0.84.7");
	InitAgent(agg_id, "10.0.84.8");
	InitAgent(agg_id, "10.0.84.9");
	InitAgent(agg_id, "10.0.85.0");
	InitAgent(agg_id, "10.0.85.1");
	InitAgent(agg_id, "10.0.85.2");
	InitAgent(agg_id, "10.0.85.3");
	InitAgent(agg_id, "10.0.85.4");
	InitAgent(agg_id, "10.0.85.5");
	InitAgent(agg_id, "10.0.85.6");
	InitAgent(agg_id, "10.0.85.7");
	InitAgent(agg_id, "10.0.85.8");
	InitAgent(agg_id, "10.0.85.9");

	InitAgent(agg_id, "10.0.70.0");
	InitAgent(agg_id, "10.0.70.1");
	InitAgent(agg_id, "10.0.70.2");
	InitAgent(agg_id, "10.0.70.3");
	InitAgent(agg_id, "10.0.70.4");
	InitAgent(agg_id, "10.0.70.5");
	InitAgent(agg_id, "10.0.70.6");
	InitAgent(agg_id, "10.0.70.7");
	InitAgent(agg_id, "10.0.70.8");
	InitAgent(agg_id, "10.0.70.9");
	InitAgent(agg_id, "10.0.71.0");
	InitAgent(agg_id, "10.0.71.1");
	InitAgent(agg_id, "10.0.71.2");
	InitAgent(agg_id, "10.0.71.3");
	InitAgent(agg_id, "10.0.71.4");
	InitAgent(agg_id, "10.0.71.5");
	InitAgent(agg_id, "10.0.71.6");
	InitAgent(agg_id, "10.0.71.7");
	InitAgent(agg_id, "10.0.71.8");
	InitAgent(agg_id, "10.0.71.9");

	InitAgent(agg_id, "10.0.72.0");
	InitAgent(agg_id, "10.0.72.1");
	InitAgent(agg_id, "10.0.72.2");
	InitAgent(agg_id, "10.0.72.3");
	InitAgent(agg_id, "10.0.72.4");
	InitAgent(agg_id, "10.0.72.5");
	InitAgent(agg_id, "10.0.72.6");
	InitAgent(agg_id, "10.0.72.7");
	InitAgent(agg_id, "10.0.72.8");
	InitAgent(agg_id, "10.0.72.9");
	InitAgent(agg_id, "10.0.73.0");
	InitAgent(agg_id, "10.0.73.1");
	InitAgent(agg_id, "10.0.73.2");
	InitAgent(agg_id, "10.0.73.3");
	InitAgent(agg_id, "10.0.73.4");
	InitAgent(agg_id, "10.0.73.5");
	InitAgent(agg_id, "10.0.73.6");
	InitAgent(agg_id, "10.0.73.7");
	InitAgent(agg_id, "10.0.73.8");
	InitAgent(agg_id, "10.0.73.9");

	InitAgent(agg_id, "10.0.74.0");
	InitAgent(agg_id, "10.0.74.1");
	InitAgent(agg_id, "10.0.74.2");
	InitAgent(agg_id, "10.0.74.3");
	InitAgent(agg_id, "10.0.74.4");
	InitAgent(agg_id, "10.0.74.5");
	InitAgent(agg_id, "10.0.74.6");
	InitAgent(agg_id, "10.0.74.7");
	InitAgent(agg_id, "10.0.74.8");
	InitAgent(agg_id, "10.0.74.9");
	InitAgent(agg_id, "10.0.75.0");
	InitAgent(agg_id, "10.0.75.1");
	InitAgent(agg_id, "10.0.75.2");
	InitAgent(agg_id, "10.0.75.3");
	InitAgent(agg_id, "10.0.75.4");
	InitAgent(agg_id, "10.0.75.5");
	InitAgent(agg_id, "10.0.75.6");
	InitAgent(agg_id, "10.0.75.7");
	InitAgent(agg_id, "10.0.75.8");
	InitAgent(agg_id, "10.0.75.9");

	InitAgent(agg_id, "10.0.90.0");
	InitAgent(agg_id, "10.0.90.1");
	InitAgent(agg_id, "10.0.90.2");
	InitAgent(agg_id, "10.0.90.3");
	InitAgent(agg_id, "10.0.90.4");
	InitAgent(agg_id, "10.0.90.5");
	InitAgent(agg_id, "10.0.90.6");
	InitAgent(agg_id, "10.0.90.7");
	InitAgent(agg_id, "10.0.90.8");
	InitAgent(agg_id, "10.0.90.9");
	InitAgent(agg_id, "10.0.91.0");
	InitAgent(agg_id, "10.0.91.1");
	InitAgent(agg_id, "10.0.91.2");
	InitAgent(agg_id, "10.0.91.3");
	InitAgent(agg_id, "10.0.91.4");
	InitAgent(agg_id, "10.0.91.5");
	InitAgent(agg_id, "10.0.91.6");
	InitAgent(agg_id, "10.0.91.7");
	InitAgent(agg_id, "10.0.91.8");
	InitAgent(agg_id, "10.0.91.9");

	InitAgent(agg_id, "10.0.92.0");
	InitAgent(agg_id, "10.0.92.1");
	InitAgent(agg_id, "10.0.92.2");
	InitAgent(agg_id, "10.0.92.3");
	InitAgent(agg_id, "10.0.92.4");
	InitAgent(agg_id, "10.0.92.5");
	InitAgent(agg_id, "10.0.92.6");
	InitAgent(agg_id, "10.0.92.7");
	InitAgent(agg_id, "10.0.92.8");
	InitAgent(agg_id, "10.0.92.9");
	InitAgent(agg_id, "10.0.93.0");
	InitAgent(agg_id, "10.0.93.1");
	InitAgent(agg_id, "10.0.93.2");
	InitAgent(agg_id, "10.0.93.3");
	InitAgent(agg_id, "10.0.93.4");
	InitAgent(agg_id, "10.0.93.5");
	InitAgent(agg_id, "10.0.93.6");
	InitAgent(agg_id, "10.0.93.7");
	InitAgent(agg_id, "10.0.93.8");
	InitAgent(agg_id, "10.0.93.9");

	InitAgent(agg_id, "10.0.94.0");
	InitAgent(agg_id, "10.0.94.1");
	InitAgent(agg_id, "10.0.94.2");
	InitAgent(agg_id, "10.0.94.3");
	InitAgent(agg_id, "10.0.94.4");
	InitAgent(agg_id, "10.0.94.5");
	InitAgent(agg_id, "10.0.94.6");
	InitAgent(agg_id, "10.0.94.7");
	InitAgent(agg_id, "10.0.94.8");
	InitAgent(agg_id, "10.0.94.9");
	InitAgent(agg_id, "10.0.95.0");
	InitAgent(agg_id, "10.0.95.1");
	InitAgent(agg_id, "10.0.95.2");
	InitAgent(agg_id, "10.0.95.3");
	InitAgent(agg_id, "10.0.95.4");
	InitAgent(agg_id, "10.0.95.5");
	InitAgent(agg_id, "10.0.95.6");
	InitAgent(agg_id, "10.0.95.7");
	InitAgent(agg_id, "10.0.95.8");
	InitAgent(agg_id, "10.0.95.9");

	BufferAllowId(agg_id, 2160);
	BufferAllowId(agg_id, 1051);
	BufferAllowId(agg_id, 1052);

	SetDeltaS(agg_id, 1053, 0.02);
	SetIntervalS(agg_id, 1053, 10);

	for(int i = 0; i < 10000; i++)
	{
		if(i % 100 == 0)
			printf("done %d\n", i);

		Process(agg_id);
	}

	size_t count = 0;
	
	SPacket *p = GetAllData(agg_id, &count);

	for(int i = 0; i < min(100, (int)count); i++)
	{
		printf("%d %d\n", (int)p[i].address.b4[0], p[i].sensor_id);
	}

	return 0;
}

