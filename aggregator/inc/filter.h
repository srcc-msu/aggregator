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
class AccessList
{
private:
	std :: unordered_set<T> access_list;

public:
	void Add(T value)
	{
		access_list.insert(value);
	}

	void Remove(T value)
	{
		access_list.erase(value);
	}

	bool IsIn(T value) const
	{
		return access_list.find(value) != access_list.end();
	}

	AccessList()
	{}

	~AccessList()
	{}
};

#endif
