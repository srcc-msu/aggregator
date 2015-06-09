#ifndef QUEUE_H
#define QUEUE_H

#include <mutex>
#include <cstring>
#include <vector>

#include "debug.h"
#include "error.h"

const size_t MAX_QUEUE_SIZE = 1024 * 1024;

/**
	Thread safe sensor queue. (ORLY?)
	accumulates single values and return them all by request.
*/
template<typename T>
class CSensorQueue
{
private:
	std::mutex mutex;
	std::vector<T> queue;

/**
	Get all data from the queue.
	Current container will be returned and new one will be created
*/
	std::vector<T> Refresh()
	{
		std::vector<T> tmp = std::vector<T>();
		tmp.swap(queue);

		return tmp;
	}

public:
/**
	Add few values to the queue.
*/
	void Add(const std::vector<T>& values)
	{
		std::lock_guard<std::mutex> lock(mutex);

		if(queue.size() + values.size() >= MAX_QUEUE_SIZE)
		{
			Refresh();
		}

		queue.insert(queue.end(), values.begin(), values.end());
	}

	std::vector<T> GetData()
	{
		std::lock_guard<std::mutex> lock(mutex);

		return Refresh();
	}
};

#endif