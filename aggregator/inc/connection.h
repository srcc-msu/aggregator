#ifndef CONNECTION_H
#define CONNECTION_H

#include <unordered_map>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

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
// Stores inited agents addresses
	unordered_map<uint32_t, long> agents_activity;

	AccessList<uint32_t> address_blacklist;

	sockaddr_in data_socket_inf;
	int data_socket;

	uint32_t aggr_address; // network byte order

	int control_port; // agent control port
	uint16_t aggr_port; // network byte order

/**
	Creates a socket, where data will be read from
*/
	void Init();

public:
/**
	print statistics about active/dead agents, reset counters
*/
	void AgentStats(int sleep_time);

/**
	Send init message to all agents,
	hope they awake and will start to work
*/
	void PokeAgents();

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
	CConnectionManager(int _control_port, uint32_t _aggr_address):
		aggr_address(_aggr_address),
		control_port(_control_port),
		aggr_port(0) // will get from os
	{
		Init();
	}

	~CConnectionManager()
	{
		close(data_socket);
	}
};

#endif
