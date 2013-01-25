#include "server.h"

#include <iostream>
#include <cstdio>

using std::cout;
using std::endl;

/**
    reads data from the aggregator \agg_id and stream it by chunks to \conn
*/
void DoStreaming(mg_connection *conn, int agg_id, const char * sess_id,
    std::unordered_map<uint16_t, string>& id_to_name)
{
    unsigned int sleep_time = MIN_SLEEP_TIME;
    static char buffer[SEND_BUFFER];

    while(1)
    {
        size_t count = 0;
        SPacket* l = GetAllData(agg_id, &count);

        if(count != 0)
            cout << "fetched for \"" << sess_id << "\"  ---  " 
                << count << "  ---  , sleep_time " << sleep_time << endl;

        size_t offset = 0;

        for(size_t i = 0; i < count; i++)
        {
            SPacket& p = l[i];

            size_t printed = std::sprintf(buffer + offset,
                "[\"lap2\","
                "{\"addr\":\"node-%02u-%02u\","
                "\"name\":\"%s\","
                "\"n\":\"%u\","
                "\"val\":\"%s\","
                "\"gts\":\"%u.%u\"}]\n",
                p.address.b1[2], p.address.b1[3],
                id_to_name[p.sensor_id].c_str(),
                int(p.sensor_num),
                p.data_string,
                p.server_timestamp, p.server_usec);

            offset += printed;
            assert(offset < SEND_BUFFER);

//  stream chunk if...
            if(offset >= SEND_BUFFER - 200 || // buffer will be over soon
                i == count - 1) // it was last element
            {
                size_t streamed = mg_printf(conn, "%s", buffer);
                cout << "streamed "  << streamed << endl;

                offset = 0;

                if(streamed == 0)
                {
                    cout << "connection closed " << sess_id << endl;
                    return;
                }
            }
        }

//  kinda of clever sleep, that is basing on average aggregator load
//  if there are less than \FETCH_THRESHOLD items in aggregator - next sleep will be longer
//  to sleep or not to sleep? TODO check
        if(count <= FETCH_THRESHOLD && sleep_time < MAX_SLEEP_TIME)
            sleep_time *= 2;
        else if(sleep_time > MIN_SLEEP_TIME)
            sleep_time /= 2;

        usleep(sleep_time);
    }

    return;
}

