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

    proxy.BackgroundProcess();

    proxy.BackgroundDispatch();

    proxy.AddBinaryStream(dynamic_pointer_cast<CSocket>
        (make_shared<CUDSocket>("/tmp/agg_socket", CSocket :: CONNECT)));

    while(1)
    {
        sleep(1);
    }

    printf("\n --- proxy finished --- \n\n");

    return 0;
}
