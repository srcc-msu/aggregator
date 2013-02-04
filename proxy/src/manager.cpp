#include "manager.h"

#include <thread>
#include <cstring>

using namespace std;

#include "config.h"

SPacket* DupPacket(SPacket* val, size_t count)
{
    SPacket* t = new SPacket[count];
    
    memcpy(t, val, sizeof(SPacket) * count);
    
    return t;
}

void CProxyManager ::BackgroundStreamHelper(CFdWriter* writer, size_t subscriber_id)
{
    CDynSleeper sleeper;

    while(1)
    {
        size_t count = 0;
        auto msg = duplicator.Get(subscriber_id, &count);

        if(count > 0)
        {
            int bs = writer->Write(msg, count);

            delete[] msg;

            if(bs == -1)
            {
                printf("subscriber %zu disconnected\n", subscriber_id);
                delete writer;
                writer = nullptr;
                return;
            }

            printf("streamed     %5zu to %2zu; sleeping         %8d\n", 
                count, subscriber_id, sleeper.GetTime());
        }

        sleeper.Sleep(count == 0);
    }
}

void CProxyManager :: BackgroundStream(CFdWriter* writer, size_t subscriber_id)
{
    thread t(&CProxyManager :: BackgroundStreamHelper, this, writer, subscriber_id);

    t.detach();
}

void CProxyManager :: BackgroundDispatchHelper()
{
    CDynSleeper sleeper;

    while(1)
    {
        size_t count = Dispatch();

        printf("duplicated   %5zu      ; sleeping %8d\n",
            count, sleeper.GetTime());

        sleeper.Sleep(count < 1000);
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
    auto packets = GetAllData(agg_id, &count);
    
    if(count == 0)
        return count;

    duplicator.Add(packets, count, DupPacket);
    
    return count;
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
