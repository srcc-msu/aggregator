#ifndef SERVER_H
#define SERVER_H

#include <string>

#include <unordered_map>

using std::string;

#include "mongoose.h"

#include "aggregator_api.h"

const unsigned int MIN_SLEEP_TIME = 8;
const unsigned int MAX_SLEEP_TIME = 1000000;

const size_t SEND_BUFFER = 65536;
const size_t FETCH_THRESHOLD = 1024;

/**
    reads data from the aggregator \agg_id and stream it by chunks to \conn
*/
void DoStreaming(mg_connection *conn, int agg_id, const char * sess_id,
    std::unordered_map<uint16_t, string>& id_to_name);

#endif
