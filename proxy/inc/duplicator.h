#include <map>
#include <queue>
#include <mutex>
#include <atomic>
#include <memory>

using namespace std;

#include "fd_writer.h"
#include "error.h"
#include "utils.h"

class CSubscriber
{
private:
	size_t uid;

	queue<std::shared_ptr<std::vector<SPacket>>> data_queue;
	size_t max_queue;

	mutex my_mutex;

public:
	CSubscriber(size_t _uid, size_t _max_queue):
		uid(_uid),
		max_queue(_max_queue)
	{}

	size_t Add(std::shared_ptr<std::vector<SPacket>> data)
	{
		lock_guard<mutex> lock_sub(my_mutex);

		if(data_queue.size() > max_queue)
		{
			fprintf(stderr, "message for %zu dropped, queueu is full\n", uid);
			return 0;
		}

		data_queue.push(data);

		return data->size();
	}

	void BackgroundStreamHelper(std::shared_ptr<CFdWriter> writer)
	{
		CDynSleeper sleeper;

		printf("new subscriber %zu\n", uid);

		int step = 0;

		while(1)
		{
			std::shared_ptr<std::vector<SPacket>> data;

			{
				lock_guard<mutex> lock_sub(my_mutex);

				if(data_queue.empty())
				{
					sleeper.Sleep(true);
					continue;
				}

				data = data_queue.front();
				data_queue.pop();
			}

			ssize_t bytes_send = writer->Write(*data);

			if(bytes_send == -1) // hm, what to do..
			{

			}

			if(step++ % 100 == 0) // print just few for statistics
				printf("streamed %5zu to %zu (%d bytes) ; sleeping %8zu\n",
					data->size(), uid, sleeper.GetTime(), bytes_send);
		}
	}
};

// --------------------------------------------------------------------------------------- 

class CDuplicator
{
private:
	size_t next_uid;

	map<size_t, shared_ptr<CSubscriber>> subscribers;

	size_t max_queue;

	CSharedLock lock;

public:
/**
	duplicate and add message to all subscribers
	returns how many have successfuly added the message to the queue
*/
	size_t Add(std::vector<SPacket> data)
	{
		lock.ReadLock();

		std::shared_ptr<std::vector<SPacket>> copy = make_shared<std::vector<SPacket>>();

		copy->insert(copy->end(), data.begin(), data.end());

		size_t added = 0;

		for(auto& subscriber : subscribers)
		{
			added += subscriber.second->Add(copy);
		}

		lock.ReadUnlock();

		return added;
	};

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
	size_t Subscribe(std::shared_ptr<CBinaryFdWriter> writer)
	{
		lock.WriteLock();

		subscribers[next_uid] = std::make_shared<CSubscriber>(next_uid, max_queue);

		lock.WriteUnlock();

		std::thread thread(&CSubscriber :: BackgroundStreamHelper, subscribers[next_uid], writer);

		thread.detach();

		return next_uid++;
	};

	CDuplicator(size_t _max_queue):
		next_uid(0),
		max_queue(_max_queue)
	{}
};