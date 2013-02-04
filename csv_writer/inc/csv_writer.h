#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <string>
#include <unordered_map>

using namespace std;

#include "error.h"

/**
	wrapper for usual FILE*, stores how many lines were written
	TODO: make normal class?
*/
struct SFWrap
{
	FILE* f;
	int written;
	string dir;

	void Wipe()
	{
		if(f)
			fclose(f);
		f = nullptr;
		written = 0;
	}

	SFWrap(string _dir = "", FILE* _f = nullptr, int _written = 0):
	f(_f),
	written(_written),
	dir(_dir)
	{}
};

/**
	stores \SPacket's from the stream and stores them in set of files
	rotates files basing on \max_line and \max_time
*/
class CCsvWriter
{
private:
	std::unordered_map<uint16_t, SFWrap> files;
	string base_dir;

	int max_lines;
	int max_time; // TODO: NIY
	int chunk;

	void Config(const string& config_fname);

public:
	void FromBin(int fd);

	CCsvWriter(const string& config_fname):
	max_lines(-1),
	max_time(-1),
	chunk(-1)
	{
		Config(config_fname);
	}

	~CCsvWriter()
	{
		for(auto& i : files)
		{
			i.second.Wipe();
		}
	}
};

#endif
