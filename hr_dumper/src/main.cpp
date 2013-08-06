#include "aggregator_api.h"

#include <memory>

using namespace std;

#include "socket.h"
#include "error.h"

#include "error.h"

const int MAX_UVALUE_REPR = 64;

const char* UValueToString(UValue value, E_VAL_TYPE type)
{
    static char str[64];
    switch(type)
    {
        case E_VAL_TYPE :: uint8  : sprintf(str, "%u", value.b1[0]); break;
        case E_VAL_TYPE :: uint16 : sprintf(str, "%u", value.b2[0]); break;
        case E_VAL_TYPE :: uint32 : sprintf(str, "%u", value.b4[0]); break;
        case E_VAL_TYPE :: uint64 : sprintf(str, "%Lu", value.b8[0]); break;

        case E_VAL_TYPE :: float32 : sprintf(str, "%f", value.f4[0]); break;
        case E_VAL_TYPE :: float64 : sprintf(str, "%f", value.f8[0]); break;

        default : 
            throw CException("unsupported E_VAL_TYPE type during printing");
    }

    return str;
}

void DumpFromBin(shared_ptr<CSocket> socket, size_t mem_chunk)
{
	SPacket* packets = new SPacket[mem_chunk];

    int bytes_read = 0;
	int total = 0;

    while((bytes_read = socket->Read(packets, sizeof(SPacket) * mem_chunk)) 
        == sizeof(SPacket) * mem_chunk)
	{
        for(size_t i = 0; i < mem_chunk; i++)
        {
        	if(packets[i].sensor_id == 0)
        		break;

			printf("%u;%u;%u%06u;cn%02u;%s;%.3f\n"
                , packets[i].sensor_id, packets[i].sensor_num 
				, packets[i].server_timestamp, packets[i].server_usec
				, packets[i].address.b1[3]
				, UValueToString(packets[i].value, packets[i].type)
				, packets[i].speed);

            total += bytes_read;
        }
	}

    fprintf(stderr, "read %d/%d, not match to %zu\n", bytes_read, total
        , sizeof(SPacket) * mem_chunk);

    delete[] packets;
}


int main(int argc, char** argv)
{
	printf("\n --- starting hr dumper --- \n\n");

    string proxy_socket_fname = "/tmp/proxy";
    string socket_fname = "/tmp/hr_dumper";

    if(argc >= 2)
        proxy_socket_fname = string(argv[1]);

    if(argc >= 3)
        socket_fname = string(argv[2]);


    printf("starting hr dumper\n");
    printf("proxy socket: %s\n", proxy_socket_fname.c_str());
    printf("my socket: %s\n", socket_fname.c_str());

	try
	{
		auto socket = make_shared<CUDSocket>(socket_fname
			, CSocket :: CREATE);

		CUDSocket proxy_ctl_socket(proxy_socket_fname, CSocket :: CONNECT);
		proxy_ctl_socket.Write(string("b")+socket_fname);

		while(1)
			DumpFromBin(dynamic_pointer_cast<CSocket>(socket), 128); // mem chunk
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

	printf("\n --- hr dumper fiished--- \n\n");
	return 0;
}
