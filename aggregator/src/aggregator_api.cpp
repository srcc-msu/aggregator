#include "aggregator_api.h"

#include <iostream>
#include <vector>
#include <thread>
#include <memory>

#include "aggregator.h"

static void BackgroundProcessHelper(std::shared_ptr<CAggregator> aggregator)
{
    while(1)
    	aggregator->Process();
}

void BackgroundProcess(std::shared_ptr<CAggregator> aggregator)
{
	static bool started = false;

	if(started == true)
		return;

	started = true;

    std::thread t1(BackgroundProcessHelper, aggregator);

    t1.detach();

    aggregator->BackgroundAgentsStat(10);
    aggregator->BackgroundStat(10);
}
