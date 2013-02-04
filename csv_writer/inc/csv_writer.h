#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <string>
#include <unordered_map>
#include <memory>

using namespace std;

#include "fwrap.h"

#include "aggregator_api.h"

/**
	get \SPacket's from the stream and store them in set of files
	rotate files basing on \max_line and \max_time
*/
class CCsvWriter
{
private:
    unordered_map<uint16_t, shared_ptr<CFWrap<SPacket>>> files;
    
    string base_dir;
    
    int max_lines;
    int max_time;
    
/**
	read config from JSON /config_fname
	JSON is temporary
	TODO: move to YAML, check that all fields are configured
*/
    void Config(const string& config_fname);

public:
/**
	parse \SPacket's from linux file descriptor \fd and route them to apropriated files
*/
    void FromBin(int fd);

    CCsvWriter(const string& config_fname):
    max_lines(-1),
    max_time(-1)
	{
		Config(config_fname);
	}

    ~CCsvWriter() // TODO: ???
    {
//    	for(auto& i : files)
//    		i.second->Wipe();
    }
};

#endif
