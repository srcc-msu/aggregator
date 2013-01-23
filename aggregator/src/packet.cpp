#include "packet.h"

#include <cstdio>

using std :: sprintf;

static const double INF = 1024; // sensor value was zero, and now - not
static const int EPS = 1e-5;

void SPacketExt :: WriteValueToPacket()
{
	auto type = info.type;
	auto msg_length = info.msg_length;
	auto out = packet.data_string;

	if(type == INTEGER || type == WIRECOUNTER || type == WIRESMART)
	{
		if(msg_length == 1) sprintf(out, "%u", value.b1[0]);
		if(msg_length == 2) sprintf(out, "%u", value.b2[0]);
		if(msg_length == 4) sprintf(out, "%u", value.b4[0]);
		if(msg_length == 8) 
		{
			if(value.b4[1] != 0)
				sprintf(out, "%u%010u", value.b4[1], value.b4[0]);
			else
				sprintf(out, "%u", value.b4[0]);
		}
	}

	else if(type == WIREFLOAT)
	{
		if(msg_length == 4) sprintf(out, "%f" , value.f4[0]);
		if(msg_length == 8) sprintf(out, "%lg", value.f8[0]); // ??? TODO check
	}

	else if(type == BINARY)
		assert(false);
}

double GetDiv(const UValue& v1, const UValue& v2, e_sens_type type, size_t msg_length)
{
	if(v1.b8[0] == v2.b8[0])
		return 1.0;

	double res = 1.0;

	if(type == INTEGER || type == WIRECOUNTER || type == WIRESMART)
	{
		if(msg_length == 1) res = (v2.b1[0] != 0) ? double(v1.b1[0]) / v2.b1[0] : INF;
		if(msg_length == 2) res = (v2.b2[0] != 0) ? double(v1.b2[0]) / v2.b2[0] : INF;
		if(msg_length == 4) res = (v2.b4[0] != 0) ? double(v1.b4[0]) / v2.b4[0] : INF;
		if(msg_length == 8) res = (v2.b8[0] != 0) ? double(v1.b8[0]) / v2.b8[0] : INF;
	}

	else if(type == BINARY)
	{
		assert(false); // ??
	}

	else if(type == WIREFLOAT)
	{
		if(msg_length == 4) res = (fabs(v2.f4[0]) > EPS) ? v1.f4[0] / v2.f4[0] : INF; 
		if(msg_length == 8) res = (fabs(v2.f8[0]) > EPS) ? v1.f8[0] / v2.f8[0] : INF;
	}

	return res;
}

UValue GetSum(const UValue& v1, const UValue& v2, e_sens_type type, size_t msg_length)
{
	UValue res;

	if(type == INTEGER || type == WIRECOUNTER || type == WIRESMART)
	{
		if(msg_length == 1) res.b1[0] = v1.b1[0] + v2.b1[0];
		if(msg_length == 2) res.b2[0] = v1.b2[0] + v2.b2[0];
		if(msg_length == 4) res.b4[0] = v1.b4[0] + v2.b4[0];
		if(msg_length == 8) res.b8[0] = v1.b8[0] + v2.b8[0];
	}

	else if(type == BINARY)
	{
		assert(false); // ??
	}

	else if(type == WIREFLOAT)
	{
		if(msg_length == 4) res.f4[0] = v2.f4[0] + v2.f4[0]; 
		if(msg_length == 8) res.f8[0] = v2.f8[0] + v2.f8[0];
	}

	return res;
}

UValue MultValue(const UValue& value, e_sens_type type, size_t msg_length, double mult)
{
	UValue res;

	if(type == INTEGER || type == WIRECOUNTER || type == WIRESMART)
	{
		if(msg_length == 1) res.b1[0] = double(value.b1[0]) * mult;
		if(msg_length == 2) res.b2[0] = double(value.b2[0]) * mult;
		if(msg_length == 4) res.b4[0] = double(value.b4[0]) * mult;
		if(msg_length == 8) res.b8[0] = double(value.b8[0]) * mult;
	}

	else if(type == BINARY)
	{
		assert(false);
	}

	else if(type == WIREFLOAT)
	{
		if(msg_length == 4) res.b4[0] = value.f4[0] * mult;
		if(msg_length == 8) res.b8[0] = value.f8[0] * mult;
	}

	return res;
}

UValue ParseSensValue(unsigned char* buffer, size_t msg_length)
{
	UValue res;
	res.b8[0] = 0;

	if(msg_length == 1) res.b1[0] =      (*((uint8_t *) buffer));
	if(msg_length == 2) res.b2[0] = ntohs(*((uint16_t*) buffer));
	if(msg_length == 4) res.b4[0] = ntohl(*((uint32_t*) buffer));
	if(msg_length == 8) 
	{
		res.b4[0] = ntohl(*((uint32_t*) (buffer + 4)));
		res.b4[1] = ntohl(*((uint32_t*)  buffer     ));
	}

	return res;
}
