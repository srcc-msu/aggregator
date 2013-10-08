#include <string>

using namespace std;

#include "socket.h"

const int MAX_COMMAND = 1024;

class CControl
{
private:
	CUDSocket ctl_socket;
	CProxyManager proxy;

public:
	
	void AddUDStream(const char* path)
	{
		printf("added new binary stream to: %s\n", path);

		proxy.AddBinaryStream(dynamic_pointer_cast<CSocket>
			(make_shared<CUDSocket>(path, CSocket :: CONNECT)));
	}

	void AddNetworkStream(const char* path)
	{
		printf("added new binary stream to: %s\n", path);

		proxy.AddBinaryStream(dynamic_pointer_cast<CSocket>
			(make_shared<CDGRAMSocket>(path+2, path[0]+path[1]*256)));
	}

	void ProcessCommands()
	{
		char command[MAX_COMMAND+1];

		while(1)
		{
			printf("waiting for a command on the control socket\n");
		
			int bytes_read = ctl_socket.Read(command, MAX_COMMAND);
			command[bytes_read] = '\0';

			switch(command[0])
			{
				case 'b' : AddUDStream(command+1); break;
				case 'n' : AddNetworkStream(command+1); break;
				case 'j' : throw CException("json stream NIY"); break;
				case 'e' : return; break;
				
				default : fprintf(stderr
					, "got unknown command on control socket, ignoring\n");
			}
		}
	}

	CControl() = delete;

	CControl(string ctl_fname, string config_fname):
		ctl_socket(ctl_fname, CSocket :: CREATE),
		proxy(config_fname)
	{
		proxy.BackgroundProcess();
		proxy.BackgroundDispatch();
	}
};