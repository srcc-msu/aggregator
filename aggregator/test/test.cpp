#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>

using namespace std;

#include "packet.h"
#include "aggregator.h"

static const int AGENT_CONTROL_PORT = 4259;

int main(int argc, char** argv)
{
	std::shared_ptr<CAggregator> aggregator = std::make_shared<CAggregator>(AGENT_CONTROL_PORT, inet_addr("10.0.248.2"));

	aggregator->Connection().InitAgent(inet_addr("10.0.80.0"));
	aggregator->Connection().InitAgent(inet_addr("10.0.80.1"));
	aggregator->Connection().InitAgent(inet_addr("10.0.80.2"));
	aggregator->Connection().InitAgent(inet_addr("10.0.80.3"));
	aggregator->Connection().InitAgent(inet_addr("10.0.80.4"));
	aggregator->Connection().InitAgent(inet_addr("10.0.80.5"));
	aggregator->Connection().InitAgent(inet_addr("10.0.80.6"));
	aggregator->Connection().InitAgent(inet_addr("10.0.80.7"));
	aggregator->Connection().InitAgent(inet_addr("10.0.80.8"));
	aggregator->Connection().InitAgent(inet_addr("10.0.80.9"));

	aggregator->QueueAggregator().SetDeltaS((uint16_t)1053, 0.02);
	aggregator->QueueAggregator().SetIntervalS((uint16_t)1053, 10);

	for(int i = 0; i < 10; i++)
	{
		aggregator->Process();
	}

	std::vector<SPacket> data = aggregator->QueueAggregator().GetData();

	for(auto &packet : data)
	{
		printf("%d %d\n", (int)packet.address.b4[0], packet.sensor_id);
	}

	return 0;
}

