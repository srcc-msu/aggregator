#include "aggregator_api.h"

#include "csv_writer.h"
#include "socket.h"
#include "error.h"

using namespace std;

int main(int argc, char** argv)
{
	printf("\n --- starting db writer --- \n\n");

    string config_fname = "csv_writer.conf";

    if(argc == 2)
        config_fname = string(argv[1]);

	try
	{
		int fd = ConnectSocket("/tmp/agg_socket"); // TODO: change

		CCsvWriter writer(config_fname);

		writer.FromBin(fd);
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

	printf("\n --- db writer fiished--- \n\n");
	return 0;
}
