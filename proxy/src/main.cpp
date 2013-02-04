#include <vector>
#include <queue>
#include <iostream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <mutex>
#include <thread>

#include <signal.h>

#include <utility>
#include <memory>

using namespace std;

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "aggregator_api.h"

#include "config.h"
#include "error.h"
#include "utils.h"

#ifndef UNIX_PATH_MAX
    #define UNIX_PATH_MAX 108
#endif

/**
    threadsafe duplicator for \T type messages

    bad subscribing mutex system, TODO: change?

    \mutex - to protect each queue
    \sub_mutex - to protectlist of subscribers
*/
template<class T>
class CDuplicator
{
private:
    vector< queue< pair<T, size_t> > > subscribers;
    vector< shared_ptr<mutex> > subscriber_mutex;

    mutex mutex_add;
    mutex mutex_get;
   
    size_t max_queue;

public:
    typedef T (*mult_duplicator)(T, size_t);

/**
    duplicate and add message to all subscribers
*/
    void Add(T msg, size_t count, mult_duplicator DupFunc)
    {
        lock_guard<mutex> lock_add(mutex_add);

//  duplicate and add \msg to everyone except 1st      
        if(subscribers.size() > 1)
            for(size_t i = 1; i < subscribers.size(); i++)
            {
                lock_guard<mutex> lock(*subscriber_mutex[i]);

                if(subscribers[i].size() < max_queue)
                    subscribers[i].push(make_pair(DupFunc(msg, count), count));
            }

//  add original pointer to first: serve it last to keep pointer alive
        if(subscribers.size() > 0)
        {
            lock_guard<mutex> lock(*subscriber_mutex[0]);

            if(subscribers[0].size() < max_queue)
                subscribers[0].push(make_pair(msg, count));
        }
    };

/**
    get one message from \subscriber_id queue
*/
    T Get(size_t subscriber_id, size_t* count)
    {
        assert(subscriber_id < subscribers.size());

        lock_guard<mutex> lock_get(mutex_get);

        lock_guard<mutex> lock_sub(*subscriber_mutex[subscriber_id]);

        if(subscribers[subscriber_id].size() == 0)
        {
            count = 0;
            return nullptr;
        }

        auto front = subscribers[subscriber_id].front().first;
        *count = subscribers[subscriber_id].front().second;
        subscribers[subscriber_id].pop();

        return front;
    };

/**
    delete subscriber
*/
    size_t DeleteSubscriber(size_t subscriber_id)
    {
        assert(subscriber_id > subscribers.size());

        lock_guard<mutex> lock_add(mutex_add);
        lock_guard<mutex> lock_get(mutex_get);

        subscribers.erase(subscribers.begin + subscriber_id);
    }

/**
    add new subscriber
*/
    size_t Subscribe()
    {
        lock_guard<mutex> lock_add(mutex_add);
        lock_guard<mutex> lock_get(mutex_get);

        size_t subscriber_id = subscribers.size();
        subscribers.push_back(queue< pair<T, size_t> >());

        subscriber_mutex.push_back(make_shared<mutex>());

        return subscriber_id;
    };

    CDuplicator(size_t _max_queue):
    max_queue(_max_queue)
    {}
};

SPacket* DupPacket(SPacket* val, size_t count)
{
    SPacket* t = new SPacket[count];
    
    memcpy(t, val, sizeof(SPacket) * count);
    
    return t;
}

class CFdWriter
{
protected:
    int fd;

public:
    virtual int Write(SPacket* p, int count) = 0;

    CFdWriter(int _fd):
    fd(_fd)
    {}
};

class CBinaryFdWriter : public CFdWriter
{
private:

public:
    int Write(SPacket* p, int count)
    {
        return write(fd, p, count * sizeof(SPacket));
    }

    CBinaryFdWriter(int _fd):
    CFdWriter(_fd)
    {}
};

class CJsonFdWriter : public CFdWriter
{
private:

public:
    int Write(SPacket* p, int count)
    {
        assert(false);

        char buffer[128];

        int bs = sprintf(buffer, "{%d,%d}", p->sensor_id, p->sensor_num);

        return write(fd, buffer, bs);
    }

    CJsonFdWriter(int _fd):
    CFdWriter(_fd)
    {}
};


class CProxyManager
{
private:
    int agg_id;
    unordered_map<uint16_t, string> id_to_name;

    CDuplicator<SPacket*> duplicator;

/**
    separate thread, may block if pipe is full
*/
    void BackgroundStreamHelper(CFdWriter* writer, size_t subscriber_id)
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

    void BackgroundStream(CFdWriter* writer, size_t subscriber_id)
    {
        thread t(&CProxyManager :: BackgroundStreamHelper, this, writer, subscriber_id);

        t.detach();
    }

    void BackgroundDispatchHelper()
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

public:

    void AddBinaryStream(int fd)
    {
        BackgroundStream(new CBinaryFdWriter(fd), duplicator.Subscribe());
    }

    void AddJsonStream(int fd)
    {
        BackgroundStream(new CJsonFdWriter(fd), duplicator.Subscribe());
    }

    void Init(string config_fname)
    {
        if((agg_id = ConfigAggregator(config_fname, id_to_name)) == -1)
            fprintf(stderr,  "failed to configure aggregator\n");
    }

    void BackgroundProcess()
    {
        ::BackgroundProcess(agg_id);
    }

    int Dispatch()
    {
        size_t count = 0;
        auto packets = GetAllData(agg_id, &count);
        
        if(count == 0)
            return count;

        duplicator.Add(packets, count, DupPacket);
        
        return count;
    }  
    
    void BackgroundDispatch()
    {
        static bool started = false;

        if(started == true)
            return;

        started = true;

        thread t(&CProxyManager :: BackgroundDispatchHelper, this);

        t.detach();
    }

    CProxyManager(string config_fname):
    duplicator(10)
    {
        Init(config_fname);
    }
};

int CreateSocket(string fname)
{
    sockaddr_un address;
    memset(&address, 0, sizeof(struct sockaddr_un));
    
    socklen_t address_length;

    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    signal(SIGPIPE, SIG_IGN); // TODO do some other way?
    
    if(socket_fd == -1)
        throw CSyscallException("socket() creation failed");

    unlink(fname.c_str());

    /* start with a clean address structure */

    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, UNIX_PATH_MAX, fname.c_str());

    if(::bind(socket_fd, (struct sockaddr *) &address, 
        sizeof(struct sockaddr_un)) != 0)
    {
        throw CSyscallException("bind() creation failed");
    }

    if(listen(socket_fd, 1) != 0)
        throw CSyscallException("listen() creation failed");

    return accept(socket_fd, (struct sockaddr *) &address, 
        &address_length);
}

int main(int argc, char** argv)
{
    printf("\n --- starting proxy --- \n\n");

    string config_fname = "aggregator.conf";

    if(argc == 2)
        config_fname = string(argv[1]);

// read config and run aggregator

    CProxyManager proxy(config_fname);

//    int fd1 = open("test1", O_WRONLY | O_CREAT, 0666);
    proxy.BackgroundProcess();

    proxy.BackgroundDispatch();

    int fd = CreateSocket("/tmp/agg_socket");

    proxy.AddBinaryStream(fd);

    while(1)
    {
        sleep(1);
    }

    close(fd);

    printf("\n --- proxy finished --- \n\n");

    return 0;
}

/* 
     ]
    },

    "agents2" :
    {
        "list" :
        [
*/
