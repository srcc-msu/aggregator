#ifndef AGGREGATOR_API_H
#define AGGREGATOR_API_H

#include <sys/types.h>

#include "packet.h"

extern "C" {

/**
	debug function
*/
int test(int port);
	
/**
	Inits agregator, deleting the old instance if it exisiting and 
	Wiping all data, blacklists and filter info
*/
void InitAggregator(int port);

/**
	Process one incoming packet by aggregator.
	Pretends to be thread safe. (orly? TODO check)
*/
void Process();

/**
	Add the \address to the global blacklist,
	IPv4 packed in uint32 currently.
	TODO: provide aliases for arrays/strings, if needed.
*/
void BlacklistAddress(uint32_t address);

/**
	Add the sensor \id to the global blacklist.
*/
void BlacklistId(uint16_t id);

/**
	Returns \seconds interval of data from circular buffer, corresponding to
	\address and \id.
*/
SPacket* GetInterval(uint32_t address, uint16_t id, size_t seconds);

/**
	Returns pointer to collected data. And they will not be availiable throught 
	this function, anymore.
	(!) When you call this 2nd time, data from previous call are deleted.
	(!) Also they will be deleted, when the librarys is unloaded.
	Returns array of \SPacket and writes the length into the \count.
*/
SPacket* GetAllData(size_t* count);
}

#endif
