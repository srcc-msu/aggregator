#include <string>

using namespace std;

#include "socket.h"
#include "manager.h"

int main(int argc, char** argv)
{
    printf("\n --- starting proxy --- \n\n");

    string config_fname = "aggregator.conf";

    if(argc == 2)
        config_fname = string(argv[1]);

// read config and run aggregator

    CProxyManager proxy(config_fname);

//    int fd1 = open("test1", O_WRONLY | O_CREAT, 0666);
    proxy.BackgroundProcess();

    proxy.BackgroundDispatch();

    int fd = ConnectUDSocket("/tmp/agg_socket");

//    int reader = ListenTCPSocket(fd);

    proxy.AddBinaryStream(fd);

    while(1)
    {
        sleep(1);
    }

    close(fd);

    printf("\n --- proxy finished --- \n\n");

    return 0;
}
