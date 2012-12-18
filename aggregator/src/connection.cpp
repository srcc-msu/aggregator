#include "connection.h"

#include <string.h>

#include <stdio.h>
#include <iostream>

using namespace std;

void CConnectionManager :: Init()
{
	data_socket_inf.sin_family = AF_INET;
    data_socket_inf.sin_port = htons(0);
	data_socket_inf.sin_addr.s_addr = htonl(INADDR_ANY);

	data_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    bind(data_socket, (sockaddr *)&data_socket_inf, sizeof(data_socket_inf));

//	learn what port did we get from OS
    sockaddr_in info;
	socklen_t info_len = sizeof(info);    

    getsockname(data_socket, (sockaddr *)&info, &info_len);
    aggr_port = info.sin_port;
}

unsigned char* write_str(unsigned char* dst, unsigned char* str, int count = 0)
{
	int l = (count == 0) ? strlen((const char*)str) : count;
	memcpy(dst, str, l);
	return dst + l;
}

int CConnectionManager :: FormV1RedirectMessage(unsigned char* msg, uint32_t address) const
{
	unsigned char* ptr = write_str(msg, (unsigned char*)"MMCS.ACF\x00\x01\x00\x00", 12);
	
	uint32_t zero = 0;

	ptr = write_str(ptr, (unsigned char*) &address, 4); // pretend address

	for(int i = 0; i < 4; i++) ptr = write_str(ptr, (unsigned char*) &zero, 3); // address field filler

	ptr = write_str(ptr, (unsigned char*) &aggr_address, 4); // agrhost
	for(int i = 0; i < 4; i++) ptr = write_str(ptr, (unsigned char*) &zero, 3); // address field filler

	ptr = write_str(ptr, (unsigned char*) &aggr_port, 2); // agrport
	ptr = write_str(ptr, (unsigned char*) &zero, 4); // cookie

	return ptr - msg;
}

void CConnectionManager :: InitAgent(uint32_t address)
{
//	create sending socket
	sockaddr_in ctl_socket_inf;
	int ctl_socket;

	ctl_socket_inf.sin_family = AF_INET;
    ctl_socket_inf.sin_port = htons(0);
	ctl_socket_inf.sin_addr.s_addr = htonl(INADDR_ANY);

	ctl_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    bind(ctl_socket, (struct sockaddr *)&ctl_socket_inf, sizeof(ctl_socket_inf));

//	create destionation address
	sockaddr_in agent_address;
	agent_address.sin_family = AF_INET;
    agent_address.sin_port = htons(control_port);
	agent_address.sin_addr.s_addr = address;

//	register agent
	agents.insert(agent_address.sin_addr.s_addr);

//	form and send V1 control message

	unsigned char msg[1024] = "";
	
	int len = FormV1RedirectMessage(msg, address);
  	
  	sendto(ctl_socket, msg, len, 0, (sockaddr*) &agent_address, sizeof(agent_address));

  	close(ctl_socket);
}


int CConnectionManager :: GetData(unsigned char* data, int max_count)
{
    sockaddr_in info;
	socklen_t info_len = sizeof(info);    

 	int bytes_read = recvfrom(data_socket, data, max_count, 0, (struct sockaddr*)&info, (socklen_t*)&info_len);

// if the address in a global blacklist - skip
	if(address_blacklist.IsIn(info.sin_addr.s_addr))
		return 0;

	return bytes_read;
}