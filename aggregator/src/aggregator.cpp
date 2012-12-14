#include "aggregator.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <assert.h>

#include "nm_control.h"
#include "sensor_metainf.h"

void CBufferAggregator :: Add(uint32_t address, uint16_t sensor_id, SPacket p)
{
	auto iter = buffers.find(address);

	if(iter == buffers.end())
		buffers[address] = std::unordered_map<uint16_t, CCircularBuffer<SPacket>*> ();

	auto sub_map = buffers[address];
	auto iter2 = sub_map.find(sensor_id);

	if(iter2 == sub_map.end())
		sub_map[sensor_id] = new CCircularBuffer<SPacket>();

	sub_map[sensor_id]->Add(p);
}

//************************************************************************

void CAggregator :: Init(int port)
{
	address.sin_family = AF_INET;
    address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    bind(socket, (struct sockaddr *)&address, sizeof(address));
}

const size_t MAX_MESSAGE_SIZE = 4096; // TODO check, message size, passed in the message seems to be incorrret

void CAggregator :: Process()
{
	unsigned char buffer[MAX_MESSAGE_SIZE];

cout << "waiting for data" << endl;
 	int bytes_read = recv(socket, buffer, MAX_MESSAGE_SIZE, 0);//, (struct sockaddr*)&from, (socklen_t*)&rmt_length);

// read and convert header
	nm_data_hdr_t* header = reinterpret_cast<nm_data_hdr_t*> (buffer);

// if the address in a global blacklist - skip
	if(address_blacklist.IsIn(header->client_host.b4[0]))
		return;

	header->version = ntohs(header->version);
	header->flags   = ntohs(header->flags);
	header->ssrc    = ntohl(header->ssrc);
	header->seq_num = ntohl(header->seq_num);
	header->ts_m    = ntohl(header->ts_m);
	header->ts_sec  = ntohl(header->ts_sec);
	header->ts_usec = ntohl(header->ts_usec);
	
// read and convert 2nd header
	nm_strm_buf_t* data = reinterpret_cast<nm_strm_buf_t*> (buffer + sizeof(nm_data_hdr_t));

	data->period     = ntohs(data->period);
	data->flags_mask = ntohl(data->flags_mask);
	data->seq_num    = ntohl(data->seq_num);
	data->window     = ntohl(data->window);
	data->data_len   = ntohl(data->data_len);


// read sensor values and store them
	int sens_offset = sizeof(nm_data_hdr_t) + sizeof(nm_strm_buf_t);

	unsigned char* sens_data = buffer + sens_offset;

	for(int cnt = 0; ; )
	{
		assert(sens_offset + cnt + 2 < bytes_read);

		uint16_t size = ntohs(*((uint16_t*)(sens_data + cnt)));

		uint16_t id = ntohs(*((uint16_t*)(sens_data + cnt + 2)));
		
		if(id == 0) break; // it was last

// if the sensor in a global blacklist - skip
		if(!id_blacklist.IsIn(id)) 
		{
			SPacket packet;

			packet.address = header->client_host.b4[0];
			packet.agent_timestamp = header->ts_m * 1000000 + header->ts_sec;
			packet.agent_usec = header->ts_usec;
			packet.sensor_id = id;

			if(!queue_filter.FilterOut(packet, sens_data + cnt + 4))
			{
				queue.Add(packet);
			}

			buffer_aggregator.Add(packet.address, packet.sensor_id, packet);
		}

		cnt += size + 4;
	}
}


/*		cout << "header   : " << reinterpret_cast<char*> (header->signature) << endl;

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
		cout << endl;*/
/*		cout << "num     : " <<  int(data->num) << endl;
		cout << "period  : " <<  data->period << endl;
		cout << "flags   : " <<  data->flags_mask << endl;
		cout << "seq_num : " <<  data->seq_num << endl;
		cout << "window  : " <<  data->window << endl;
		cout << "len     : " <<  data->data_len << endl;
*/

/*			cout << "(" << size << ", " << type << ", " << 
				sensor_metainf[type].msg_length << ") = " << packet.data_string << endl;

*/
