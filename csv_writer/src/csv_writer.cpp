#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <fstream>
#include <sstream>
#include <memory>
#include <thread>

using namespace std;

#include <limits.h>

#include <json/json.h>

#include "error.h"
#include "aggregator_api.h"

/**
	wrapper for usual FILE*, stores how many lines it did write and
	when last item has been written
	provides buffering and rotating options
*/

class CFWrap
{
private:
// only one instance of this class will write to disk at single moment	
	static mutex global_write_mutex; 

//	mutex queue_mutex;
//	lock_guard<mutex> lock(queue_mutex);

    string dir;

    shared_ptr<vector<SPacket>> queue;

    size_t max_lines;
    unsigned int max_time;

    void ReinitQueue()
    {

		queue = make_shared<vector<SPacket>>();
		queue->reserve(max_lines+1);
    }

/**
	reinit queue, write current queue to disk, threadsafe
*/
	void WriteHelper(shared_ptr<vector<SPacket>> queue_dump)
	{
		lock_guard<mutex> lock(CFWrap :: global_write_mutex);

		char fname[PATH_MAX] = "";
		snprintf(fname, PATH_MAX, "%s%u%06u.csv", dir.c_str(), (*queue_dump)[0].server_timestamp, (*queue_dump)[0].server_usec);

		FILE* f = fopen(fname, "w");

		if(!f)
		{
			fprintf(stderr, "can not open file: %s\n", fname);
			
			throw CSyscallException("fopen failed");
		}
	
		printf("Writing file to disk: %s\n", fname);

		for(auto& it : *queue_dump)
		{
			fprintf(f, "%u%06u;node-%02u-%02u;%s;%u\n", it.server_timestamp, it.server_usec, 
				it.address.b1[2], it.address.b1[3], it.data_string, it.sensor_num);
		}

		fclose(f);

		printf("Writing finished\n");
	}

	void Write()
	{
		shared_ptr<vector<SPacket>> queue_dump = queue;

		ReinitQueue();

		thread t(&CFWrap :: WriteHelper, this, queue_dump);

    	t.detach();
	}


public:
	void AddPacket(const SPacket& p)
	{
		queue->push_back(p);


		if(queue->size() >= max_lines)
		{
			printf("%d sizeout %d/%d : \n", p.sensor_id, (int)queue->size(), (int)max_lines);
	
			Write();
		}
		else if(queue->size() > 0 && p.server_timestamp - (*queue)[0].server_timestamp > max_time)
		{
			printf("%d timeout %u/%u : \n", p.sensor_id, p.server_timestamp - (*queue)[0].server_timestamp, max_time);
	
			Write();
		}
	}

    CFWrap(string _dir, int _max_lines, int _max_time, FILE* _f = nullptr):
    dir(_dir),
    max_lines(_max_lines),
    max_time(_max_time)
    {
		ReinitQueue();
    }
};

mutex CFWrap :: global_write_mutex;

/**
	stores \SPacket's from the stream and stores them in set of files
	rotates files basing on \max_line and \max_time
*/
class CCsvWriter
{
private:
    unordered_map<uint16_t, shared_ptr<CFWrap>> files;
    
    string base_dir;
    
    int max_lines;
    int max_time;
    
    void Config(const string& config_fname);

public:
    void FromBin(int fd);

    CCsvWriter(const string& config_fname):
    max_lines(-1),
    max_time(-1)
	{
		Config(config_fname);
	}

    ~CCsvWriter()
    {
//    	for(auto& i : files)
//    		i.second->Wipe();
    }
};

void CCsvWriter :: Config(const string& config_fname)
{
    ifstream tmp(config_fname);
    stringstream buffer;
    buffer << tmp.rdbuf();

    Json::Value root;
    Json::Reader reader;

    if(!reader.parse(buffer.str(), root))
    {
        fprintf(stderr, "failed to parse configuration: %s\n", reader.getFormatedErrorMessages ().c_str());
            
        throw CException("json parser failed");
    }

//  default interval

    max_lines = root["csv_writer"]["write"]["lines"].asInt();
    printf("Max lines count before writing the file %d\n", max_lines);

    max_time = root["csv_writer"]["write"]["time"].asInt();
    printf("Max time before writing to disk set to %d\n", max_time);

	base_dir = root["csv_writer"]["base_dir"].asString();
	printf("Base dir for data: %s\n", base_dir.c_str());

    const Json::Value dirs = root["csv_writer"]["sensor_dirs"];

    for(size_t i = 0; i < dirs.size(); i++)
    {
    	files[dirs[i][0u].asInt()] = make_shared<CFWrap>(base_dir + dirs[i][1u].asString(), max_lines, max_time);
        printf("dir for sensor %d is %s\n", dirs[i][0u].asInt(), dirs[i][1u].asCString());
    }
}

void CCsvWriter :: FromBin(int fd)
{
	SPacket packet;

	while(read(fd, &packet, sizeof(SPacket)) == sizeof(SPacket))
	{
		auto it = files.find(packet.sensor_id);

		if(it != files.end())
		{
			shared_ptr<CFWrap> fwrap = it->second;
			fwrap->AddPacket(packet);
		}
	}
}

#endif

