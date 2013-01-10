#ifndef PACKET_H
#define PACKET_H

#include <sys/types.h>
#include <cmath>

using std :: fabs;

#include "sensor_metainf.h"

#define MAX_LEN 40 // TODO check or change it

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

	char data_string[MAX_LEN]; // string ??? seems like an easiest way to pass uniform data for now

	SPacket():
	address(0),
	agent_timestamp(0),
	agent_usec(0),
	server_timestamp(0),
	server_usec(0),
	sensor_id(0),
	sensor_num(0)
	{}
};


union UValue
{
	uint64_t b8[1];
	uint32_t b4[2];
	uint16_t b2[4];
	uint8_t b1[8];

	float  f4[2];
	double f8[1];
};

UValue ParseSensValue(unsigned char* buffer, size_t msg_length);

/**
	TODO check div zero
*/
double GetDiv(UValue v1, UValue v2, e_sens_type type, size_t msg_length);
UValue MultValue(UValue value, e_sens_type type, size_t msg_length, double mult);

/**
	Packet structure with extended info that will be used for filtering.
	Currently copies and modifies the packet
*/
struct SPacketExt
{
	SPacket packet;
	
	UValue value;

	SSensorMetainf info;

private:
	void WriteValueToPacket();

public:
	SPacketExt()
	{}

/**
	wheh this class is constucted - it fills data_string field of packet
	and stores modified copy inside himself. Old packet should not be used.
	TODO: make it more clear
*/
	SPacketExt(SPacket packet, unsigned char* raw_buffer):
	packet(packet)
	{
		info = sensor_metainf[packet.sensor_id];
		value = ParseSensValue(raw_buffer, info.msg_length);
		value = MultValue(value, info.type, info.msg_length, info.scale);

		WriteValueToPacket();
	}
};

#endif