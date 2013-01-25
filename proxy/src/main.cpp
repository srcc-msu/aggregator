#include <iostream>
#include <string>
#include <cstring>

#include <unordered_map>

using std::cout;
using std::cerr;
using std::endl;
using std::strcmp;

#include "mongoose.h"
#include "config.h"

#include "server.h"

int agg_id = -1;

std::unordered_map<uint16_t, string> id_to_name;

static const int SESS_ID_SIZE = 128;

bool streaming = false;

/**
    get \sess_id from \query_string
    returns 1 if successful
*/
int GetSessID(const char* query_string, char* sess_id)
{
    return sscanf(query_string, "sessid=%128s", sess_id); // const SESS_ID_SIZE
}

static void* callback(mg_event event, mg_connection *conn) 
{
    static int success;

    const struct mg_request_info *request_info = mg_get_request_info(conn);

    if (event == MG_NEW_REQUEST) 
    {
        cout << request_info->uri << endl;
        cout << request_info->request_method << endl;
        cout << request_info->query_string << endl;

        if (strcmp(request_info->uri, "/subscribe") == 0)
        {
            mg_printf(conn, "NIY\n");
            cout << "new subscriber, NIY" << endl;

            return &success;
        }

        else if (strcmp(request_info->uri, "/drop") == 0)
        {
            GetAllData(agg_id, nullptr);

            mg_printf(conn, "dropped\n");
            cout << "agregator queue dropped by request" << endl;


            return &success;
        }

        else if (strcmp(request_info->uri, "/session/new") == 0) 
        {
            char sess_id[SESS_ID_SIZE] = "";

            if(GetSessID(request_info->query_string, sess_id) != 1)
            {
                mg_printf(conn, "wrong query string, must be 'sessid=<anything here>'\n");
                return nullptr;
            }

            mg_printf(conn, "registered %s\n", sess_id);

            cout << "new session " << sess_id << endl;

            return &success;
        }

        else if (strcmp(request_info->uri, "/stream") == 0) 
        {
            if(streaming)
            {
                mg_printf(conn, "no more streaming!\n");
                cout << "no more streaming!\n" << endl;
            }

            streaming = true;

            char sess_id[SESS_ID_SIZE] = "";

            if(GetSessID(request_info->query_string, sess_id) != 1)
            {
                mg_printf(conn, "wrong query string, must be 'sessid=<anything here>'\n");
                return nullptr;
            }

            cout << "new connection " << sess_id << endl;

//  flash buffer to prevent overflowing of new subscrber
            GetAllData(agg_id, nullptr);

            DoStreaming(conn, agg_id, sess_id, id_to_name);

            streaming = false;
        }

        return &success;
    } 
    else 
        return nullptr;
}

int main(int argc, char** argv)
{
    string config_file = "aggregator.conf";

    if(argc == 2)
        config_file = string(argv[1]);

// read config and run aggregator
    if((agg_id = ConfigAggregator(config_file, id_to_name)) == -1)
    {
        cerr << "failed to configure aggregator" << endl;
        return 1;
    }

//  run webserver
    cout << endl << "init done, starting webserve" << endl << endl;

    mg_context* ctx;
    const char *options[] = {"listening_ports", "9293", NULL};

    ctx = mg_start(&callback, NULL, options);

    while(1)
        Process(agg_id);

    getchar();

    mg_stop(ctx);

    cout << "webserver and aggregator stoped" << endl;

    return 0;
}
