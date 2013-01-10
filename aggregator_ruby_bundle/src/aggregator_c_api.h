#ifndef AGGREGATOR_C_API_H
#define AGGREGATOR_C_API_H

#include <sys/types.h>
#include <stdint.h>
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//	bad code piece, duplicated from c++ part
//	ruby did not like that part...
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//

#define MAX_LEN 40


/**
	A base structure, that will be sent to upper level
	TODO learn if more info needed
*/
struct SPacket
{
	uint32_t address; // IPv4

	uint32_t agent_timestamp;
	uint32_t agent_usec;

	uint32_t server_timestamp;
	uint32_t server_usec;

	uint16_t sensor_id;
	uint8_t sensor_num;
	
	char data_string[MAX_LEN]; 
};

//
	
int InitAggregator(const char* address);
	
void InitAgent(int aggr_id, const char* address);

void Process(int aggr_id);
void BackgroundProcess(int agg_id);

void GlobalBlacklistAddress(int agg_id, const char* address);

void QueueBlacklistId(int agg_id, uint16_t id);
void QueueUnblacklistId(int agg_id, uint16_t id);

void BufferAllowId(int agg_id, uint16_t id);
void BufferDisallowId(int agg_id, uint16_t id);

void SetDeltaAS(int agg_id, const char* address, uint16_t sensor_id, double delta);
void SetDeltaS(int agg_id, uint16_t sensor_id, double delta);
void SetDeltaA(int agg_id, const char* address, double delta);
void SetDelta(int agg_id, double delta);

void SetIntervalAS(int agg_id, const char* address, uint16_t sensor_id, int max_interval);
void SetIntervalA(int agg_id, const char* address, int max_interval);
void SetIntervalS(int agg_id, uint16_t sensor_id, int max_interval);
void SetInterval(int agg_id, int max_interval);

struct SPacket* GetInterval(int aggr_id, const char* address, uint16_t id, uint16_t num, size_t from, size_t upto);

struct SPacket* GetAllData(int aggr_id, size_t* count);

#endif
