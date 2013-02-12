#ifndef ERROR_H
#define ERROR_H

#include <exception>
#include <cstdio>
#include <cstring>

#include <errno.h>

using namespace std;

const int MAX_ERR_STR = 256;

/**
    exception, caused by some error, that may happen and
    should be checked (TODO: not always true)
*/
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

/**
    exception, caused by linux system call failure,
    stores and prints errno
    TODO forbid copy
*/
class CSyscallException : public exception
{
private:
	const char* str;
	char* buffer;

public:

/**
    TODO: mem corruption?
*/
    const char* what() const throw()
    {
    	sprintf(buffer, "%s\n[ errno = %2d ]: %s\n", str, errno, strerror(errno));

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
