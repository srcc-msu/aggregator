#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>

#include <mutex>
#include <condition_variable>

static const unsigned int MIN_SLEEP_TIME = 8;
static const unsigned int MAX_SLEEP_TIME = 1024*1204;

/**
	provides ability to sleep smlower or faster, basing on predicat
*/
class CDynSleeper
{
private:
	unsigned int min_sleep_time;
	unsigned int max_sleep_time;

	unsigned int sleep_time;

public:

/**
	get current sleep time
*/
	unsigned int GetTime() const
	{
		return sleep_time;
	}

/**
	sleep
	if \sleep_more is true - sleep more than next time, other way - less
*/
	void Sleep(bool sleep_more)
	{
		if(sleep_more && sleep_time < max_sleep_time)
			sleep_time *= 2;
		else if(sleep_time > min_sleep_time)
			sleep_time /= 2;

		usleep(sleep_time);
	}

	CDynSleeper(int _min_sleep_time = MIN_SLEEP_TIME
		, int _max_sleep_time = MAX_SLEEP_TIME):
		min_sleep_time(_min_sleep_time),
		max_sleep_time(_max_sleep_time),
		sleep_time(_min_sleep_time)
	{}
};

class CSharedLock
{
	std::condition_variable unlocked;
	std::mutex mutex;

	bool writer;
	int readers;

public:
	void ReadLock()
	{
		std::unique_lock<std::mutex> lock(mutex);

		while (writer)
		   unlocked.wait(lock);

		readers++;
	}

	void ReadUnlock()
	{
		std::lock_guard<std::mutex> lock(mutex);

		readers--;

		if (readers == 0)
			unlocked.notify_all();
	}

	void WriteLock()
	{
		std::unique_lock<std::mutex> lock(mutex);

		while (writer || (readers > 0))
			unlocked.wait(lock);

		writer = true;
	}

	void WriteUnlock()
	{
		std::lock_guard<std::mutex> lock(mutex);

		writer = false;

		unlocked.notify_all();
	}

	CSharedLock():
		writer(false),
		readers(0)
	{}
};

#endif
