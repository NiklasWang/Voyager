#include "Logs.h"
#include "Atomic.h"
#include "FragementBuffer.h"
#include "ObjectBuffer.h"

namespace pandora {

int32_t FragementBuffer::flush()
{
    int32_t rc = NOT_FOUND;
    RWLock::AutoRLock l(mMapLock);

    auto iter = mMap.begin();
    while (iter != mMap.end()) {
        rc = iter->second->flush();
        iter++;
    }

    return rc;
}

int32_t FragementBuffer::dump()
{
    int32_t rc = NOT_FOUND;
    RWLock::AutoRLock l(mMapLock);

    auto iter = mMap.begin();
    while (iter != mMap.end()) {
        rc = iter->second->dump();
        iter++;
    }

    return rc;
}

int32_t FragementBuffer::dump(const char *file, const char *func, const int32_t line)
{
    LOGI(mModule, "Dump by  %s | %s | +%d", file, func, line);
    return dump();
}

FragementBuffer::FragementBuffer(uint32_t eachCapacity) :
    mModule(MODULE_OBJECT_BUFFER),
    mEachCapacity(eachCapacity)
{
}

FragementBuffer::~FragementBuffer()
{
    RWLock::AutoWLock l(mMapLock);

    while (mMap.begin() != mMap.end()) {
        auto iter = mMap.begin();
        iter->second->flush();
        delete iter->second;
        mMap.erase(iter);
    }
    mMap.clear();
}

};

