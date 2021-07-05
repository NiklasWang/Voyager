#include <errno.h>

#include "SemaphoreTimed.h"

namespace pandora {

SemaphoreTimed::WaitStatus SemaphoreTimed::wait()
{
    int32_t rc = 0;
    WaitStatus status = WAIT_STATUS_ERROR;

    struct timespec ts;
    
    pthread_mutex_lock(&mutex);
    clock_gettime(CLOCK_MONOTONIC, &ts);
    int64_t nsec = ts.tv_nsec + 1000000LL * mTime;
    ts.tv_sec += nsec / 1000000000;
    ts.tv_nsec = nsec % 1000000000;
    rc = pthread_cond_timedwait(&cond, &mutex, &ts);
    pthread_mutex_unlock(&mutex);

    if (rc == ETIMEDOUT) {
        status = WAIT_STATUS_TIMEDOUT;
    } else if (rc == 0) {
        status = WAIT_STATUS_INTERRUPTED;
    } else {
        status = WAIT_STATUS_ERROR;
    }

    return status;
}

SemaphoreTimed::SemaphoreTimed(int32_t timeout) :
    Semaphore(),
    mTime(timeout)
{
}

SemaphoreTimed::~SemaphoreTimed()
{
}

};

