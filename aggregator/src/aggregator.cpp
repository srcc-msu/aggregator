#include "aggregator.h"

#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <assert.h>

#include "nm_control.h"
#include "sensor_metainf.h"

#include "debug.h"

#include <iostream>
using std :: cerr;
using std :: endl;

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
		cerr << "off1 " << sizeof(nm_data_hdr_t) << endl;

	    for(size_t i = 0; i < bytes_read; i++)
		{	
	    	if(i % 16 == 0 && i > 0) 
	    		printf ("\n");

	    	printf("%2x", buffer[i]);
	    }

	    printf("\n");

   		cerr << "header   : " << reinterpret_cast<char*> (header->signature) << endl;

		cerr << "version  : " <<  header->version << endl;
		cerr << "msg_type : " <<  int(header->msg_type) << endl;
		cerr << "str_num  : " <<  int(header->strm_num) << endl;
		cerr << "flags    : " <<  header->flags << endl;

		cerr << "min  : " <<  header->ts_m << endl;
		cerr << "sec  : " <<  header->ts_sec << endl;
		cerr << "usec : " <<  header->ts_usec << endl;

		for(int i = 0; i < 4; i++)
		{
			cerr << int(header->client_host.b1[i]) << " ";
		}

		cerr << endl;
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

//	few values are packed into one message, make separate packet for each
		if(sensor_metainf[id].type != BINARY) // TODO check
		{
			for(size_t part = 0; part < size/val_size; part++)
			{
				SPacket packet;

				packet.address.b4[0] = header->client_host.b4[0];
				packet.agent_timestamp = header->ts_m * 1000000 + header->ts_sec;
				packet.agent_usec = header->ts_usec;
				packet.server_timestamp = current_time.tv_sec;
				packet.server_usec = current_time.tv_usec;
				packet.sensor_id = id;
				packet.sensor_num = part;

				SPacketExt ext_packet(packet, sens_data + cnt + 4 + val_size * part);

				if(queue_aggregator.Check(ext_packet))
					packets_buffer[packets_count++] = ext_packet.packet;

				buffer_aggregator.Add(ext_packet);
			}
		}

		cnt += size + 4;
	}

	queue_aggregator.UncheckedAdd(packets_buffer, packets_count);
}
