#include "aggregator_api.h"

#include <memory>

using namespace std;

#include "csv_writer.h"
#include "socket.h"
#include "error.h"

int main(int argc, char** argv)
{
	printf("\n --- starting db writer --- \n\n");

    string proxy_socket_fname = "/tmp/proxy";
    string socket_fname = "/tmp/csv_writer";
    string config_fname = "csv_writer.conf";

    if(argc >= 2)
        proxy_socket_fname = string(argv[1]);

    if(argc >= 3)
        socket_fname = string(argv[2]);

    if(argc >= 4)
        config_fname = string(argv[3]);


    printf("starting csv-writer\n");
    printf("proxy socket: %s\n", proxy_socket_fname.c_str());
    printf("my socket: %s\n", socket_fname.c_str());
    printf("config file: %s\n", config_fname.c_str());

	try
	{
		CCsvWriter writer(config_fname);

		auto socket = make_shared<CUDSocket>(socket_fname
			, CSocket :: CREATE);
		CUDSocket proxy_ctl_socket(proxy_socket_fname, CSocket :: CONNECT);
		proxy_ctl_socket.Write(string("b")+socket_fname);

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

	printf("\n --- db writer fiished--- \n\n");
	return 0;
}
