#include "aggregator.h"

#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>

#include "nm_control.h"
#include "sensor_metainf.h"

#include "debug.h"
#include "error.h"

const size_t MAX_MESSAGE_SIZE = 4096; // TODO check

void CAggregator :: AccumulateStat(int res)
{
	if(res > 0)
		stat_added ++;

	switch(res)
	{
		case 0: stat_filtered_out    ++; break;

		case 1: stat_allow_time      ++; break;
		case 2: stat_allow_delta     ++; break;
		case 3: stat_allow_abs_delta ++; break;

		default: throw CException("unknown packet filtering result");
	}
}

//--------------------------------

void CAggregator :: ProcessChunk(const SDecodedPacket& raw_packet
	, uint16_t sensor_id, size_t count, size_t val_size
	, unsigned char* sens_data
	, vector<SPacket>& packets_buffer)
{
	bool any_changed = false;

	if(!queue_aggregator.IsAllowed(sensor_id))
	{
		stat_filtered_blacklist += count;
		return;
	}

	SPacket packet;

// few values are packed into one message, make separate packet for each
	for(size_t sensor_num = 0; sensor_num < count; sensor_num++)
	{
// \sensor_num in packet starting from 1, 0 = average across all
		CreatePacket(packet, sens_data + sensor_num * val_size
			, sensor_id, sensor_num + 1
			, UIP(raw_packet.header->client_host.b4[0])
			, raw_packet.current_time.tv_sec, raw_packet.current_time.tv_usec
			, raw_packet.header->ts_m * 1000000 + raw_packet.header->ts_sec
			, raw_packet.header->ts_usec);

// check if this one is filtered/blacklisted/etc
		int res = queue_aggregator.Check(packet);

DMSG2("reason " << res);

		if(res > 0) // it is allowed
		{
			any_changed = true;
			packets_buffer.push_back(packet);
		}

		AccumulateStat(res);
	}

// add average
	if(any_changed && queue_aggregator.IsAverageId(sensor_id))
	{
		SPacket average_packet = packet;

		average_packet.value = queue_aggregator.CalcAverage
			(average_packet.address.b4[0], average_packet.sensor_id, count);
		average_packet.sensor_num = 0;

		if(queue_aggregator.Check(average_packet) > 0)
			packets_buffer.push_back(average_packet);
	}
}

//--------------------------------

nm_data_hdr_t* MapHeaderNtoH(unsigned char* buffer)
{
	nm_data_hdr_t* header = reinterpret_cast<nm_data_hdr_t*> (buffer);

	header->version = ntohs(header->version);
	header->flags   = ntohs(header->flags);
	header->ssrc    = ntohl(header->ssrc);
	header->seq_num = ntohl(header->seq_num);
	header->ts_m    = ntohl(header->ts_m);
	header->ts_sec  = ntohl(header->ts_sec);
	header->ts_usec = ntohl(header->ts_usec);

	return header;
}

//--------------------------------

const int ESTIMATED_SENSORS = 256;

void CAggregator :: ProcessPacket(const SDecodedPacket& raw_packet)
{
	vector<SPacket> packets_buffer;
	packets_buffer.reserve(ESTIMATED_SENSORS);

// loop for every sensor_id in this packet
	for(int cnt = 0; ; )
	{
		unsigned char* current = raw_packet.sens_data + cnt;

// wrong packet - something changed?
		if(current > raw_packet.sens_data + raw_packet.data_size)
			throw CException("error during parsing a recieved packet");

		uint16_t size = ntohs(*((uint16_t*)(current)));
		uint16_t sensor_id = ntohs(*((uint16_t*)(current + 2)));

DMSG2("size: " << size << "; id " << sensor_id << "; offset: " << cnt);

		if(sensor_id == 0)
		{
DMSG2("finished, cnt: " << cnt);

			break; // it was last
		}

		size_t val_size = sensor_metainf[sensor_id].msg_length;
		size_t count = size / val_size;

// add all sensors from this chunk to \packets_buffer
		ProcessChunk(raw_packet, sensor_id, count, val_size
			, current + 4, packets_buffer);

		cnt += size + 4;
	}

	queue_aggregator.UncheckedAdd(packets_buffer);
}

//--------------------------------

void CAggregator :: Process()
{
	unsigned char buffer[MAX_MESSAGE_SIZE];

	size_t bytes_read = connection.GetData(buffer, MAX_MESSAGE_SIZE);

DMSG1("recieved " << bytes_read);

// if it is equal, probably message was tranucated and is wrong
	if(bytes_read >= MAX_MESSAGE_SIZE)
		throw CException("mesage size exceeds max size");

	SDecodedPacket raw_packet(buffer, bytes_read);

	//	we got zero length or wrong messages - skip it
	if(raw_packet.bytes_read < sizeof(nm_data_hdr_t))
	{
DMSG1("message ignored, too short");
		return;
	}

	if(raw_packet.header->strm_num != 1) // some unknown for now message
	{
DMSG1("message ignored, unknown stream");
		return;
	}

	ProcessPacket(raw_packet);
}

//--------------------------------

void CAggregator :: Stat(int sleep_time)
{
	printf("\n --- aggregator stat ---\n");

	printf("blacklisted  : %7ld / sec\n", stat_filtered_blacklist 
		/ sleep_time);
	printf("filtered out : %7ld / sec\n", stat_filtered_out / sleep_time);
	printf("added        : %7ld / sec\n", stat_added / sleep_time);
	printf("   time      : %7ld / sec\n", stat_allow_time / sleep_time);
	printf("   delta     : %7ld / sec\n", stat_allow_delta / sleep_time);
	printf("   abs delta : %7ld / sec\n", stat_allow_abs_delta / sleep_time);

	printf(" -----------------------\n\n");

	stat_filtered_blacklist = 0;
	stat_filtered_out = 0;
	stat_allow_time = 0;
	stat_allow_delta = 0;
	stat_allow_abs_delta = 0;
	stat_added = 0;
}

//--------------------------------

void CAggregator :: AgentsStat(int sleep_time)
{
	printf("\n --- agents stat for %3d seconds --- \n", sleep_time);

	connection.PokeAgents();
	connection.AgentStats(sleep_time);

	printf(" ----------------------------------- \n\n");
}

//--------------------------------

void CAggregator :: BackgroundAgentsStat(int sleep_time)
{
	static bool started = false;

	if(started == true)
		return;

    std::thread t(&CAggregator :: BackgroundAgentHelper, this, sleep_time);
    t.detach();
}

//--------------------------------

void CAggregator :: BackgroundStat(int sleep_time)
{
	static bool started = false;

	if(started == true)
		return;

    std::thread t(&CAggregator :: BackgroundStatHelper, this, sleep_time);
    t.detach();
}

//--------------------------------

void CAggregator :: BackgroundStatHelper(int sleep_time)
{
    while(1)
    {
		sleep(sleep_time);
		Stat(sleep_time);
    }
}

//--------------------------------

void CAggregator :: BackgroundAgentHelper(int sleep_time)
{
    while(1)
    {
		sleep(sleep_time);
		AgentsStat(sleep_time);
    }
}
