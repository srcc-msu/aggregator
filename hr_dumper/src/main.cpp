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
        case E_VAL_TYPE :: uint64 : sprintf(str, "%lu", value.b8[0]); break;

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

    size_t bytes_read = 0;
	size_t total = 0;

    while((bytes_read = socket->Read(packets, sizeof(SPacket) * mem_chunk)) 
        == (size_t)sizeof(SPacket) * mem_chunk)
	{
        for(size_t i = 0; i < mem_chunk; i++)
        {
        	if(packets[i].sensor_id == 0)
        		break;

			printf("%u;%u%06u;%02u.%02u.%02u.%02u;%s;%u;%.3f\n"
                , packets[i].sensor_id
				, packets[i].server_timestamp, packets[i].server_usec
				, packets[i].address.b1[0], packets[i].address.b1[1], packets[i].address.b1[2], packets[i].address.b1[3]
				, UValueToString(packets[i].value, packets[i].type)
				, packets[i].sensor_num, packets[i].speed);

            total += bytes_read;
        }
	}

    fprintf(stderr, "read %zu/%zu, not match to %zu\n", bytes_read, total
        , (size_t)sizeof(SPacket) * mem_chunk);

    delete[] packets;
}


int main(int argc, char** argv)
{
	printf("\n --- starting hr dumper --- \n\n");

	string mode = "";

	if(argc >= 2)
		mode = argv[1];

	if(mode == "network" && argc == 3)
    {
		string mode = argv[1];
        string str_port;
        
        int port = 0;

        str_port = argv[2];
        port = strtol(str_port.c_str(), nullptr, 10/*base*/);

        printf("starting hr_dumper\n");
        printf("port: %d\n", port);

        try
        {
	        auto socket = make_shared<CDGRAMSocket>(port);
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

    printf("Usage:\n");
    printf("./hr_dumper network <port>\n");

	return 0;
}
