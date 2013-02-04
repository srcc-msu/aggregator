#include "socket.h"

#include <cstdio>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>
using namespace std;

#include "error.h"

int ConnectSocket(const string& fname)
{
	int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	
	if(socket_fd == -1)
		throw CSyscallException("socket() creation failed");

	sockaddr_un address;
	memset(&address, 0, sizeof(sockaddr_un));

	address.sun_family = AF_UNIX;
	snprintf(address.sun_path, UNIX_PATH_MAX, fname.c_str());

	if(connect(socket_fd, 
	        (struct sockaddr *) &address, 
	        sizeof(struct sockaddr_un)) != 0)
	{
		fprintf(stderr, "can not connect to socket %s\n", fname.c_str());
		throw CSyscallException("connect() failed");
	}

	return socket_fd;
}
