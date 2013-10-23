#include <map>
#include <queue>
#include <mutex>
#include <atomic>
#include <memory>

using namespace std;

#include "error.h"
#include "utils.h"

template <typename T>
class CSubscriber
{
private:
	size_t uid;

	queue< pair<T, size_t> > data_queue;
	size_t max_queue;

	mutex my_mutex;


public:
	size_t Add(T data, size_t count)
	{
		lock_guard<mutex> lock_sub(my_mutex);

		if(data_queue.size() > max_queue)
		{
			fprintf(stderr, "message for %zu dropped, queueu is full\n", uid);
			return 0;
		}

		data_queue.push(make_pair(data, count));

		return count;
	}

	T Get(size_t* count)
	{
		lock_guard<mutex> lock_sub(my_mutex);

		if(data_queue.size() == 0)
		{
			*count = 0;
			return nullptr;
		}

		auto front = data_queue.front().first;
		*count = data_queue.front().second;
		data_queue.pop();

		return front;
	}

	CSubscriber(size_t _uid, size_t _max_queue):
		uid(_uid),
		max_queue(_max_queue)
	{}
};

/**
	NOT threadsafe duplicator for \T type messages to all subscirbers

	bad subscribing mutex system, TODO: change?
*/
template<typename T>
class CDuplicator
{
private:
	size_t next_uid;

	map<size_t, shared_ptr<CSubscriber<T>> > subscribers;

	size_t max_queue;

	atomic<int> read_allowed;

	CSharedLock lock;

public:
	typedef T (*mult_duplicator)(const T, size_t&);

/**
	duplicate and add message to all subscribers
	returns how many have successfuly added the message to the queue
*/
	size_t Add(T msg, size_t count, mult_duplicator DupFunc)
	{
		lock.ReadLock();

		size_t sent = 0;

		for(auto& subscriber : subscribers)
		{
			auto dup = DupFunc(msg, count); // count changes

			sent += subscriber.second->Add(dup, count);
		}

		lock.ReadUnlock();
		return sent;
	};

/**
	get one message from \subscriber_id queue
*/
	bool IsAlive(size_t uid)
	{
		lock.ReadLock();

		auto subscriber = subscribers.find(uid);
		bool res = (subscriber != subscribers.end());

		lock.ReadUnlock();

		return res;
	}

/**
	get one message from \subscriber_id queue
*/
	T Get(size_t uid, size_t* count)
	{
		lock.ReadLock();

		auto subscriber = subscribers.find(uid);

		if(subscriber == subscribers.end())
		{
			count = 0;

			lock.ReadUnlock();
			return nullptr;
		}

		T ret = subscriber->second->Get(count);

		lock.ReadUnlock();
		return ret;
	}

/**
	delete subscriber
*/
	void DeleteSubscriber(size_t uid)
	{
		lock.WriteLock();

		auto subscriber = subscribers.find(uid);

		if(subscriber != subscribers.end())
			subscribers.erase(subscriber);
		else
			fprintf(stderr, "attempt to delete unknown subscriber\n");

		lock.WriteUnlock();

		fprintf(stderr, "subscriber %zu killed\n", uid);
	}

/**
	add new subscriber
*/
	size_t Subscribe()
	{
		lock.WriteLock();

		subscribers[next_uid] = std::make_shared<CSubscriber<T>>(next_uid, max_queue);

		lock.WriteUnlock();

		return next_uid++;
	};

	CDuplicator(size_t _max_queue):
		next_uid(0),
		max_queue(_max_queue)
	{}
};