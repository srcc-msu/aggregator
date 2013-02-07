#ifndef FD_WRITER_H
#define FD_WRITER_H

using namespace std;

#include "aggregator_api.h"

class CFdWriter
{
protected:
    int fd;

public:
    virtual int Write(const SPacket* p, int count) const = 0;

    CFdWriter(int _fd):
    fd(_fd)
    {}
};

class CBinaryFdWriter : public CFdWriter
{
private:

public:
	int Write(const SPacket* p, int count) const
    {
        int total_bytes_write = 0;

        for(int i = 0; i < count; i++)
        {
            int bytes_write = write(fd, p+i, sizeof(SPacket));

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

    CBinaryFdWriter(int _fd):
    CFdWriter(_fd)
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

        return write(fd, buffer, bs);
    }

    CJsonFdWriter(int _fd):
    CFdWriter(_fd)
    {}
};

#endif
