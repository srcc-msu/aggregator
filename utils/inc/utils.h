#ifndef UTILS_H
#define UTILS_H

static const unsigned int MIN_SLEEP_TIME = 8;
static const unsigned int MAX_SLEEP_TIME = 1024*1204;

/**
    provides ability to sleep smlower or faster, basing on predicat
*/
class CDynSleeper
{
private:
    unsigned int min_sleep_time;
    unsigned int max_sleep_time;

    unsigned int sleep_time;

public:

/**
    get current sleep time
*/
    unsigned int GetTime() const
    {
        return sleep_time;
    }

/**
    sleep
    if \sleep_more is true - sleep more than next time, other way - less
*/
    void Sleep(bool sleep_more)
    {
        if(sleep_more && sleep_time < MAX_SLEEP_TIME)
            sleep_time *= 2;
        else if(sleep_time > MIN_SLEEP_TIME)
            sleep_time /= 2;

        usleep(sleep_time);
    }

    CDynSleeper(int _min_sleep_time = MIN_SLEEP_TIME
        , int _max_sleep_time = MAX_SLEEP_TIME):
        min_sleep_time(_min_sleep_time),
        max_sleep_time(_max_sleep_time),
        sleep_time(_min_sleep_time)
    {}
};

#endif
