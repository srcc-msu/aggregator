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

public:
/**
	Add few values to the queue.
*/
	void Add(const std::vector<T>& values)
	{
		std::lock_guard<std::mutex> lock(mutex);

		if(queue.size() + values.size() >= MAX_QUEUE_SIZE)
		{
			queue = std::vector<T>();
		}

		queue.insert(queue.end(), values.begin(), values.end());
	}

	void GetData(std::vector<T>& target)
	{
		std::lock_guard<std::mutex> lock(mutex);

		target.swap(queue);

		queue = std::vector<T>();
	}
};

#endif
