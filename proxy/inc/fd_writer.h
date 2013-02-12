#ifndef FD_WRITER_H
#define FD_WRITER_H

using namespace std;

#include "aggregator_api.h"
#include "socket.h"

class CFdWriter
{
protected:
    shared_ptr<CSocket> socket;

public:
    virtual int Write(const SPacket* p, int count) const = 0;

    CFdWriter(shared_ptr<CSocket> _socket):
    socket(_socket)
    {}
};

/**
    big \mem_chunk = more transfer per one write, but more useless values
*/
class CBinaryFdWriter : public CFdWriter
{
private:
    int mem_chunk;

public:

/**

*/
	int Write(const SPacket* p, int count) const
    {
        if(count % mem_chunk)
        {
            fprintf(stderr, "attempt to write memory chunk with wrong size %d\n", count);
            throw CException("CFdWriter :: Write failed");
        }

        int total_bytes_write = 0;

        for(int i = 0; i < count; i += mem_chunk)
        {
            int bytes_write = socket->Write(p+i, sizeof(SPacket)*mem_chunk);

            if(bytes_write == -1)
            {
                return -1;
            }

            total_bytes_write += bytes_write;
        }

        if(total_bytes_write !=  (int)sizeof(SPacket)*count)
            printf("tried to write %d, wrote %d\n"
                , (int)sizeof(SPacket)*count, total_bytes_write);

		return total_bytes_write;
    }

    CBinaryFdWriter(shared_ptr<CSocket> socket, int _mem_chunk):
    CFdWriter(socket),
    mem_chunk(_mem_chunk)
    {}
};

class CJsonFdWriter : public CFdWriter
{
private:

public:
	int Write(const SPacket* p, int count) const
    {
        throw CException("NIY");

        char buffer[128];

        int bs = sprintf(buffer, "{%d,%d}", p->sensor_id, p->sensor_num);

        return socket->Write(buffer, bs);
    }

    CJsonFdWriter(shared_ptr<CSocket> socket):
    CFdWriter(socket)
    {}
};

#endif
