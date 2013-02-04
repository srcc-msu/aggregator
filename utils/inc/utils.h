#ifndef UTILS_H
#define UTILS_H

static const unsigned int MIN_SLEEP_TIME = 8;
static const unsigned int MAX_SLEEP_TIME = 1024*1204;

class CDynSleeper
{
private:
    unsigned int sleep_time;

public:

    unsigned int GetTime()
    {
        return sleep_time;
    }

    void Sleep(bool sleep_more)
    {
        if(sleep_more && sleep_time < MAX_SLEEP_TIME)
            sleep_time *= 2;
        else if(sleep_time > MIN_SLEEP_TIME)
            sleep_time /= 2;

        usleep(sleep_time);
    }

    CDynSleeper():
    sleep_time(MIN_SLEEP_TIME)
    {}
};

#endif
