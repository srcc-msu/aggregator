#ifndef CONFIG_H
#define CONFIG_H

#include "config.h"

#include <string>

using std::string;

#include <json/json.h>

#include "aggregator_api.h"

/**
	service function, init agents with delay
*/
void BackgroundInitHelper(int agg_id, Json::Value agents);

/**
	runs agent initialisation in separate thread with sleep, 
	to prevent flooding network and synchronous mesыage sending
*/
void BackgroundInit(int agg_id, Json::Value agents);
int ConfigAggregator(const string config_file, std::unordered_map<uint16_t, string>& id_to_name);

#endif