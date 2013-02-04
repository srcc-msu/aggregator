#include <vector>
#include <queue>
#include <mutex>
#include <memory>

#include <cassert>

using namespace std;

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