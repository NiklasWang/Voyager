#ifndef _SEMAPHORE_TIMED_H_
#define _SEMAPHORE_TIMED_H_

#include <stdint.h>

#include "Semaphore.h"

namespace pandora {

class SemaphoreTimed :
    public Semaphore {
public:
    enum WaitStatus {
        WAIT_STATUS_INTERRUPTED,
        WAIT_STATUS_TIMEDOUT,
        WAIT_STATUS_ERROR,
    };

public:
    WaitStatus wait();

public:
    SemaphoreTimed(int32_t timeout);
    virtual ~SemaphoreTimed();

private:
    SemaphoreTimed(const SemaphoreTimed &rhs) = delete;
    SemaphoreTimed &operator=(const SemaphoreTimed &rhs) = delete;

private:
    int32_t mTime;
};

};

#endif
