#include "packet.h"

#include <cstdio>

using std :: sprintf;

void SPacketExt :: WriteValueToPacket()
{
	auto type = info.type;
	auto msg_length = info.msg_length;
	auto out = packet.data_string;

	if(type == INTEGER || type == WIRECOUNTER || type == WIRESMART)
	{
		if(msg_length == 1) sprintf(out, "%d", value.b1[0]);
		if(msg_length == 2) sprintf(out, "%d", value.b2[0]);
		if(msg_length == 4) sprintf(out, "%d", value.b4[0]);
		if(msg_length == 8) 
		{
			if(value.b4[0] != 0)
				sprintf(out, "%d%d", value.b4[0], value.b4[1]);
			else
				sprintf(out, "%d", value.b4[0]);
		}
	}

	else if(type == WIREFLOAT)
	{
		if(msg_length == 4) sprintf(out, "%f" , float (value.b4[0]));
		if(msg_length == 8) sprintf(out, "%lg", double(value.b8[0])); // ??? TODO check
	}

	else if(type == BINARY)
		assert(false);
}

double GetDiv(UValue v1, UValue v2, e_sens_type type, size_t msg_length)
{
	double res = 1.0;

	if(type == INTEGER || type == WIRECOUNTER || type == WIRESMART)
	{
		if(msg_length == 1) res = double(v1.b1[0]) / v2.b1[0];
		if(msg_length == 2) res = double(v1.b2[0]) / v2.b2[0];
		if(msg_length == 4) res = double(v1.b4[0]) / v2.b4[0];
		if(msg_length == 8) res = double(v1.b8[0]) / v2.b8[0];
	}

	else if(type == BINARY)
	{
		assert(false);
	}

	else if(type == WIREFLOAT)
	{
		if(msg_length == 4) res = double(v1.b4[0]) / float (v2.b4[0]);
		if(msg_length == 8) res = double(v1.b8[0]) / double(v2.b8[0]);
	}

	return res;
}

UValue ParseSensValue(unsigned char* buffer, size_t msg_length)
{
	UValue res;
	
	if(msg_length == 1) res.b1[0] =       *((uint8_t*)  buffer);
	if(msg_length == 2) res.b2[0] = ntohs(*((uint16_t*) buffer));
	if(msg_length == 4) res.b4[0] = ntohl(*((uint32_t*) buffer));
	if(msg_length == 8) 
	{
		res.b4[0] = ntohl(*((uint32_t*) (buffer + 4)));
		res.b4[1] = ntohl(*((uint32_t*)  buffer     ));
	}

	return res;
}