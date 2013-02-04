#include "aggregator_api.h"

#include "csv_writer.h"
#include "socket.h"
#include "error.h"

using namespace std;

int main()
{
	printf("\n --- starting db writer --- \n\n");

	try
	{
		int fd = ConnectSocket("/tmp/agg_socket");

		CCsvWriter writer("csv_writer.conf");

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
