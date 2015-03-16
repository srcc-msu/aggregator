#ifndef AGGREGATOR_API_H
#define AGGREGATOR_API_H

#include "aggregator.h"

/**
	Starts the loop, procesing all incoming packet in separate thread.
	Pretends to be thread safe. (orly? TODO check)
*/
void BackgroundProcess(CAggregator& aggregator);

#endif
