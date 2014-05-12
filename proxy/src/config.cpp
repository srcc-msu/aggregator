#include "config.h"

#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
//#include <chrono>
#include <unistd.h>

using namespace std;

#include "error.h"

void BackgroundInitHelper(int agg_id, Json::Value agents)
{
	for(size_t i = 0; i < agents.size(); i++)
	{
		const char* agent = agents[i][1].asCString();

		InitAgent(agg_id, agent);
		printf("agent %s added\n", agent);

//      this_thread::sleep_for(std::chrono::milliseconds(10)); // not working in gcc 4.6
		usleep(10000);
	}
}

//--------------------------------

void BackgroundInit(int agg_id, Json::Value agents)
{
	static bool started = false;

	if(started == true)
		return;

	started = true;

	std::thread t1(BackgroundInitHelper, agg_id, agents);

	t1.detach();
}

//--------------------------------

int ConfigAggregator(const string& config_fname
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

//  aggregator addres
	const char* address = root["server"]["address"].asCString();
	int agg_id = InitAggregator(address);
	printf("Aggregator on %s\n", address);

//  default delta
	double delta = root["server"]["queue"]["delta"].asDouble();
	SetDelta(agg_id, delta);
	printf("delta set to %lg\n", delta);

//  default abs_delta
	double abs_delta = root["server"]["queue"]["abs_delta"].asDouble();
	SetAbsDelta(agg_id, abs_delta);
	printf("abs_delta set to %lg\n", abs_delta);

//  default interval
	int interval = root["server"]["queue"]["interval"].asInt();
	SetInterval(agg_id, interval);
	printf("time interval set to %d\n", interval);

//  person interval filters for sensos
	const Json::Value sensors_interval
		= root["server"]["queue"]["sensors_interval"];

	for(size_t i = 0; i < sensors_interval.size(); i++)
	{
		SetIntervalS(agg_id, sensors_interval[i][0u].asInt()
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
		SetDeltaS(agg_id, sensors_delta[i][0u].asInt()
			, sensors_delta[i][1u].asDouble());
		printf("sensor %d has personal delta %lg\n"
			, sensors_delta[i][0u].asInt(), sensors_delta[i][1u].asDouble());
	}

//  person abs_delta filters for sensos
	const Json::Value sensors_abs_delta
		= root["server"]["queue"]["sensors_abs_delta"];

	for(size_t i = 0; i < sensors_abs_delta.size(); i++)
	{
		SetAbsDeltaS(agg_id, sensors_abs_delta[i][0u].asInt()
			, sensors_abs_delta[i][1u].asDouble());

		printf("sensor %d has personal abs_delta %lg\n"
			, sensors_abs_delta[i][0u].asInt()
			, sensors_abs_delta[i][1u].asDouble());
	}

//  sensors to be collected in the buffer
	const Json::Value sensors = root["server"]["buffer"]["sensors"];

	for(size_t i = 0; i < sensors.size(); i++)
	{
		int sensor = sensors[i].asInt();

		BufferAllowId(agg_id, sensor);
		printf("sensor %d will be collected in buffer\n", sensor);
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
		QueueRegisterAverageId(agg_id, sensor);

		printf("average will be calculated for %d\n", sensor);
	}

// speed
	const Json::Value speed = root["server"]["queue"]["speed"];

	for(size_t i = 0; i < speed.size(); i++)
	{
		int sensor = speed[i].asInt();
		QueueRegisterSpeedId(agg_id, sensor);

		printf("speed will be calculated for %d\n", sensor);
	}

// blacklist
	const Json::Value blacklist = root["server"]["queue"]["blacklist"];

	for(size_t i = 0; i < blacklist.size(); i++)
	{
		int sensor = blacklist[i].asInt();
		QueueBlacklistId(agg_id, sensor);

		printf("sensor %d will be ignored\n", sensor);
	}

// agents
	const Json::Value agents = root["agents"]["list"];

	BackgroundInit(agg_id, agents);

	return agg_id;
}
