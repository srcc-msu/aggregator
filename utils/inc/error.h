#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <cstdio>
#include <cstring>

#include <errno.h>

using namespace std;

const int MAX_ERR_STR = 256;

class CException : public exception
{
private:
	const char* str;

public:
    const char* what() const throw()
    {
    	return str;
    }

    CException(const char* _str = nullptr):
    str(_str)
    {}
};

class CSyscallException : public exception
{
private:
	const char* str;
	char* buffer;

public:
    const char* what() const throw()
    {
    	static char error[MAX_ERR_STR] = "";
    	strerror_r(errno, buffer, MAX_ERR_STR);


	  	sprintf(buffer, "%s\n[ errno = %2d ]: %s", str, errno, error);

    	return buffer;
    }

    CSyscallException(const char* _str = ""):
    str(_str)
    {
    	size_t len = strlen(str);
    	buffer = new char[len + MAX_ERR_STR + 32];
    }

    ~CSyscallException() throw()
    {
    	if(buffer)
    	{
    		delete[] buffer;
    		buffer = nullptr;
    	}
    }
};

#endif
