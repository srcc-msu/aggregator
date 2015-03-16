#ifndef FD_WRITER_H
#define FD_WRITER_H

using namespace std;

#include "socket.h"

class CFdWriter
{
protected:
    shared_ptr<CSocket> socket;

public:
    virtual size_t Write(std::vector<SPacket> packets) const = 0;

    CFdWriter(shared_ptr<CSocket> _socket):
      socket(_socket)
    {}
};

class CBinaryFdWriter : public CFdWriter
{
public:

	size_t Write(std::vector<SPacket> data) const
    {
        size_t total_bytes_write = 0;

        for(auto& packet : data)
        {
            ssize_t bytes_write = socket->Write(&packet, sizeof(SPacket));

            if(bytes_write == -1)
            {
                return -1;
            }

            total_bytes_write += bytes_write;
        }

        if(total_bytes_write != sizeof(SPacket) * data.size())
            printf("warning: tried to write %zu, wrote %zu\n"
                , sizeof(SPacket) * data.size(), total_bytes_write);

		return total_bytes_write;
    }

    CBinaryFdWriter(shared_ptr<CSocket> socket):
        CFdWriter(socket)
    {}
};

#endif
