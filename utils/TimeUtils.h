#ifndef _UTILS_TIMES_H_
#define _UTILS_TIMES_H_

#include <stdint.h>
#include <ctime>
#include <stdint.h>
#include <iostream>
#include <string>
#include <sys/time.h>

namespace pandora {

class TimeUtils
{
public:

    static inline int64_t getTimeNs()
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return ts.tv_sec * 1000000000 + ts.tv_nsec;
    }

    static inline int64_t getTimeUs()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000000ll + tv.tv_usec;
    }

    static inline int64_t getTimeMs()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    //YY-mm-dd HH:MM:SS
    static inline std::string getDateTime()
    {
        time_t now = time(0);
        tm *ltm = localtime(&now);

        char myDate[40],myTime[40];
        strftime(myDate, 40, "%Y-%m-%d", ltm);
        strftime(myTime, 40, " %H:%M:%S", ltm);

        return std::string(myDate) + std::string(myTime);
    }

    // Get HHMMSSsss time format according to given Ns
    static int getLocalTime_t(int64_t localTime)
    {
        time_t local_time = localTime / 1000000000;
        struct tm* tm;
        tm = localtime(&local_time);
        int time_ = 0;
        if(localTime / 1000000000 > 0) {
            time_ = (tm->tm_hour * 10000 + tm->tm_min * 100 + tm->tm_sec) * 1000 +  localTime  % 1000000000 / 1000000;
        } else {
            time_ = localTime % 1000000000 / 1000000;
        }
        return time_; // Ms
    }

};

};

#endif
