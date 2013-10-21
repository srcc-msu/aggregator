#include "packet.h"

#include <cmath>

#include <cstdio>
#include "error.h"

using namespace std;

static const double INF = 1024; // sensor value was zero, and now - not
static const double EPS = 1e-5;

//--------------------------------

double GetDiv(const UValue& v1, const UValue& v2, E_VAL_TYPE type)
{
	if(v1.b8[0] == v2.b8[0])
		return 1.0;

	double res = 1.0;

	switch(type)
	{
		case E_VAL_TYPE :: uint8  : res = (v2.b1[0] != 0)
			? double(v1.b1[0]) / v2.b1[0] : INF; break;
		case E_VAL_TYPE :: uint16 : res = (v2.b2[0] != 0)
			? double(v1.b2[0]) / v2.b2[0] : INF; break;
		case E_VAL_TYPE :: uint32 : res = (v2.b4[0] != 0)
			? double(v1.b4[0]) / v2.b4[0] : INF; break;
		case E_VAL_TYPE :: uint64 : res = (v2.b8[0] != 0)
			? double(v1.b8[0]) / v2.b8[0] : INF; break;

		case E_VAL_TYPE :: float32 : res = (fabs(v2.f4[0]) > EPS)
			? v1.f4[0] / v2.f4[0] : INF; break;
		case E_VAL_TYPE :: float64 : res = (fabs(v2.f8[0]) > EPS)
			? v1.f8[0] / v2.f8[0] : INF; break;

		default :
			throw CException("unsupported E_VAL_TYPE type during div");
	}

	return res;
}

//--------------------------------

double GetDiff(const UValue& v1, const UValue& v2, E_VAL_TYPE type)
{
	if(v1.b8[0] == v2.b8[0])
		return 0.0;

	double res = 0.0;

	switch(type)
	{
		case E_VAL_TYPE :: uint8  : res = double(v1.b1[0]) - v2.b1[0]; break;
		case E_VAL_TYPE :: uint16 : res = double(v1.b2[0]) - v2.b2[0]; break;
		case E_VAL_TYPE :: uint32 : res = double(v1.b4[0]) - v2.b4[0]; break;
		case E_VAL_TYPE :: uint64 : res = double(v1.b8[0]) - v2.b8[0]; break;

		case E_VAL_TYPE :: float32 : res = v1.f4[0] - v2.f4[0]; break;
		case E_VAL_TYPE :: float64 : res = v1.f8[0] - v2.f8[0]; break;

		default :
			throw CException("unsupported E_VAL_TYPE type during diff");
	}

	return res;
}

//--------------------------------

UValue GetSum(const UValue& v1, const UValue& v2, E_VAL_TYPE type)
{
	UValue res;

	switch(type)
	{
		case E_VAL_TYPE :: uint8  : res.b1[0] = v1.b1[0] + v2.b1[0]; break;
		case E_VAL_TYPE :: uint16 : res.b2[0] = v1.b2[0] + v2.b2[0]; break;
		case E_VAL_TYPE :: uint32 : res.b4[0] = v1.b4[0] + v2.b4[0]; break;
		case E_VAL_TYPE :: uint64 : res.b8[0] = v1.b8[0] + v2.b8[0]; break;

		case E_VAL_TYPE :: float32 : res.f4[0] = v1.f4[0] + v2.f4[0]; break;
		case E_VAL_TYPE :: float64 : res.f8[0] = v1.f8[0] + v2.f8[0]; break;

		default :
			throw CException("unsupported E_VAL_TYPE type during sum");
	}

	return res;
}

//--------------------------------

UValue MultValue(const UValue& value, E_VAL_TYPE type, double mult)
{
	UValue res;

	switch(type)
	{
		case E_VAL_TYPE :: uint8  : res.b1[0] = double(value.b1[0])
			* mult; break;
		case E_VAL_TYPE :: uint16 : res.b2[0] = double(value.b2[0])
			* mult; break;
		case E_VAL_TYPE :: uint32 : res.b4[0] = double(value.b4[0])
			* mult; break;
		case E_VAL_TYPE :: uint64 : res.b8[0] = double(value.b8[0])
			* mult; break;

		case E_VAL_TYPE :: float32 : res.f4[0] = value.f4[0] * mult; break;
		case E_VAL_TYPE :: float64 : res.f8[0] = value.f8[0] * mult; break;

		default :
			throw CException("unsupported E_VAL_TYPE type during mult");
	}

	return res;
}

UValue ConvertTo(E_VAL_TYPE type_to, const UValue& value, E_VAL_TYPE type)
{
	UValue res;

	if(type != E_VAL_TYPE :: uint32 || type_to != E_VAL_TYPE :: float32)
		throw CException("unsupported types convert : NIY");

	res.f4[0] = value.b4[0];

	return res;
}

//--------------------------------

UValue ParseUValue(unsigned char* buffer, size_t msg_length)
{
	UValue res;

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

//--------------------------------

E_VAL_TYPE GetType(uint16_t sensor_id, e_sens_type type, size_t msg_length)
{
	if(type == INTEGER || type == WIRECOUNTER || type == WIRESMART)
	{
		if(msg_length == 1) return E_VAL_TYPE :: uint8;
		if(msg_length == 2) return E_VAL_TYPE :: uint16;
		if(msg_length == 4) return E_VAL_TYPE :: uint32;
		if(msg_length == 8) return E_VAL_TYPE :: uint64;
	}
	else if(type == WIREFLOAT)
	{
		if(msg_length == 4) return E_VAL_TYPE :: float32;
		if(msg_length == 8) return E_VAL_TYPE :: float64;
	}

	fprintf(stderr, "%d != {%d, %d, %d, %d}; size: %zu\n"
		, type, INTEGER, WIRECOUNTER, WIRESMART, WIREFLOAT, msg_length);

	throw CException("unsupported packet type during parsing");
}

//--------------------------------

void CreatePacket(SPacket& packet, unsigned char* buffer
    , uint16_t sensor_id, uint8_t sensor_num
    , UIP address
    , uint32_t server_timestamp, uint32_t server_usec
    , uint32_t agent_timestamp, uint32_t agent_usec)
{
    const SSensorMetainf& info = sensor_metainf[sensor_id];

    packet.type = GetType(sensor_id, info.type, info.msg_length);
    packet.value = ParseUValue(buffer, info.msg_length);

// if we try to reduce integer - expand uint32 to float32
    if(info.scale < 1.0 - EPS && packet.type == E_VAL_TYPE :: uint32)
    {
		packet.value = ConvertTo(E_VAL_TYPE :: float32, packet.value, packet.type);
		packet.type = E_VAL_TYPE :: float32;
    }

    packet.value = MultValue(packet.value, packet.type, info.scale);

    packet.sensor_id        = sensor_id;
    packet.sensor_num       = sensor_num;
    packet.address          = address;
    packet.server_timestamp = server_timestamp;
    packet.server_usec      = server_usec;

    packet.agent_timestamp = agent_timestamp;
    packet.agent_usec      = agent_usec;
}
