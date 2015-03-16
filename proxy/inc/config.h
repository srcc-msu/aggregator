#ifndef CONFIG_H
#define CONFIG_H

#include "config.h"

#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
//#include <chrono>
#include <unistd.h>
#include <unordered_map>

#include <json/json.h>

#include "error.h"
#include "aggregator.h"

void BackgroundInitHelper(std::shared_ptr<CAggregator> aggregator, Json::Value agents)
{
	for(size_t i = 0; i < agents.size(); i++)
	{
		const char* agent = agents[i][1].asCString();

		aggregator->Connection().InitAgent(inet_addr(agent));
		printf("agent %s added\n", agent);

//      this_thread::sleep_for(std::chrono::milliseconds(10)); // not working in gcc 4.6
		usleep(10000);
	}
}

//--------------------------------

void BackgroundInit(std::shared_ptr<CAggregator> aggregator, Json::Value agents)
{
	static bool started = false;

	if(started == true)
		return;

	started = true;

	std::thread thread(BackgroundInitHelper, aggregator, agents);

	thread.detach();
}

//--------------------------------

static const int AGENT_CONTROL_PORT = 4259;

std::shared_ptr<CAggregator> ConfigAggregator(const string& config_fname
	, std::unordered_map<uint16_t, string>& id_to_name)
{
	std::ifstream tmp(config_fname);
	std::stringstream buffer;
	buffer << tmp.rdbuf();

	Json::Value root;
	Json::Reader reader;

	if(!reader.parse(buffer.str(), root))
	{
		fprintf(stderr, "failed to parse configuration: %s\n"
			, reader.getFormatedErrorMessages ().c_str());

		throw CException("json parser failed");
	}


//  aggregator address
	const char* address = root["server"]["address"].asCString();
	printf("Aggregator on %s\n", address);

	std::shared_ptr<CAggregator> aggregator = std::make_shared<CAggregator>(AGENT_CONTROL_PORT, inet_addr(address));

//  default delta
	double delta = root["server"]["queue"]["delta"].asDouble();
	aggregator->QueueAggregator().SetDelta(delta);
	printf("delta set to %lg\n", delta);

//  default abs_delta
	double abs_delta = root["server"]["queue"]["abs_delta"].asDouble();
	aggregator->QueueAggregator().SetAbsDelta(abs_delta);
	printf("abs_delta set to %lg\n", abs_delta);

//  default interval
	int interval = root["server"]["queue"]["interval"].asInt();
	aggregator->QueueAggregator().SetInterval(interval);
	printf("time interval set to %d\n", interval);

//  person interval filters for sensos
	const Json::Value sensors_interval
		= root["server"]["queue"]["sensors_interval"];

	for(size_t i = 0; i < sensors_interval.size(); i++)
	{
		aggregator->QueueAggregator().SetIntervalS(sensors_interval[i][0u].asInt()
			, sensors_interval[i][1u].asInt());

		printf("sensor %d has personal interval %d\n"
			, sensors_interval[i][0u].asInt()
			, sensors_interval[i][1u].asInt());
	}

//  person delta filters for sensos
	const Json::Value sensors_delta
		= root["server"]["queue"]["sensors_delta"];

	for(size_t i = 0; i < sensors_delta.size(); i++)
	{
		aggregator->QueueAggregator().SetDeltaS(sensors_delta[i][0u].asInt()
			, sensors_delta[i][1u].asDouble());
		printf("sensor %d has personal delta %lg\n"
			, sensors_delta[i][0u].asInt(), sensors_delta[i][1u].asDouble());
	}

//  person abs_delta filters for sensos
	const Json::Value sensors_abs_delta
		= root["server"]["queue"]["sensors_abs_delta"];

	for(size_t i = 0; i < sensors_abs_delta.size(); i++)
	{
		aggregator->QueueAggregator().SetAbsDeltaS(sensors_abs_delta[i][0u].asInt()
			, sensors_abs_delta[i][1u].asDouble());

		printf("sensor %d has personal abs_delta %lg\n"
			, sensors_abs_delta[i][0u].asInt()
			, sensors_abs_delta[i][1u].asDouble());
	}

// names
	const Json::Value names = root["server"]["sensor_names"];

	for(size_t i = 0; i < names.size(); i++)
	{
		Json::Value sensor = names[i];

		id_to_name[sensor[0u].asInt()] = sensor[1u].asString();

		printf("name for %d is %s\n", sensor[0u].asInt()
			, sensor[1u].asCString());
	}

// average
	const Json::Value average = root["server"]["queue"]["average"];

	for(size_t i = 0; i < average.size(); i++)
	{
		int sensor = average[i].asInt();
		aggregator->QueueAggregator().RegisterAverageId(sensor);

		printf("average will be calculated for %d\n", sensor);
	}

// speed
	const Json::Value speed = root["server"]["queue"]["speed"];

	for(size_t i = 0; i < speed.size(); i++)
	{
		int sensor = speed[i].asInt();
		aggregator->QueueAggregator().RegisterSpeedId(sensor);

		printf("speed will be calculated for %d\n", sensor);
	}

// blacklist
	const Json::Value blacklist = root["server"]["queue"]["blacklist"];

	for(size_t i = 0; i < blacklist.size(); i++)
	{
		int sensor = blacklist[i].asInt();
		aggregator->QueueAggregator().BlacklistId(sensor);

		printf("sensor %d will be ignored\n", sensor);
	}

// agents
	const Json::Value agents = root["agents"]["list"];

	BackgroundInit(aggregator, agents);

	return aggregator;
}


#endif