#include <string.h>

using namespace std;

#include "socket.h"
#include "error.h"

int main(int argc, char** argv)
{
	printf("\n --- hopsa config utility --- \n\n");

	string mode = "";

	if(argc >= 2)
		mode = argv[1];

	if(mode == "init_stream" && argc == 5)
	{
		string proxy_socket_fname = string(argv[2]);
		string remote_address = string(argv[3]);

		string str_port = argv[4];

		string msg = string("n") + remote_address + " " + str_port;

		printf("Sending command message to socket %s : %s\n", proxy_socket_fname.c_str(), msg.c_str());
	
		CUDSocket proxy_ctl_socket(proxy_socket_fname, CSocket :: CONNECT);
		proxy_ctl_socket.Write(msg);

		return 0;
	}
	else if(mode == "stop_stream" && argc == 4)
	{
		string proxy_socket_fname = string(argv[2]);

		string str_uid = argv[3];

		string msg = string("d") + str_uid;

		printf("Sending command message to socket %s : %s\n", proxy_socket_fname.c_str(), msg.c_str());
	
		CUDSocket proxy_ctl_socket(proxy_socket_fname, CSocket :: CONNECT);
		proxy_ctl_socket.Write(msg);

		return 0;
	}

	printf("Usage:\n");
	printf("./control init_stream <proxy UD control file> <remote address> <remote port>\n");
	printf("or\n");
	printf("./control stop_stream <proxy UD control file> <uid>\n");

	return 0;
}