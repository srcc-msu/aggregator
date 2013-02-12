#ifndef SOCKET_H
#define SOCKET_H

#include <string>

using namespace std;

#include <cstdio>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "error.h"

#ifndef UNIX_PATH_MAX
    #define UNIX_PATH_MAX 108 // TODO: find it..
#endif

/**
    creates a single unix TCP socket
*/
/*int CreateTCPSocket(const string& fname)
{
    sockaddr_un address;
    memset(&address, 0, sizeof(sockaddr_un));
    address.sun_family = AF_UNIX;

    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    signal(SIGPIPE, SIG_IGN); // prevent getting SIGPIPE; TODO: do some other way?

    if(socket_fd == -1)
        throw CSyscallException("socket() creation failed");

    unlink(fname.c_str());

    snprintf(address.sun_path, UNIX_PATH_MAX, "%s", fname.c_str());

    if(::bind(socket_fd, (sockaddr *) &address, sizeof(sockaddr_un)) != 0) // conflicts with std::bind
    {
        throw CSyscallException("bind() failed");
    }

    return socket_fd;
}*/


/**
    listen \socket_fd for one incoming connection
*/
/*
int ListenTCPSocket(int socket_fd)
{
    sockaddr_un address;
    memset(&address, 0, sizeof(sockaddr_un));
    socklen_t address_length;

    if(listen(socket_fd, 1) != 0)
        throw CSyscallException("listen() failed");

    int res = accept(socket_fd, (sockaddr *) &address,
        &address_length);

    if(res == -1)
        throw CSyscallException("accpet() failed");

    return res;
}*/

/**
    connect to unix TCP socket and returns the descriptor
*//*
int ConnectTCPSocket(const string& fname)
{
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if(socket_fd == -1)
        throw CSyscallException("socket() creation failed");

    sockaddr_un address;
    memset(&address, 0, sizeof(sockaddr_un));

    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, UNIX_PATH_MAX, "%s", fname.c_str());

    if(connect(socket_fd, (sockaddr *) &address, sizeof(sockaddr_un)) != 0)
    {
        fprintf(stderr, "can not connect to socket %s\n", fname.c_str());
        throw CSyscallException("connect() failed");
    }

    return socket_fd;
}*/

class CSocket
{
public:
    enum E_TYPE
    {
        CREATE,
        CONNECT,
        LISTEN
    };

protected:
    int socket_fd;

    virtual void Create() = 0;
    virtual void Connect() = 0;

    void Init(CSocket :: E_TYPE type)
    {
        if(type == CSocket :: CREATE)
            Create();
        else if(type == CSocket :: CONNECT)
            Connect();
        else
            throw CException("Listen NIY in CSocket::Init\n");
    }

public:

    int GetFD()
    {
        return socket_fd;
    }

    int Read(void* buff, size_t count)
    {
        int bytes_read = read(socket_fd, buff, count);
        return bytes_read;
    }

    int Write(const void* buff, size_t count)
    {
        int bytes_written = write(socket_fd, buff, count);
        return bytes_written;
    }

    ~CSocket()
    {
        socket_fd = -1;
    }

    CSocket():
    socket_fd(-1)
    {}
};

class CUDSocket : public CSocket
{
private:
    string fname;

    virtual void Create()
    {
        sockaddr_un address;
        memset(&address, 0, sizeof(sockaddr_un));
        address.sun_family = AF_UNIX;

        socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

        if(socket_fd == -1)
            throw CSyscallException("socket() creation failed");

        unlink(fname.c_str());

        snprintf(address.sun_path, UNIX_PATH_MAX, "%s", fname.c_str());

        if(::bind(socket_fd, (sockaddr *) &address, sizeof(sockaddr_un)) != 0) // conflicts with std::bind
        {
            throw CSyscallException("bind() failed");
        }
    }

    virtual void Connect()
    {
        socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

        if(socket_fd == -1)
            throw CSyscallException("socket() creation failed");

        //signal(SIGPIPE, SIG_IGN); // prevent getting SIGPIPE; TODO: do some other way?

        sockaddr_un address;
        memset(&address, 0, sizeof(sockaddr_un));

        address.sun_family = AF_UNIX;
        snprintf(address.sun_path, UNIX_PATH_MAX, "%s", fname.c_str());

        if(connect(socket_fd, (sockaddr *) &address, sizeof(sockaddr_un)) != 0)
        {
            fprintf(stderr, "can not connect to socket %s\n", fname.c_str());
            throw CSyscallException("connect() failed");
        }
    }

public:

    ~CUDSocket()
    {
        close(socket_fd);
        unlink(fname.c_str());
    }

    CUDSocket(const string& _fname, CSocket :: E_TYPE type):
    fname(_fname)
    {
        Init(type);
    }
};

#endif
