#include "aggregator.h"

#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <assert.h>

#include "nm_control.h"
#include "sensor_metainf.h"

#include "debug.h"

#include <iostream>

using std::cerr;
using std::endl;

const size_t MAX_MESSAGE_SIZE = 4096; // TODO check

void CAggregator :: Process()
{
	unsigned char buffer[MAX_MESSAGE_SIZE];

//	read sensor values and store them
	size_t sens_offset = sizeof(nm_data_hdr_t);
	unsigned char* sens_data = buffer + sens_offset; 

//	TODO add timeout to socket
	size_t bytes_read = connection.GetData(buffer, MAX_MESSAGE_SIZE);

	assert(bytes_read < MAX_MESSAGE_SIZE);

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
	
	DBLOCK2(
	{
		fprintf(stderr,  "off1 " << sizeof(nm_data_hdr_t) );

	    for(size_t i = 0; i < bytes_read; i++)
		{	
	    	if(i % 16 == 0 && i > 0) 
	    		printf ("\n");

	    	printf("%2x", buffer[i]);
	    }

	    printf("\n");

   		fprintf(stderr,  "header   : " << reinterpret_cast<char*> (header->signature) );

		fprintf(stderr,  "version  : " <<  header->version );
		fprintf(stderr,  "msg_type : " <<  int(header->msg_type) );
		fprintf(stderr,  "str_num  : " <<  int(header->strm_num) );
		fprintf(stderr,  "flags    : " <<  header->flags );

		fprintf(stderr,  "min  : " <<  header->ts_m );
		fprintf(stderr,  "sec  : " <<  header->ts_sec );
		fprintf(stderr,  "usec : " <<  header->ts_usec );

		for(int i = 0; i < 4; i++)
		{
			fprintf(stderr,  int(header->client_host.b1[i]) << " ";
		}

		cerr );
	})


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
	static SPacket packets_buffer[MAX_MESSAGE_SIZE];

	for(int cnt = 0; ; )
	{
		assert(sens_offset + cnt + 4 < bytes_read); // wrong packet - something changed?

		uint16_t size = ntohs(*((uint16_t*)(sens_data + cnt)));

		uint16_t id = ntohs(*((uint16_t*)(sens_data + cnt + 2)));
		
		DMSG2(size << " " << id);

		if(id == 0) break; // it was last

		size_t val_size = sensor_metainf[id].msg_length;

		if(sensor_metainf[id].type != BINARY) // TODO check
		{
			int any_changed = -1;

//	few values are packed into one message, make separate packet for each
			for(size_t part = 0; part < size/val_size; part++)
			{
				SPacket packet;

				packet.address.b4[0] = header->client_host.b4[0];
				packet.agent_timestamp = header->ts_m * 1000000 + header->ts_sec;
				packet.agent_usec = header->ts_usec;
				packet.server_timestamp = current_time.tv_sec;
				packet.server_usec = current_time.tv_usec;
				packet.sensor_id = id;
				packet.sensor_num = part+1;

				SPacketExt ext_packet(packet, sens_data + cnt + 4 + val_size * part);

				int res = queue_aggregator.Check(ext_packet);
				DMSG2("res for " << packet.sensor_id << "." << int(packet.sensor_num) << " is: " << res);

				DBLOCK1(
				{
					if(res > 0)
						DMSG1("res for allowing " << packet.sensor_id << "." << int(packet.sensor_num) << " is: " << res);
				})

				if(res > 0)
				{
					ext_packet.WriteValueToPacket();
					packets_buffer[packets_count] = ext_packet.packet;
					stat_added ++;
					any_changed = packets_count;
					packets_count++;
				}

				if(res == 0)
					stat_filtered_out ++;
				else if(res == 1)
					stat_allow_time ++;
				else if(res == 2)
					stat_allow_delta ++;
				else if(res == 3)
					stat_allow_abs_delta ++;
				else if(res == -1)
				{
					stat_filtered_blacklist += size/val_size;
					break;
				}

				buffer_aggregator.Add(ext_packet);
			}

//	add average
			if(any_changed != -1 && queue_aggregator.IsAverageId(id))
			{
				SPacket packet = packets_buffer[any_changed];
				UValue value = queue_aggregator.CalcAverage(packet.address.b4[0], packet.sensor_id, size/val_size);

				packet.sensor_num = 0;
				packets_buffer[packets_count++] = SPacketExt(packet, value).packet;
			}
		}

		cnt += size + 4;
	}

	queue_aggregator.UncheckedAdd(packets_buffer, packets_count);
}

void CAggregator :: Stat()
{
	printf("\n --- aggregator stat ---\n");

	printf("blacklisted  : %ld\n", stat_filtered_blacklist);
	printf("filtered out : %ld\n", stat_filtered_out);
	printf("added        : %ld\n", stat_added);
	printf("\ttime      : %ld\n", stat_allow_time);
	printf("\tdelta     : %ld\n", stat_allow_delta);
	printf("\tabs delta : %ld\n", stat_allow_abs_delta);

	printf(" ---------------------\n\n");

	stat_filtered_blacklist = 0;
	stat_filtered_out = 0;
	stat_allow_time = 0;
	stat_allow_delta = 0;
	stat_allow_abs_delta = 0;
	stat_added = 0;
}

void CAggregator :: AgentsStat()
{
	printf("\n --- agents stat --- \n");

	connection.AgentStats();
	connection.PokeAgents();

	printf(" ------------------- \n\n");
}

void CAggregator :: BackgroundAgentsStat(int sleep_time)
{
    std::thread t(BackgroundAgentHelper, this, sleep_time);
    t.detach();
}

void CAggregator :: BackgroundStat(int sleep_time)
{
    std::thread t(BackgroundStatHelper, this, sleep_time);
    t.detach();
}

void BackgroundStatHelper(CAggregator* agg, int sleep_time)
{
    while(1)
    {
		sleep(sleep_time);
		agg->Stat();
    }
}

void BackgroundAgentHelper(CAggregator* agg, int sleep_time)
{
    while(1)
    {
		sleep(sleep_time);
		agg->AgentsStat();
    }
}
