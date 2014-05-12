#include <string>

using namespace std;

#include "socket.h"
#include "manager.h"
#include "control.h"

int main(int argc, char** argv)
{
	printf("\n --- starting proxy --- \n\n");

	if(argc == 3)
	{
		string ctl_fname = string(argv[1]);
		string config_fname = string(argv[2]);

		printf("starting proxy\n");
		printf("control socket: %s\n", ctl_fname.c_str());
		printf("config file: %s\n", config_fname.c_str());

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

	printf("Usage:\n");
	printf("./proxy <proxy UD control file> <config file>\n");

	return 0;
}
