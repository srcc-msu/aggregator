#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include "aggregator_api.h"
#include <json/json.h>

#include <thread>

#include <unordered_map>

using namespace std;

#include "mongoose.h"

int agg_id = -1;

static const unsigned int MIN_SLEEP_TIME = 8;
static const unsigned int MAX_SLEEP_TIME = 1000000;

std::unordered_map<uint16_t, string> id_to_name;

/**
    get \sess_id from \query_string
    returns 1 if successful
*/
int GetSessID(const char* query_string, char* sess_id)
{
    return sscanf(query_string, "sessid=%s", sess_id);
}

static void* callback(mg_event event, mg_connection *conn) 
{
    const struct mg_request_info *request_info = mg_get_request_info(conn);

    if (event == MG_NEW_REQUEST) 
    {
        cout << request_info->uri << endl;
        cout << request_info->request_method << endl;
        cout << request_info->query_string << endl;

        if (strcmp(request_info->uri, "/subscribe") == 0)
        {
            printf("done %d\n",  mg_printf(conn, "done\n"));
            return (void*) 1;
        }

        if (strcmp(request_info->uri, "/session/new") == 0) 
        {
            char sess_id[128] = "";

            if(GetSessID(request_info->query_string, sess_id) != 1)
            {
                mg_printf(conn, "2 wrong query string, must be 'sessid=<anything here>'\n");
                return nullptr;
            }

            mg_printf(conn, "registered %s\n", sess_id);

            return (void*) 1;
        }

        else if (strcmp(request_info->uri, "/stream") == 0) 
        {
            char sess_id[128] = "";

            cout << GetSessID(request_info->query_string, sess_id) << endl;
            if(GetSessID(request_info->query_string, sess_id) != 1)
            {
                mg_printf(conn, "1 wrong query string, must be 'sessid=<anything here>'\n");
                return nullptr;
            }

            size_t count = 0;

            cout << "new connection " << sess_id << endl;

            GetAllData(agg_id, &count);

            unsigned int sleep_time = MIN_SLEEP_TIME;

            while(1)
            {
                SPacket* l = GetAllData(agg_id, &count);

                if(count != 0)
                    cout << "fetched for \"" << sess_id << "\" " << count << ", sleep_time " << sleep_time << endl;

                static char buffer[30000];
                int offset = 0;

                for(size_t i = 0; i < count; i++)
                {
                    SPacket& p = l[i];

                    int printed = sprintf(buffer + offset,
                        "[\"lap2\","
                        "{\"addr\":\"node-%03u-%02u\","
                        "\"name\":\"%s\","
                        "\"n\":\"%u\","
                        "\"val\":\"%s\","
                        "\"gts\":\"%u.%u\"}]\n",
                        p.address.b1[2], p.address.b1[3],
                        id_to_name[p.sensor_id].c_str(),
                        int(p.sensor_num),
                        p.data_string,
                        p.server_timestamp, p.server_usec);

/*                    int printed = sprintf(buffer + offset,
                        "[[\"sb\",\"sensor\"],"
                        "{\"ip\":\"%u.%u.%u.%u\","
                        "\"sensor_id\":[%s,%u],"
                        "\"value\":%s,"
                        "\"gts\":%u.%u}]\n", 
                        p.address.b1[0], p.address.b1[1], p.address.b1[2], p.address.b1[3], 
                        id_to_name[p.sensor_id].c_str(), int(p.sensor_num), p.data_string,
                        p.server_timestamp, p.server_usec);*/

                    offset += printed;
                    assert(offset < 30000);

                    if(offset >= 30000 - 1000 || (printed == 0 && offset > 0) || i == count - 1)
                    {
                        int streamed = mg_printf(conn, "%s", buffer);
                        offset = 0;

                        if(streamed == 0)
                        {
                            cout << "connection closed " << sess_id << endl;
                            return (void*) 1;
                        }
                    }
                }

//  to sleep or not to sleep? TODO check

                if(count <= 1000 && sleep_time < MAX_SLEEP_TIME)
                    sleep_time *= 2;
                else if(sleep_time > MIN_SLEEP_TIME)
                    sleep_time /= 2;

                usleep(sleep_time);
            }

            return (void*) 1;
        }

        return (void*) 1;
    } 
    else 
        return nullptr;
}

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

bool ConfigAggregator(const string config_file)
{
    std::ifstream t(config_file);
    std::stringstream buffer;
    buffer << t.rdbuf();

    Json::Value root;
    Json::Reader reader;

    if(!reader.parse(buffer.str(), root))
    {
        std::cout  << "failed to parse configuration" << endl 
            << reader.getFormatedErrorMessages () << endl;

        return false;
    }

//  aggregator addres
    const char* address = root["server"]["address"].asCString();
    agg_id = InitAggregator(address);
    cout << "Aggregator on " << address << " started" << endl;

//  default delta
    double delta = root["server"]["queue"]["delta"].asDouble();
    SetDelta(agg_id, delta);
    cout << "delta set to " << delta << endl;

//  default interval
    int interval = root["server"]["queue"]["interval"].asInt();
    SetInterval(agg_id, interval);
    cout << "time interval set to " << interval << endl;

//  sesnsor to be collected in the buffer
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

    BackgroundProcess(agg_id);
// agents
    const Json::Value agents = root["agents"]["list"];

    BackgroundInit(agg_id, agents);

    return true;
}

int main(int argc, char** argv)
{
    string config_file = "aggregator.conf";

    if(argc == 2)
    {
        config_file = string(argv[1]);
    }

//  running aggregator
    if(!ConfigAggregator(config_file))
    {
        cerr << "failed to configure aggregator" << endl;
        return 1;
    }

//  running webserver
    mg_context* ctx;
    const char *options[] = {"listening_ports", "9293", NULL};

    ctx = mg_start(&callback, NULL, options);

    getchar();

    mg_stop(ctx);

    return 0;
}
