#ifndef FD_WRITER_H
#define FD_WRITER_H

#include <cassert>

using namespace std;

#include "aggregator_api.h"

class CFdWriter
{
protected:
    int fd;

public:
    virtual int Write(SPacket* p, int count) = 0;

    CFdWriter(int _fd):
    fd(_fd)
    {}
};

class CBinaryFdWriter : public CFdWriter
{
private:

public:
    int Write(SPacket* p, int count)
    {
        return write(fd, p, count * sizeof(SPacket));
    }

    CBinaryFdWriter(int _fd):
    CFdWriter(_fd)
    {}
};

class CJsonFdWriter : public CFdWriter
{
private:

public:
    int Write(SPacket* p, int count)
    {
        assert(false);

        char buffer[128];

        int bs = sprintf(buffer, "{%d,%d}", p->sensor_id, p->sensor_num);

        return write(fd, buffer, bs);
    }

    CJsonFdWriter(int _fd):
    CFdWriter(_fd)
    {}
};

#endif
