#include "aggregator_api.h"

#include <memory>
#include <cstdio>
#include <cassert>

using namespace std;

#include "csv_writer.h"
#include "socket.h"
#include "error.h"

int main(int argc, char** argv)
{
	printf("\n --- starting db writer --- \n\n");

	string mode = "";

	if(argc >= 2)
		mode = argv[1];

	if(mode == "network" && argc == 4)
	{
		string mode = argv[1];
		string str_port;
		string config_fname;

		int port = 0;

		if(argc < 3)
		{
			fprintf(stderr, "missing arguments\n");
			return 1;
		}

		str_port = argv[2];
		port = strtol(str_port.c_str(), nullptr, 10);

		if(argc >= 4)
			config_fname = string(argv[3]);

		printf("starting csv-writer\n");
		printf("port: %d\n", port);
		printf("config file: %s\n", config_fname.c_str());

		try
		{
			CCsvWriter writer(config_fname);

			auto socket = make_shared<CDGRAMSocket>(port);
			writer.FromBin(dynamic_pointer_cast<CSocket>(socket), 128); // mem chunk
		}
		catch(const CSyscallException& e)
		{
			fprintf(stderr, "Syscall exception occured: %s\n", e.what());
			return 1;
		}

		catch(const CException& e)
		{
			fprintf(stderr, "Exception occured: %s\n", e.what());
			return 1;
		}

		printf("\n --- db writer finished--- \n\n");

		return 0;
	}

	printf("Usage:\n");
	printf("./csv_writer network <port> <config file>\n");

	return 0;
}
