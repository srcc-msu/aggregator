#include "csv_writer.h"

#include <fstream>
#include <memory>
#include <sstream>

using namespace std;

#include <json/json.h>

#include "error.h"

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

    max_lines = root["csv_writer"]["write"]["lines"].asInt();
    printf("Max lines count before writing the file %d\n", max_lines);

    max_time = root["csv_writer"]["write"]["time"].asInt();
    printf("Max time before writing to disk set to %d\n", max_time);

	base_dir = root["csv_writer"]["base_dir"].asString();
	printf("Base dir for data: %s\n", base_dir.c_str());

    const Json::Value dirs = root["csv_writer"]["sensor_dirs"];

    for(size_t i = 0; i < dirs.size(); i++)
    {
    	files[dirs[i][0u].asInt()] = make_shared<CFWrap<SPacket>>(base_dir + dirs[i][1u].asString(), max_lines, max_time);
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
			shared_ptr<CFWrap<SPacket>> fwrap = it->second;
			fwrap->AddPacket(packet);
		}
	}
}
