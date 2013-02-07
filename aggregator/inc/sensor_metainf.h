#ifndef SENSOR_METAINF_H
#define SENSOR_METAINF_H

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <unordered_map>

enum e_sens_type {INTEGER, BINARY, WIRECOUNTER, WIRESMART, WIREFLOAT};

/**
	Stores metainformation about one sensor.
*/
struct SSensorMetainf
{
	e_sens_type	type;
	size_t msg_length;
	double scale;

	SSensorMetainf(e_sens_type type = INTEGER, size_t msg_length = 0, double scale = 1.0):
	type(type),
	msg_length(msg_length),
	scale(scale)
	{}
};

extern std::unordered_map<int, SSensorMetainf> sensor_metainf;
/**
	All metainformation is here.
	Must be updated, if something changes
	TODO: move it outside from the programm
*/
void InitMetainf();

/**
	Parses the raw \buffer, according to provided \id and
	writes the result to \out string
*/

void ParseSensValue(char* out, unsigned char* buffer, int id);

#endif
