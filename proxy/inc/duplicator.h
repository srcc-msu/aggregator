#include <vector>
#include <queue>
#include <mutex>
#include <memory>

using namespace std;

#include "error.h"

/**
    threadsafe duplicator for \T type messages to all subscirbers

    bad subscribing mutex system, TODO: change?
*/
template<typename T>
class CDuplicator
{
private:
    vector< queue< pair<T, size_t> > > subscribers;
// protects a single subscriber's queue
    vector< shared_ptr<mutex> > subscriber_mutex;

    mutex mutex_add;
    mutex mutex_get;

    size_t max_queue;

public:
    typedef T (*mult_duplicator)(const T, size_t&);

    void CheckQueue(size_t i)
    {
        if(subscribers[i].size() > max_queue)
        {
            subscribers[i].pop(); // TODO memory leak here
            fprintf(stderr, "message for %zu droppe, queueu is full\n", i);
        }
    }

/**
    duplicate and add message to all subscribers
*/
    int Add(T msg, size_t count, mult_duplicator DupFunc)
    {
        lock_guard<mutex> lock_add(mutex_add);

//  duplicate and add \msg to everyone except 1st
//        if(subscribers.size() > 1)
            for(size_t i = 0; i < subscribers.size(); i++)
            {
                lock_guard<mutex> lock(*subscriber_mutex[i]);

                CheckQueue(i);

                auto dup = DupFunc(msg, count); // count changes
                subscribers[i].push(make_pair(dup, count));
            }
/*
//  add original pointer to first: serve it last to keep pointer alive
        if(subscribers.size() > 0)
        {
            lock_guard<mutex> lock(*subscriber_mutex[0]);

            CheckQueue(0);

            subscribers[0].push(make_pair(msg, count));
        }*/
        return subscribers.size();
    };

/**
    get one message from \subscriber_id queue
*/
    T Get(size_t subscriber_id, size_t* count)
    {
        if(subscriber_id >= subscribers.size())
            throw CException("attempt to get data from unknown subscriber");

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
    void DeleteSubscriber(size_t subscriber_id)
    {
        throw CException("NIY");

        if(subscriber_id >= subscribers.size())
            throw CException("attempt to delete unknown subscriber");

        lock_guard<mutex> lock_add(mutex_add);
        lock_guard<mutex> lock_get(mutex_get);

//        subscribers.erase(subscribers.begin + subscriber_id);
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