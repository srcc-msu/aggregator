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

void CAggregator :: ProcessSensor(unsigned int sensor_id, nm_data_hdr_t* header, unsigned char* sens_data,
	size_t val_size, size_t count, const timeval& current_time, SPacket* packets_buffer, int& packets_count)
{
	int any_changed = -1;

	bool buffer_allow = buffer_aggregator.IsAllowed(sensor_id);
	bool queue_allow = queue_aggregator.IsAllowed(sensor_id);

	if(!queue_allow)
		stat_filtered_blacklist += count;

	if(!buffer_allow && !queue_allow)
		return;

	SPacket packet;

// next fileds are the same for all this packets
	packet.address.b4[0] = header->client_host.b4[0];
	packet.agent_timestamp = header->ts_m * 1000000 + header->ts_sec;
	packet.agent_usec = header->ts_usec;
	packet.server_timestamp = current_time.tv_sec;
	packet.server_usec = current_time.tv_usec;
	packet.sensor_id = sensor_id;

//	few values are packed into one message, make separate packet for each
	for(size_t sensor_num = 0; sensor_num < count; sensor_num++)
	{
		packet.sensor_num = sensor_num+1;

// copy packet and create ext_packet, basing on it
		SPacketExt ext_packet(packet, sens_data + val_size * sensor_num);

		if(queue_allow)
		{
// check if this one is filtered/blacklisted/etc
			int res = queue_aggregator.Check(ext_packet);

			if(res > 0) // it is allowed
			{
// fill the \data_string filed in it
				ext_packet.WriteValueToPacket();

// add speed to the packet, TODO: may be slow
				queue_aggregator.AddSpeed(ext_packet);
				
// and store for addition			
				packets_buffer[packets_count] = ext_packet.packet;
				any_changed = packets_count;
				packets_count++;
			}

			AccumulateStat(res);
		}
		
		if(buffer_allow)	
			buffer_aggregator.Add(ext_packet);
	}

//	add average
	if(any_changed != -1 && queue_aggregator.IsAverageId(sensor_id))
	{
		SPacket packet = packets_buffer[any_changed];
		
		UValue value = queue_aggregator.CalcAverage(packet.address.b4[0], packet.sensor_id, count);

		packet.sensor_num = 0;
		packets_buffer[packets_count++] = SPacketExt(packet, value).packet;
	}
}

void CAggregator :: Process()
{
	unsigned char buffer[MAX_MESSAGE_SIZE];

//	read sensor values and store them
	size_t sens_offset = sizeof(nm_data_hdr_t);
	unsigned char* sens_data = buffer + sens_offset;

//	TODO add timeout to socket
	size_t bytes_read = connection.GetData(buffer, MAX_MESSAGE_SIZE);

// if it is equal, probably message was tranucated and is wrong
	if(bytes_read >= MAX_MESSAGE_SIZE)
		throw CException("mesage size exceeds max size");

	DMSG1("recieved " << bytes_read);

//	we got zero length or wrong messages - skip it
	if(bytes_read < sens_offset)
	{
		DMSG1("message ignored, too short");
		return;
	}

//	read and convert header
	nm_data_hdr_t* header = reinterpret_cast<nm_data_hdr_t*> (buffer);

	header->version = ntohs(header->version);
	header->flags   = ntohs(header->flags);
	header->ssrc    = ntohl(header->ssrc);
	header->seq_num = ntohl(header->seq_num);
	header->ts_m    = ntohl(header->ts_m);
	header->ts_sec  = ntohl(header->ts_sec);
	header->ts_usec = ntohl(header->ts_usec);
	
	if(header->strm_num != 1) // some unknown for now message
	{
		DMSG1("message ignored, unknown stream");
		return;
	}

//	get current server time, that wil be written to all packets
	timeval current_time;
	gettimeofday(&current_time, NULL);

	int packets_count = 0;

// estimated messages count, will be definilty less than MAX_MESSAGE_SIZE
// ... /facepalm
	static SPacket packets_buffer[MAX_MESSAGE_SIZE];

	for(int cnt = 0; ; )
	{
		if(sens_offset + cnt + 4 > bytes_read) // wrong packet - something changed?
			throw CException("error during parsing a recieved packet");

		uint16_t size = ntohs(*((uint16_t*)(sens_data + cnt)));

		uint16_t sensor_id = ntohs(*((uint16_t*)(sens_data + cnt + 2)));
		
		DMSG2(size << " " << sensor_id);

		if(sensor_id == 0) break; // it was last

		size_t val_size = sensor_metainf[sensor_id].msg_length;

		if(sensor_metainf[sensor_id].type != BINARY) // TODO check
		{
			ProcessSensor(sensor_id, header, sens_data + cnt + 4, val_size, size,
				current_time, packets_buffer, packets_count);
		}

		cnt += size + 4;
	}

	queue_aggregator.UncheckedAdd(packets_buffer, packets_count);
}

//--------------------------------

void CAggregator :: Stat()
{
	printf("\n --- aggregator stat ---\n");

	printf("blacklisted  : %ld\n", stat_filtered_blacklist);
	printf("filtered out : %ld\n", stat_filtered_out);
	printf("added        : %ld\n", stat_added);
	printf("    time      : %ld\n", stat_allow_time);
	printf("    delta     : %ld\n", stat_allow_delta);
	printf("    abs delta : %ld\n", stat_allow_abs_delta);

	printf(" ---------------------\n\n");

	stat_filtered_blacklist = 0;
	stat_filtered_out = 0;
	stat_allow_time = 0;
	stat_allow_delta = 0;
	stat_allow_abs_delta = 0;
	stat_added = 0;
}

//--------------------------------

void CAggregator :: AgentsStat()
{
	printf("\n --- agents stat --- \n");

	connection.AgentStats();
	connection.PokeAgents();

	printf(" ------------------- \n\n");
}

//--------------------------------

void CAggregator :: BackgroundAgentsStat(int sleep_time)
{
    std::thread t(BackgroundAgentHelper, this, sleep_time);
    t.detach();
}

//--------------------------------

void CAggregator :: BackgroundStat(int sleep_time)
{
    std::thread t(BackgroundStatHelper, this, sleep_time);
    t.detach();
}

//--------------------------------

void BackgroundStatHelper(CAggregator* agg, int sleep_time)
{
    while(1)
    {
		sleep(sleep_time);
		agg->Stat();
    }
}

//--------------------------------

void BackgroundAgentHelper(CAggregator* agg, int sleep_time)
{
    while(1)
    {
		sleep(sleep_time);
		agg->AgentsStat();
    }
}
