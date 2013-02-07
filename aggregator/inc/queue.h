#ifndef QUEUE_H
#define QUEUE_H

#include <mutex>

#include <cstring>

#include "debug.h"
#include "error.h"

const size_t MAX_QUEUE_SIZE = 16 * 1024;

/**
	Thread safe sensor queue. (ORLY?)
	accumulates single values and return them all by request.
*/
template<typename T>
class CSensorQueue
{
private:
	std::mutex mutex;

	T* queue;
	size_t pointer;

/**
	creates new queuq, return old
*/
	T* Reinit() // TODO renaem something
	{
		T* old = queue;
		queue = new T[MAX_QUEUE_SIZE];

		pointer = 0;

		return old;
	}

public:
/**
	Add single value to the queue.
	bad function, using mutex on every addition is not efficient
*/
	void Add(T value)
	{
		std::lock_guard<std::mutex> lock(mutex);

		if(pointer == MAX_QUEUE_SIZE)
			Reinit();

		queue[pointer] = value;
		pointer++;
	}

/**
	Add few values to the queue.
*/
	void Add(T* values, size_t count)
	{
		std::lock_guard<std::mutex> lock(mutex);

		if(count > MAX_QUEUE_SIZE)
			count = count % MAX_QUEUE_SIZE;

		if(pointer == MAX_QUEUE_SIZE || pointer + count >= MAX_QUEUE_SIZE)
			Reinit();

		memcpy(queue + pointer, values, count * sizeof(T));

		pointer += count;
	}

/**
	Get all data from the queue.
	Current pointer will be returned and new one will be allocated -
	old one will be purged on next request. Also it can be purged manually.
*/
	T* GetAll(size_t* count)
	{
		std::lock_guard<std::mutex> lock(mutex);
		
		if(count)
			*count = pointer;

		return Reinit();
	}

	CSensorQueue():
	queue(nullptr),
	pointer(0)
	{
		Reinit();
	}

	~CSensorQueue()
	{}
};


const size_t DEFAULT_BUFF_SIZE = 300;

/**
	Thread safe circular buffer.
	Accumulates single values, rotating them over time.
	Returns data from requested interval.
*/
template<typename T>
class CCircularBuffer
{
private:
	mutable std::mutex mutex;
	T* buffer;
	
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

/**
	Gets n values from the buffer, n = \from - \upto. If thy were not added yet -
	returns default values from \T constructor.
*/
	T* Get(size_t from, size_t upto, size_t* count)
	{
        DMSG1(from << " <> " << upto << " requested. current pointer is " << pointer);

		if(from <= upto || upto > size)
		{
			*count = 0;
			return nullptr;
		}

		if(from > size)
			from = size;

		*count = from - upto;

		T* res = new T[*count];

		std::lock_guard<std::mutex> lock(mutex);

		int start = pointer - from;
		int end = pointer - upto;

		if(start >= 0)
		{
			std::memcpy(res, buffer + start, (end - start) * sizeof(T));
		}

		else if(start < 0 && end > 0)
		{
			std::memcpy(res, buffer + size + start, (-start) * sizeof(T));
			std::memcpy(res + (-start), buffer, end * sizeof(T));
		}
		
		else if(end <= 0)
		{
			std::memcpy(res, buffer + size + start, (end-start) * sizeof(T));
		}

		return res;
	}

	void Resize(size_t new_size)
	{
		std::lock_guard<std::mutex> lock(mutex);

		if(buffer != nullptr)
			delete[] buffer;

		if(new_size == 0)
			throw CException("can not init circualr buffer with 0 size");

		size = new_size;
		pointer = 0;
		buffer = new T[size];

		for(size_t i = 0; i < size; i++)
			buffer[i] = T();
	}

	CCircularBuffer(size_t size = DEFAULT_BUFF_SIZE):
	buffer(nullptr),
	size(0),
	pointer(0)
	{
		Resize(size);
	}

	~CCircularBuffer()
	{
		delete[] buffer;
		buffer = nullptr;
	}
};

#endif
