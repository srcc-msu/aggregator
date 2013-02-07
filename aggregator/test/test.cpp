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

	try{BackgroundProcess(agg_id);}
	catch(...)
	{}
	InitAgent(agg_id, "10.0.80.19");

	sleep(10);

	size_t count = 0;
	GetAllData(agg_id, &count);

	return 0;
}

