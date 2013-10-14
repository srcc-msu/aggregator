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

#include <netinet/in.h>
#include <arpa/inet.h>

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

	virtual size_t Read(void* buff, size_t count) = 0;
	virtual size_t Write(const void* buff, size_t count) = 0;

	virtual size_t Write(const string& data) = 0;

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
		if(access(fname.c_str(), F_OK) == 0)
		{
			fprintf(stderr, "file exists %s\n", fname.c_str());
			throw CException("can not create socket");
		}

		sockaddr_un address;
		memset(&address, 0, sizeof(sockaddr_un));
		address.sun_family = AF_UNIX;

		socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

		if(socket_fd == -1)
			throw CSyscallException("socket() creation failed");

		unlink(fname.c_str());

		snprintf(address.sun_path, UNIX_PATH_MAX, "%s", fname.c_str());

		if(::bind(socket_fd, (sockaddr *) &address
			, sizeof(sockaddr_un)) != 0) // conflicts with std::bind
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

		if(connect(socket_fd, (sockaddr *) &address
			, sizeof(sockaddr_un)) != 0)
		{
			fprintf(stderr, "can not connect to socket %s\n"
				, fname.c_str());
			throw CSyscallException("connect() failed");
		}
	}

public:

	virtual size_t Read(void* buff, size_t count)
	{
		int bytes_read = read(socket_fd, buff, count);
		return bytes_read;
	}

	virtual size_t Write(const void* buff, size_t count)
	{
		int bytes_written = write(socket_fd, buff, count);
		return bytes_written;
	}

	virtual size_t Write(const string& data)
	{
		return Write(data.c_str(), data.size());
	}

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

/**
	serves to a pair of DGRAM sockets
	they will send messages to each other

	CREATE stands for server mode, address is where to send
	CONNECT stands for client part, address is where to read from
*/
class CDGRAMSocket : public CSocket
{
private:
	string address;
	int port;

	sockaddr_in* tgt_socket;

/**
	create listener
*/
	virtual void Create()
	{
		sockaddr_in addr;

		memset(&addr, 0, sizeof(sockaddr_in));

		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = INADDR_ANY;

		socket_fd = socket(PF_INET, SOCK_DGRAM, 0);

		if(socket_fd == -1)
			throw CSyscallException("socket() creation failed");

		if(::bind(socket_fd, (sockaddr *)&addr, sizeof(sockaddr_in)) == -1)
			throw CSyscallException("bind() failed");
	}

/**
	create sender
*/
	virtual void Connect()
	{
		tgt_socket = new sockaddr_in();
		memset(tgt_socket, 0, sizeof(sockaddr_in));

		tgt_socket->sin_family = AF_INET;
		tgt_socket->sin_port = htons(port);
		tgt_socket->sin_addr.s_addr = inet_addr(address.c_str());

		sockaddr_in my;
		memset(&my, 0, sizeof(sockaddr_in));

		my.sin_family = AF_INET;
		my.sin_port = INADDR_ANY;
		my.sin_addr.s_addr = INADDR_ANY;

		socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
		if(socket_fd == -1)
			throw CSyscallException("socket() creation failed");
		
		
		if(::bind(socket_fd, (sockaddr *)&my, sizeof(sockaddr_in)) == -1)
			throw CSyscallException("bind() failed");
	}

public:

// shoot me for this. or shoot C for lack of usable modules
	static bool IsValidIpAddress(char *ipAddress)
	{
		struct sockaddr_in sa;
		int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
		return result != 0;
	}

	virtual size_t Read(void* buff, size_t count)
	{
		int bytes_read = recv(socket_fd, buff, count, 0);
		return bytes_read;
	}

	virtual size_t Write(const void* buff, size_t count)
	{
		if(tgt_socket == nullptr)
			throw CException("can not write to this socket");

		int bytes_written = sendto(socket_fd, buff, count, 0, (sockaddr *)tgt_socket, sizeof(sockaddr_in));
		return bytes_written;
	}

	virtual size_t Write(const string& data)
	{
		return Write(data.c_str(), data.size());
	}

	~CDGRAMSocket()
	{
		close(socket_fd);

		if(tgt_socket)
			delete tgt_socket;
	}

	CDGRAMSocket(const string& _address, int _port):
		address(_address),
		port(_port),
		tgt_socket(nullptr)
	{
		Init(CSocket :: CONNECT);
	}

	CDGRAMSocket(int _port):
		address(""),
		port(_port),
		tgt_socket(nullptr)
	{
		Init(CSocket :: CREATE);
	}
};

#endif
