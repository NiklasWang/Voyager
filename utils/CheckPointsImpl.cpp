#include <thread>
#include <stdarg.h>

#include "Memory.h"
#include "CheckPointsImpl.h"

#define CHECKPOINT_NAME_MAX_LEN 4096

namespace pandora {

CheckPointsImpl gDefaultCheckPointsImpl;

sp<CheckPointsImpl::LocalCheckPointInterface>
CheckPointsImpl::getOrCreateLocalCheckPoint()
{
    int32_t rc = NO_ERROR;
    std::size_t index = 0;//std::hash<std::thread::id>{}(std::this_thread::get_id());
    sp<LocalCheckPointInterface> result;

    if (SUCCEED(rc)) {
        if (mCheckPoints.find(index) == mCheckPoints.end()) {
            result = new LocalCheckPoint();
            if (ISNULL(result)) {
                LOGE(mModule, "Failed to create local check point.");
                rc = NO_MEMORY;
            } else {
                mCheckPoints[index] = result;
            }
        } else {
            result = mCheckPoints[index];
        }
    }

    return result;
}

sp<CheckPointsImpl::LocalCheckPointInterface>
CheckPointsImpl::getOrCreateLocalOrderFixedCheckPoint()
{
    int32_t rc = NO_ERROR;
    std::size_t index = 0;//std::hash<std::thread::id>{}(std::this_thread::get_id());
    sp<LocalCheckPointInterface> result;

    if (SUCCEED(rc)) {
        if (mOrderFixedCheckPoints.find(index) == mOrderFixedCheckPoints.end()) {
            result = new LocalOrderFixedCheckPoint();
            if (ISNULL(result)) {
                LOGE(mModule, "Failed to create local order fixed check point.");
                rc = NO_MEMORY;
            } else {
                mOrderFixedCheckPoints[index] = result;
            }
        } else {
            result = mOrderFixedCheckPoints[index];
        }
    }

    return result;
}

sp<CheckPointsImpl::LocalCheckPointInterface>
CheckPointsImpl::getOrCreateLocalCheckPoint(CheckPointType type)
{
    return type == CHECK_POINT_TYPE_FIXED_ORDER ?
        getOrCreateLocalOrderFixedCheckPoint() :
        getOrCreateLocalCheckPoint();
}

int32_t CheckPointsImpl::request(CheckPointType type, const std::string &name)
{
    sp<CheckPointsImpl::LocalCheckPointInterface> intf =
        getOrCreateLocalCheckPoint(type);
    return NOTNULL(intf) ? intf->request(name) : INTERNAL_ERROR;
}

int32_t CheckPointsImpl::requestBlur(CheckPointType type, const std::string &name)
{
    sp<CheckPointsImpl::LocalCheckPointInterface> intf =
        getOrCreateLocalCheckPoint(type);
    return NOTNULL(intf) ? intf->requestBlur(name) : INTERNAL_ERROR;
}

int32_t CheckPointsImpl::requestNone(CheckPointType type, const std::string &name)
{
    sp<CheckPointsImpl::LocalCheckPointInterface> intf =
        getOrCreateLocalCheckPoint(type);
    return NOTNULL(intf) ? intf->requestNone(name) : INTERNAL_ERROR;
}

int32_t CheckPointsImpl::requestNoneBlur(CheckPointType type, const std::string &name)
{
    sp<CheckPointsImpl::LocalCheckPointInterface> intf =
        getOrCreateLocalCheckPoint(type);
    return NOTNULL(intf) ? intf->requestNoneBlur(name) : INTERNAL_ERROR;
}

int32_t CheckPointsImpl::request(CheckPointType type, bool fmt, const char *format, ...)
{
    int32_t rc = NO_ERROR;
    char *buf = nullptr;

    if (SUCCEED(rc)) {
        buf = (char *)Malloc(CHECKPOINT_NAME_MAX_LEN);
        if (ISNULL(buf)) {
            LOGE(mModule, "Failed to alloc check point name.");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        va_list args;
        va_start(args, format);
        int32_t written = vsnprintf(buf, CHECKPOINT_NAME_MAX_LEN, format, args);
        va_end(args);
        buf[written] = '\0';
    }

    if (SUCCEED(rc)) {
        rc = request(type, buf);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to request for checkpoint %s, %d", buf, rc);
        }
    }

    if (SUCCEED(rc) || FAILED(rc)) {
        if (NOTNULL(buf)) {
            SECURE_FREE(buf);
        }
    }

    return rc;
}

int32_t CheckPointsImpl::reach(CheckPointType type, const std::string &name)
{
    sp<CheckPointsImpl::LocalCheckPointInterface> intf =
        getOrCreateLocalCheckPoint(type);
    return NOTNULL(intf) ? intf->reach(name) : INTERNAL_ERROR;
}

int32_t CheckPointsImpl::reach(CheckPointType type, const char *format, ...)
{
    int32_t rc = NO_ERROR;
    char *buf = nullptr;

    if (SUCCEED(rc)) {
        buf = (char *)Malloc(CHECKPOINT_NAME_MAX_LEN);
        if (ISNULL(buf)) {
            LOGE(mModule, "Failed to alloc check point name.");
            rc = NO_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        va_list args;
        va_start(args, format);
        int32_t written = vsnprintf(buf, CHECKPOINT_NAME_MAX_LEN, format, args);
        va_end(args);
    }

    if (SUCCEED(rc)) {
        rc = reach(type, std::string(buf));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to reach for checkpoint %s, %d", buf, rc);
        }
    }

    if (SUCCEED(rc) || FAILED(rc)) {
        if (NOTNULL(buf)) {
            SECURE_FREE(buf);
        }
    }

    return rc;
}

int32_t CheckPointsImpl::track(CheckPointType type, std::function<int32_t ()> func, bool ignoreFuncReturn)
{
    sp<CheckPointsImpl::LocalCheckPointInterface> intf =
        getOrCreateLocalCheckPoint(type);
    return NOTNULL(intf) ? intf->track(func, ignoreFuncReturn) : INTERNAL_ERROR;
}

int32_t CheckPointsImpl::dump(CheckPointType type)
{
    sp<CheckPointsImpl::LocalCheckPointInterface> intf =
        getOrCreateLocalCheckPoint(type);
    return NOTNULL(intf) ? intf->dump() : INTERNAL_ERROR;
}

void CheckPointsImpl::clear(CheckPointType type)
{
    sp<CheckPointsImpl::LocalCheckPointInterface> intf =
        getOrCreateLocalCheckPoint(type);
    if (NOTNULL(intf)) {
        intf->clear();
    }
    return;
}

void CheckPointsImpl::clearAll()
{
    mCheckPoints.clear();
    mOrderFixedCheckPoints.clear();
}

CheckPointsImpl::CheckPointsImpl() :
    Identifier(MODULE_UTILS, "CheckPointsImpl", "v1.0.0")
{
}

CheckPointsImpl::~CheckPointsImpl()
{
    clearAll();
}

};

