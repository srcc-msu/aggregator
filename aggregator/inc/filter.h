#ifndef FILTER_H
#define FILTER_H

#include <stdint.h>
#include <math.h>

#include <unordered_set>

#include "packet.h"
#include "sensor_metainf.h"

/**
	provides a base blacklisting interface
	TODO check performance
*/
template <class T>
class Blacklister
{
private:
	std :: unordered_set<T> blacklist;

public:
	void Add(T value)
	{
		blacklist.insert(value);
	}

	void Delete(T value)
	{
		blacklist.erase(value);
	}

	bool IsIn(T value) const
	{
		return blacklist.count(value) > 0;
	}

	Blacklister()
	{}

	~Blacklister()
	{}
};

const double DEFAULT_DELTA = 0.01;
const double DEFAULT_MAX_INTERVAL = 30;

/**
	stores metainformation about each sensor on each address
*/
struct SensorFilterMetainf
{
	double delta;

/*NIY*/
	//int abs_delta; // should be used if \delta == 0

	uint32_t max_interval; // interval in seconds, when new packet will be sent regardless anyting

	SPacketExt last;

	SensorFilterMetainf(double delta = DEFAULT_DELTA, int max_interval = DEFAULT_MAX_INTERVAL):
	delta(delta),
	max_interval(max_interval)
	{}
};


/**
	provides extended filtering options for sensors, basing on values,
	ids and addresses
*/
class CSensorFilter
{
private:
	
	std::unordered_map<uint32_t, std::unordered_map<uint16_t, SensorFilterMetainf>> filters;

public:
/**
	Bad functions, check if the /packet must be filtered or not
	modifies last ext_packets, modifies the \packet
*/
	bool FilterOut(SPacket& packet, unsigned char* raw_buffer);
};

#endif
