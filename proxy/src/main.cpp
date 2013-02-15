#include <string>

using namespace std;

#include "socket.h"
#include "manager.h"
#include "control.h"

int main(int argc, char** argv)
{
    printf("\n --- starting proxy --- \n\n");

    string ctl_fname = "/tmp/proxy";
    string config_fname = "aggregator.conf";

    if(argc >= 2)
        ctl_fname = string(argv[1]);

    if(argc >= 3)
        config_fname = string(argv[2]);

    printf("starting proxy\ncontrol socket: %s\nconfig file: %s\n"
        , ctl_fname.c_str(), config_fname.c_str());

    try
    {
        CControl control(ctl_fname, config_fname);

        control.ProcessCommands();
    }
    catch(const CSyscallException& e)
    {
        fprintf(stderr, "syscall exception occured: %s\n", e.what());
        return 1;
    }

    catch(const CException& e)
    {
        fprintf(stderr, "exception occured: %s\n", e.what());
        return 1;
    }

    printf("\n --- proxy finished --- \n\n");

    return 0;
}
