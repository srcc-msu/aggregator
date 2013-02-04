#ifndef SOCKET_H
#define SOCKET_H

#include <string>

using namespace std;

#ifndef UNIX_PATH_MAX
    #define UNIX_PATH_MAX 108 // TODO: find it..
#endif

/**
	connects to unix socket and returns the descriptor
*/
int ConnectSocket(const string& fname);

#endif
