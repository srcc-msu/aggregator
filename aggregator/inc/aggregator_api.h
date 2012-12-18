#ifndef AGGREGATOR_API_H
#define AGGREGATOR_API_H

#include <sys/socket.h>
#include <sys/types.h>

#include "packet.h"

extern "C" {

/**
	dummy function
*/
void __aggregator();
	
/**
	Inits a new agregator, returns the aggregator handle
	it is used in all functions to specify the aggregator
*/
int InitAggregator(const char* address);
	
/**
	Configures the agent on \addres to send data to this aggregator
*/
void InitAgent(int aggr_id, const char* address);

/**
	Process one incoming packet by aggregator.
	Pretends to be thread safe. (orly? TODO check)
*/
void Process(int aggr_id);

/**
	Add the \address to the global blacklist,
	IPv4 packed in uint32 currently.
	TODO: provide aliases for arrays/strings, if needed.
*/
void BlacklistAddress(int aggr_id, const char* address);

/**
	Add the sensor \id to the global blacklist.
*/
void BlacklistId(int aggr_id, uint16_t id);

/**
	Returns \seconds interval of data from circular buffer, corresponding to
	\address and \id.
*/
SPacket* GetInterval(int aggr_id, const char* address, uint16_t id, size_t seconds);

/**
	Returns pointer to collected data. And they will not be availiable throught 
	this function, anymore.
	(!) When you call this 2nd time, data from previous call are deleted.
	(!) Also they will be deleted, when the librarys is unloaded.
	Returns array of \SPacket and writes the length into the \count.
*/
SPacket* GetAllData(int aggr_id, size_t* count);
}

#endif
