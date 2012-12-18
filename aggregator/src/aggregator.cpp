#include "aggregator.h"

#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <assert.h>

#include "nm_control.h"
#include "sensor_metainf.h"

#include "debug.h"

void CBufferAggregator :: Add(uint32_t address, uint16_t sensor_id, SPacket p)
{
	if(buffers[address].count(sensor_id) == 0)
		buffers[address][sensor_id] = new CCircularBuffer<SPacket>();

	buffers[address][sensor_id]->Add(p);

	DMSG2("added to circ. buffer, size is now " << buffers.size() << " and " << buffers[address].size());
}

//************************************************************************

const size_t MAX_MESSAGE_SIZE = 4096; // TODO check, message size, passed in the message seems to be incorrret

void CAggregator :: Process()
{
	unsigned char buffer[MAX_MESSAGE_SIZE];

	DMSG1("waiting for data");

	int bytes_read = connection.GetData(buffer, MAX_MESSAGE_SIZE);

	DMSG1("recieved " << bytes_read);

//	we got zero length message - filtered out
//	TODO add timeoutto socket
	if(bytes_read == 0)
		return;

//	read and convert header
	nm_data_hdr_t* header = reinterpret_cast<nm_data_hdr_t*> (buffer);

	header->version = ntohs(header->version);
	header->flags   = ntohs(header->flags);
	header->ssrc    = ntohl(header->ssrc);
	header->seq_num = ntohl(header->seq_num);
	header->ts_m    = ntohl(header->ts_m);
	header->ts_sec  = ntohl(header->ts_sec);
	header->ts_usec = ntohl(header->ts_usec);
	
//	read and convert 2nd header
	nm_strm_buf_t* data = reinterpret_cast<nm_strm_buf_t*> (buffer + sizeof(nm_data_hdr_t));

	data->period     = ntohs(data->period);
	data->flags_mask = ntohl(data->flags_mask);
	data->seq_num    = ntohl(data->seq_num);
	data->window     = ntohl(data->window);
	data->data_len   = ntohl(data->data_len);


//	read sensor values and store them
	int sens_offset = sizeof(nm_data_hdr_t) + sizeof(nm_strm_buf_t); // WTF?? it is wrong

	sens_offset = 54 + 20; // WTF?

	unsigned char* sens_data = buffer + sens_offset; 

	DBLOCK2(
	{
		cout << "off1 " << sizeof(nm_data_hdr_t) << " off2 " << sizeof(nm_strm_buf_t) << endl;

	    for(int i = 0; i < bytes_read; i++)
		{	
	    	if(i % 16 == 0 && i > 0) 
	    		printf ("\n");

	    	printf("%2x", buffer[i]);
	    }

	    printf("\n");

   		cout << "header   : " << reinterpret_cast<char*> (header->signature) << endl;

		cout << "version  : " <<  header->version << endl;
		cout << "msg_type : " <<  int(header->msg_type) << endl;
		cout << "str_num  : " <<  int(header->strm_num) << endl;
		cout << "flags    : " <<  header->flags << endl;

		cout << "min  : " <<  header->ts_m << endl;
		cout << "sec  : " <<  header->ts_sec << endl;
		cout << "usec : " <<  header->ts_usec << endl;

		for(int i = 0; i < 4; i++)
		{
			cout << int(header->client_host.b4[i]) << " ";
		}
		cout << endl;

		cout << "num     : " <<  int(data->num) << endl;
		cout << "period  : " <<  data->period << endl;
		cout << "flags   : " <<  data->flags_mask << endl;
		cout << "seq_num : " <<  data->seq_num << endl;
		cout << "window  : " <<  data->window << endl;
		cout << "len     : " <<  data->data_len << endl;
	})


//	get current server time, that wil be written to all packets
	timeval current_time;
	gettimeofday(&current_time, NULL);

	for(int cnt = 0; ; )
	{
		assert(sens_offset + cnt + 2 < bytes_read);

		uint16_t size = ntohs(*((uint16_t*)(sens_data + cnt)));

		uint16_t id = ntohs(*((uint16_t*)(sens_data + cnt + 2)));
		
		DMSG2(size << " " << id);

		if(id == 0) break; // it was last

//	if the sensor in a global blacklist - skip
		if(!id_blacklist.IsIn(id)) 
		{
			SPacket packet;

			packet.address = header->client_host.b4[0];
			packet.agent_timestamp = header->ts_m * 1000000 + header->ts_sec;
			packet.agent_usec = header->ts_usec;
			packet.server_timestamp = current_time.tv_sec;
			packet.server_usec = current_time.tv_usec;
			packet.sensor_id = id;

			if(!queue_filter.FilterOut(packet, sens_data + cnt + 4))
			{
				queue.Add(packet);
				DMSG2(id << " packet added");
			}
			
			buffer_aggregator.Add(packet.address, packet.sensor_id, packet);
		}
		DBLOCK2(
			else DMSG2(id << " is in blacklist");
		)

		cnt += size + 4;
	}
}

/*			cout << "(" << size << ", " << type << ", " << 
				sensor_metainf[type].msg_length << ") = " << packet.data_string << endl;

				cout << sizeof(nm_data_hdr_t) << " off2 " << sizeof(nm_strm_buf_t) << endl;

        for(int i = 0; i < sens_offset; i++)
                printf("%x", buffer[i]);
        printf("\n");

        for(int i = sens_offset; i < 140; i++)
                printf("%x", buffer[i]);

        printf("\n");


*/
