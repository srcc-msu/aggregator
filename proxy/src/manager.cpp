#include "manager.h"

#include <thread>
#include <cstring>

using namespace std;

#include "config.h"

const SPacket* DupPacket(const SPacket* val, size_t& count)
{
    int addition = 0;

    if(count % MEM_CHUNK) // make \mem_size % \MEM_CHUNK == 0
        addition = MEM_CHUNK - count % MEM_CHUNK;

    int mem_size = count + addition;

    SPacket* t = new SPacket[mem_size];

    memcpy(t, val, sizeof(SPacket) * count);

    memset(t + count, 0, addition * sizeof(SPacket));

    count = mem_size;

    return t;
}

void CProxyManager :: BackgroundStreamHelper(CFdWriter* writer
    , size_t subscriber_id)
{
    CDynSleeper sleeper;

    printf("new subscriber %zu\n", subscriber_id);

    int step = 0;

    while(1)
    {
        size_t count = 0;
        auto msg = duplicator.Get(subscriber_id, &count);

        if(count > 0)
        {
            int bytes_send = writer->Write(msg, count);

            if(bytes_send == -1)
            {
                printf("subscriber %zu disconnected\n", subscriber_id);
                delete writer;
                writer = nullptr;
                return;
            }

            if(step++ % 32 == 0) // print just few for statistics
                printf("streamed %5zu to %2zu (%d bytes) ; sleeping %8d\n",
                    count, subscriber_id, sleeper.GetTime(), bytes_send);

            delete[] msg;
        }

        sleeper.Sleep(count == 0);
    }

    printf("end\n");
}

void CProxyManager :: BackgroundStream(CFdWriter* writer
    , size_t subscriber_id)
{
    thread t(&CProxyManager :: BackgroundStreamHelper
        , this, writer, subscriber_id);

    t.detach();
}

void CProxyManager :: BackgroundDispatchHelper()
{
    CDynSleeper sleeper;

    int step = 0;

    while(1)
    {
        size_t count = Dispatch();

        if(count != 0 && step++ % 32 == 0) // print just few for statistics
            printf("duplicated %5zu ; sleeping %8d\n"
                , count, sleeper.GetTime());

        sleeper.Sleep(count < 1024);
    }
}

void CProxyManager :: Config(const string& config_fname)
{
    if((agg_id = ConfigAggregator(config_fname, id_to_name)) == -1)
        fprintf(stderr,  "failed to configure aggregator\n");
}

int CProxyManager :: Dispatch()
{
    size_t count = 0;
    const SPacket* packets = GetAllData(agg_id, &count);

    if(count == 0)
        return count;

    int sub = duplicator.Add(packets, count, DupPacket);

    if(packets)
        delete[] packets;

    return (sub == 0) ? 0 : count;
}

void CProxyManager :: BackgroundDispatch()
{
    static bool started = false;

    if(started == true)
        return;

    started = true;

    thread t(&CProxyManager :: BackgroundDispatchHelper, this);

    t.detach();
}
