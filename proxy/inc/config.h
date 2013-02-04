#ifndef CONFIG_H
#define CONFIG_H

#include "config.h"

#include <string>

using std::string;

#include <json/json.h>

#include "aggregator_api.h"

/**
	service function, init agents with delay in separate thread
*/
void BackgroundInitHelper(int agg_id, Json::Value agents);

/**
	run agent initialisation in separate thread with sleep, 
	to prevent flooding network and synchronous message sending
*/
void BackgroundInit(int agg_id, Json::Value agents);

/**
	read config from JSON /fname and fill \id_to_name: sensor_id -> "human name" map
	JSON is temporary
	TODO: move to YAML, check that all fields are filled
*/
int ConfigAggregator(const string& fname, std::unordered_map<uint16_t, string>& id_to_name);

#endif