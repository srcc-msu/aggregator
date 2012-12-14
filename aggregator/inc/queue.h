#ifndef QUEUE_H
#define QUEUE_H

#include <mutex>
#include <assert.h>
#include <iostream>

#include <cstring>

const size_t MAX_QUEUE_SIZE = 1024 * 1024 * 16;

/**
	Thread safe sensor queue. (ORLY?)
	accumulates single values and return them all by request.
*/
template<class T>
class CSensorQueue
{
private:
	std::mutex mutex;

	T* queue;
	size_t pointer;
	T* old;

	void Reinit()
	{
		if(old != nullptr)
			delete[] old;

		old = queue;

		queue = new T[MAX_QUEUE_SIZE];
		pointer = 0;
	}

public:
/// Add single value to the queue.
	void Add(T value)
	{
		std::lock_guard<std::mutex> lock(mutex);

		if(pointer == MAX_QUEUE_SIZE)
			Reinit();

		queue[pointer] = value;
		pointer++;
	}

/** 
	Get all data from the queue.
	Current pointer will be returned and new one will be allocated - 
	old one will be purged on next request. Also it can be purged manually.
*/
	T* GetAll(size_t* count)
	{
		std::lock_guard<std::mutex> lock(mutex);
		
		*count = pointer;

		Reinit();
		return old;
	}

/** 
	Purge old pointer, if needed. 
	This function is called automatically on each new data request.
*/
	void RemoveOld()
	{
		std::lock_guard<std::mutex> lock(mutex);

		if(old != nullptr)
			delete[] old;

		old = nullptr;
	}

	CSensorQueue():
	queue(nullptr),
	old(nullptr)
	{
		Reinit();
	}

	~CSensorQueue()
	{
		if(old != nullptr)
			delete[] old;

		old = nullptr;

		delete[] queue;
		queue = nullptr;
	}
};


const size_t DEFAULT_BUFF_SIZE = 300;

/**
	Thread safe circular buffer.
	Accumulates single values, rotating them over time.
	Returns data from requested interval.
*/
template<class T>
class CCircularBuffer
{
private:
	mutable std::mutex mutex;
	T* buffer;
	T* old;
	
	size_t size;
	size_t pointer;

	CCircularBuffer& operator =(const CCircularBuffer& t)
	{}

	CCircularBuffer(const CCircularBuffer& t)
	{}


public:
	void Add(T value)
	{
		std::lock_guard<std::mutex> lock(mutex);

		buffer[pointer++] = value;

		if(pointer == size)
			pointer = 0;
	}

	T* Get(size_t n)
	{
		std::lock_guard<std::mutex> lock(mutex);

		if(n > size)
			n = size;

		T* res = new T[n];

		if(pointer >= n)
		{
			std::memcpy(res, buffer + (pointer - n), (pointer - n) * sizeof(T));
		}
		
		else if(pointer < n)
		{
			std::memcpy(res, buffer + size - (n - pointer), (n - pointer) * sizeof(T));
			std::memcpy(res + (n - pointer), buffer, pointer * sizeof(T));
		}

		if(old != nullptr)
			delete[] old;
		old = res;

		return res;
	}

	void Resize(size_t new_size)
	{
		std::lock_guard<std::mutex> lock(mutex);

		if(buffer != nullptr)
			delete[] buffer;

		assert(new_size > 0);

		size = new_size;
		pointer = 0;
		buffer = new T[size];
	}

	CCircularBuffer(size_t size = DEFAULT_BUFF_SIZE):
	buffer(nullptr),
	old(nullptr),
	size(0),
	pointer(0)
	{
		Resize(size);
	}

	~CCircularBuffer()
	{
		delete[] buffer;
		buffer = nullptr;

		if(old != nullptr)
			delete[] old;
		old = nullptr;

	}
};

#endif
