#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include "aggregator_api.h"
#include <json/json.h>

using namespace std;

#include "mongoose.h"

int agg_id = -1;

static const unsigned int MIN_SLEEP_TIME = 10;

int i = 0;

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

        if(request_info->query_string == nullptr)
            return nullptr;

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

            cout << "new connection " << sess_id << endl;

            unsigned int sleep_time = MIN_SLEEP_TIME;

            while(1)
            {
                size_t count = 0;
                SPacket* l = GetAllData(agg_id, &count);

                if(count != 0)
                    cout << "fetched for " << sess_id << " " << count << ", sleep_time " << sleep_time << endl;

                for(size_t i = 0; i < count; i++)
                {
                    SPacket& p = l[i];

                    int printed = mg_printf(conn, 
                        "[[\"sb\",\"sensor\"],"
                        "{\"ip\":\"%u.%u.%u.%u\","
                        "\"sensor_id\":[%u,%u],"
                        "\"value\":%s,"
                        "\"gts\":%u.%u}]\n", 
                        p.address.b1[0], p.address.b1[1], p.address.b1[2], p.address.b1[3], 
                        p.sensor_id, p.sensor_num, p.data_string,
                        p.server_timestamp, p.server_usec);

                    if(printed == 0)
                    {
                        cout << "connection closed " << sess_id << endl;
                        return (void*) 1;
                    }
                }

                // to sleep or not to sleep? TODO check

                if(count == 0)
                    sleep_time *= 2;
                else
                    sleep_time /= 2;

                if(sleep_time < MIN_SLEEP_TIME)
                    sleep_time = MIN_SLEEP_TIME;

                usleep(sleep_time);
            }

            return (void*) 1;
        }

        return (void*) 1;
    } 
    else 
        return nullptr;
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

// agents
    const Json::Value agents = root["agents"]["list"];
    
    for(size_t i = 0; i < agents.size(); i++)
    {
        const char* agent = agents[i][1].asCString();

        InitAgent(agg_id, agent);
        cout << "agent " << agent << " added" << endl;
    }

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

    BackgroundProcess(agg_id);


//  running webserver
    mg_context* ctx;
    const char *options[] = {"listening_ports", "9294", NULL};

    ctx = mg_start(&callback, NULL, options);

    getchar();

    mg_stop(ctx);

    return 0;
}
