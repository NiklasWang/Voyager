#ifndef __OVERALL_CONTROL_SINGLETON_H_
#define __OVERALL_CONTROL_SINGLETON_H_

#include <pthread.h>

#include "OverallControlAllocator.h"

namespace voyager {

class OverallControlSingleton :
    public OverallControlAllocator {
public:

    static OverallControlSingleton *getInstance();
    static uint32_t removeInstance();

private:
    OverallControlSingleton();
    virtual ~OverallControlSingleton();
    OverallControlSingleton(const OverallControlSingleton &rhs) = delete;
    OverallControlSingleton &operator=(const OverallControlSingleton &rhs) = delete;

private:
    static uint32_t gCnt;
    static pthread_mutex_t gInsLock;
    static OverallControlSingleton *gThis;
};

};

#endif
