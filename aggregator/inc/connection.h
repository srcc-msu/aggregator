#ifndef CONNECTION_H
#define CONNECTION_H

#include <unordered_map>
#include <iostream>

using std::cout;
using std::endl;

#include <sys/socket.h>
#include <unistd.h>

#include "filter.h"
#include "debug.h"

/**
	Manages data flow socket connection and communication with agents
*/
class CConnectionManager
{
private:
	sockaddr_in data_socket_inf;
	int data_socket;
	uint32_t aggr_address; // network byte order
	uint16_t aggr_port; // network byte order

	int control_port;

// Stores inited agents addresses
	std::unordered_map<uint32_t, long> agents_activity;
	
	AccessList<uint32_t> address_blacklist;

/**
	Creates a socket, where data will be read from
*/
	void Init();

public:
	void AgentStats()
	{
		int not_responding = 0;
		long sum = 0;
		long count = 0;

		for(auto it : agents_activity)
		{
			if(it.second == 0)
			{
				if(not_responding == 0)
					printf("Not responding agents: ");

				in_addr address;
				address.s_addr = it.first;

				printf("%s, ", inet_ntoa(address));

				not_responding ++;
			}
			else
			{
				sum += it.second;
				count ++;
			}
		}

		printf("\nprocessed %ld packets from %ld agents", sum, count);
	}

	void PokeAgents()
	{
		for(auto it : agents_activity)
		{
			if(it.second == 0)
			{
				InitAgent(it.first);
				DMSG1("agent " << it.first << " is not responding, reiniting");
				usleep(10000);
			}
		}
	}

/**
	Adds the \address to the blacklist.	
	All messages from this address will be ignored.
*/
	void BlacklistAddress(uint32_t address)
		{ address_blacklist.Add(address); }

/**
	Remove the \address from the blacklist.	
	All messages from this address will not be ignored anymore.
*/
	void UnblacklistAddress(uint32_t address)
		{ address_blacklist.Remove(address); }

/**
	Form a control message, that sets an angent him to send data
	to \recv_socket.address:\recv_socket.port.
*/
	int FormV1RedirectMessage(unsigned char* msg, uint32_t address) const;

/**
	Sends the control message to the agent on \address:\control_port.
*/
	void InitAgent(uint32_t address);

/**
	Recieves a raw packet from wocket. Provides IP filtering.
	Returns amount of bytes read.
*/
	int GetData(unsigned char* data, int max_count);

/**
	aggr_address in network byte order
*/
	CConnectionManager(int control_port, uint32_t aggr_address):
	aggr_address(aggr_address),
	aggr_port(0),
	control_port(control_port)
	{
		Init();
	}

	~CConnectionManager()
	{
		close(data_socket);
	}
};

#endif