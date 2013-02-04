#ifndef FWRAP_H
#define FWRAP_H

#include <vector>
#include <limits.h>
#include <mutex>
#include <thread>

using namespace std;

/**
	provide buffering, name rotating and writing on timeout
	writing to disk is asynchronous, only one file(across all instances) 
	will be written on disk at any moment
*/
template<typename T>
class CFWrap
{
private:
/// only one instance of this class will write to disk at single moment	
	static mutex global_write_mutex; 

/// full directory for file
    string dir; 

    shared_ptr<vector<T>> queue;

/// count of lines, that will be stored in memory before writing to the file
    size_t max_lines;

/// max time difference for all packets in queue
    unsigned int max_time;

/**
	create new queue and reserve required capacity
*/
    void ReinitQueue();

/**
	service function to be launched in separate thread, threadsafe
*/
	void WriteHelper(shared_ptr<vector<T>> queue_dump);


public:
/**
	reinit queue, create new thread, that will write current queue to a disk
*/
	void Write();

/**
	adds \packet to the queue, check if need to write the queue to a disk
*/
	void AddPacket(const T& packet);

    CFWrap(string _dir, int _max_lines, int _max_time, FILE* _f = nullptr):
    dir(_dir),
    max_lines(_max_lines),
    max_time(_max_time)
    {
		ReinitQueue();
    }
};

#endif
