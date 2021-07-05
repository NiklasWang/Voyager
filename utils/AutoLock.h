#ifndef _AUTO_LOCK_H_
#define _AUTO_LOCK_H_

#include <pthread.h>

namespace pandora {

class AutoLock
{
public:
    AutoLock() :
        lock(PTHREAD_MUTEX_INITIALIZER) {
        pthread_mutex_lock(&lock);
    }
    ~AutoLock() {
        pthread_mutex_unlock(&lock);
        pthread_mutex_destroy(&lock);
    }

private:
    AutoLock(const AutoLock &rhs) = delete;
    AutoLock &operator=(const AutoLock &rhs) = delete;

private:
    pthread_mutex_t lock;
};


class AutoExternalLock
{
public:
    explicit AutoExternalLock(pthread_mutex_t *_lock) :
        lock(_lock) {
        pthread_mutex_lock(lock);
    }
    ~AutoExternalLock() {
        pthread_mutex_unlock(lock);
    }

private:
    AutoExternalLock() = delete;
    AutoExternalLock(const AutoExternalLock &rhs) = delete;
    AutoExternalLock &operator=(const AutoExternalLock &rhs) = delete;

private:
    pthread_mutex_t *lock;
};


class Mutex
{
public:
    Mutex() :
        locker(PTHREAD_MUTEX_INITIALIZER) {
    }
    ~Mutex() {
        pthread_mutex_destroy(&locker);
    }
    void lock() {
        pthread_mutex_lock(&locker);
    }
    void unlock() {
        pthread_mutex_unlock(&locker);
    }

private:
    pthread_mutex_t locker;
};

class AutoMutex
{
public:
    explicit AutoMutex(Mutex *_mutex) :
        mutex(_mutex) {
        mutex->lock();
    }
    ~AutoMutex() {
        mutex->unlock();
    }

private:
    AutoMutex() = delete;
    AutoMutex(const AutoMutex &rhs) = delete;
    AutoMutex &operator=(const AutoMutex &rhs) = delete;

private:
    Mutex *mutex;
};

};

#endif
