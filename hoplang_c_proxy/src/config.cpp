#include "config.h"

#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>

using std::cout;
using std::endl;

void BackgroundInitHelper(int agg_id, Json::Value agents)
{
    for(size_t i = 0; i < agents.size(); i++)
    {
        const char* agent = agents[i][1].asCString();

        InitAgent(agg_id, agent);
        cout << "agent " << agent << " added" << endl;
        usleep(10000);
    }
}

void BackgroundInit(int agg_id, Json::Value agents)
{
	static bool started = false;

	if(started == true)
		return;

	started = true;

    std::thread t1(BackgroundInitHelper, agg_id, agents);

    t1.detach();
}

int ConfigAggregator(const string config_file, std::unordered_map<uint16_t, string>& id_to_name)
{
    std::ifstream tmp(config_file);
    std::stringstream buffer;
    buffer << tmp.rdbuf();

    Json::Value root;
    Json::Reader reader;

    if(!reader.parse(buffer.str(), root))
    {
        std::cout  << "failed to parse configuration" << endl 
            << reader.getFormatedErrorMessages () << endl;

        return -1;
    }

//  aggregator addres
    const char* address = root["server"]["address"].asCString();
    int agg_id = InitAggregator(address);
    cout << "Aggregator on " << address << " started" << endl;

//  default delta
    double delta = root["server"]["queue"]["delta"].asDouble();
    SetDelta(agg_id, delta);
    cout << "delta set to " << delta << endl;

//  default abs_delta
    double abs_delta = root["server"]["queue"]["abs_delta"].asDouble();
    SetAbsDelta(agg_id, abs_delta);
    cout << "abs_delta set to " << abs_delta << endl;

//  default interval
    int interval = root["server"]["queue"]["interval"].asInt();
    SetInterval(agg_id, interval);
    cout << "time interval set to " << interval << endl;

//  person interval filters for sensos
    const Json::Value sensors_interval = root["server"]["queue"]["sensors_interval"];

    for(size_t i = 0; i < sensors_interval.size(); i++)
    {
        SetIntervalS(agg_id, sensors_interval[i][0u].asInt(), sensors_interval[i][1u].asInt());
        cout << "sensor " << sensors_interval[i][0u].asInt() << " has personal interval " << sensors_interval[i][1u].asInt() << endl;
    }

//  person delta filters for sensos
    const Json::Value sensors_delta = root["server"]["queue"]["sensors_delta"];

    for(size_t i = 0; i < sensors_delta.size(); i++)
    {
        SetDeltaS(agg_id, sensors_delta[i][0u].asInt(), sensors_delta[i][1u].asDouble());
        cout << "sensor " << sensors_delta[i][0u].asInt() << " has personal delta " << sensors_delta[i][1u].asDouble() << endl;
    }

//  person abs_delta filters for sensos
    const Json::Value sensors_abs_delta = root["server"]["queue"]["sensors_abs_delta"];

    for(size_t i = 0; i < sensors_abs_delta.size(); i++)
    {
        SetAbsDeltaS(agg_id, sensors_abs_delta[i][0u].asInt(), sensors_abs_delta[i][1u].asDouble());
        cout << "sensor " << sensors_abs_delta[i][0u].asInt() << " has personal abs_delta " << sensors_abs_delta[i][1u].asDouble() << endl;
    }

//  sensors to be collected in the buffer
    const Json::Value sensors = root["server"]["buffer"]["sensors"];

    for(size_t i = 0; i < sensors.size(); i++)
    {
        int sensor = sensors[i].asInt();

        BufferAllowId(agg_id, sensor);
        cout << "sensor " << sensor << " will be collected in buffer" << endl;
    }

// names
    const Json::Value names = root["server"]["sensor_names"];

    for(size_t i = 0; i < names.size(); i++)
    {
        Json::Value sensor = names[i];

        id_to_name[sensor[0u].asInt()] = sensor[1u].asString();

        cout << "name for " << sensor[0u].asInt() << " is " << sensor[1u].asString() << endl;
    }

// average
    const Json::Value average = root["server"]["queue"]["average"];

    for(size_t i = 0; i < average.size(); i++)
    {
        int sensor = average[i].asInt();
        QueueRegisterAverageId(agg_id, sensor);

        cout << "average will be calculated for " << sensor << endl;
    }

// blacklist
    const Json::Value blacklist = root["server"]["queue"]["blacklist"];

    for(size_t i = 0; i < blacklist.size(); i++)
    {
        int sensor = blacklist[i].asInt();
        QueueBlacklistId(agg_id, sensor);

        cout << "sensor " << sensor << " will be ignored" << endl;
    }

// agents
    const Json::Value agents = root["agents"]["list"];

    BackgroundInit(agg_id, agents);

    return agg_id;
}
