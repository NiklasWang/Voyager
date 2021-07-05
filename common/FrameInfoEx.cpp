#include "FrameInfoEx.h"
#include "Common.h"
#include "Logs.h"
#include "Memory.h"
#include "InterfaceImpl.h"

namespace pandora {

int32_t FrameInfoEx::allocAndCopy(const FrameInfo &info, bool copy)
{
    int32_t rc = NO_ERROR;
    int64_t size = 0LL;
    uint8_t *data = nullptr;

    if (SUCCEED(rc)) {
        rc = assign(info);
        if (FAILED(rc)) {
            LOGE(MODULE_COMMON, "Failed to assign, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        for (uint32_t i = 0; i < ARRAYSIZE(info.planes); i++) {
            if (NOTNULL(info.planes[i].ptr)) {
                size += info.planes[i].size;
            }
        }
        data = (uint8_t *)Malloc(size);
        if (ISNULL(data)) {
            LOGE(MODULE_COMMON, "Failed to alloc %d bytes.", size);
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        allocated[0] = true;
        allocatedSize = size;
        int64_t offset = 0LL;
        for (uint32_t i = 0; i < ARRAYSIZE(info.planes); i++) {
            if (NOTNULL(info.planes[i].ptr)) {
                planes[i].ptr = data + offset;
                offset += info.planes[i].size;
            } else {
                planes[i].ptr = nullptr;
                planes[i].w = planes[i].h = planes[i].stride = planes[i].scanline = 0;
                planes[i].size = 0LL;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (copy) {
            for (uint32_t i = 0; i < ARRAYSIZE(info.planes); i++) {
                if (NOTNULL(info.planes[i].ptr)) {
                    memcpy(planes[i].ptr, info.planes[i].ptr, info.planes[i].size);
                }
            }
        }
    }

    return rc;
}

int32_t FrameInfoEx::alloc(const FrameInfo &info)
{
    return allocAndCopy(info, false);
}

int32_t FrameInfoEx::copy(const FrameInfo &info)
{
    int32_t rc = NO_ERROR;
    bool alloc = false;

    if (ISNULL(planes[0].ptr) &&
        ISZERO(planes[0].size)) {
        alloc = true;
    }

    if (alloc) {
        rc = allocAndCopy(info, true);
        if (FAILED(rc)) {
            LOGE(MODULE_COMMON, "Failed to alloc and copy, %d", rc);
        }
    }

    if (!alloc) {
        rc = copyOnly(info);
        if (FAILED(rc)) {
            LOGE(MODULE_COMMON, "Failed to copy, %d", rc);
        }
    }

    return rc;
}

int32_t FrameInfoEx::assign(const FrameInfo &info)
{
    ts = info.ts;
    format = info.format;
    allocatedSize = 0LL;
    for (int32_t i = 0; i < MAX_PLANE_SUPPORT; i++) {
        planes[i].ptr = info.planes[i].ptr;
        planes[i].w = info.planes[i].w;
        planes[i].h = info.planes[i].h;
        planes[i].stride = info.planes[i].stride;
        planes[i].scanline = info.planes[i].scanline;
        planes[i].size = info.planes[i].size;
        allocated[i] = false;
    }

    return NO_ERROR;
}

int32_t FrameInfoEx::copyOnly(const FrameInfo &info)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < MAX_PLANE_SUPPORT; i++) {
            if (NOTNULL(info.planes[i].ptr)) {
                if (planes[i].size < info.planes[i].size) {
                    LOGE(MODULE_COMMON, "Plane[%d] size insufficient, %d/%d",
                        i, planes[i].size, info.planes[i].size);
                    rc = NO_MEMORY;
                    break;
                } else {
                    // Input frame padding might differ with output
                    if (!isSolidFrameSize(info.format)) {
                        memcpy(planes[i].ptr, info.planes[i].ptr, info.planes[i].size);
                    } else {
                        int64_t inputoffset = 0LL;
                        int64_t outputoffset = 0LL;
                        for (int32_t j = 0; j < info.planes[i].h; j++) {
                            memcpy((uint8_t *)planes[i].ptr + outputoffset,
                                (uint8_t *)info.planes[i].ptr + inputoffset,
                                info.planes[i].w);
                            inputoffset += info.planes[i].stride;
                            outputoffset += planes[i].stride;
                        }
                    }
                }
            } else {
                planes[i].ptr = nullptr;
                planes[i].w = planes[i].h = 0;
                planes[i].stride = planes[i].scanline = 0;
                planes[i].size = 0;
                allocated[i] = false;
            }
        }
    }

    if (SUCCEED(rc)) {
        ts = info.ts;
        format = info.format;
        allocatedSize = 0LL;
    }

    return rc;
}

int32_t FrameInfoEx::copy(const FrameInfoEx &info)
{
    const FrameInfo &i = info;
    return copy(i);
}

int32_t FrameInfoEx::alloc(const FrameInfoEx &info)
{
    const FrameInfo &i = info;
    return alloc(i);
}

int32_t FrameInfoEx::assign(const FrameInfoEx &info)
{
    const FrameInfo &i = info;
    return assign(i);
}

int32_t FrameInfoEx::copyOnly(const FrameInfoEx &info)
{
    const FrameInfo &i = info;
    return copyOnly(i);
}

int32_t FrameInfoEx::copyAutoExpand(const FrameInfoEx &info)
{
    int32_t rc = NO_ERROR;
    bool expandRequired = false;

    if (SUCCEED(rc)) {
        for (uint32_t i = 0; i < ARRAYSIZE(info.planes); i++) {
            if (info.planes[i].size > planes[i].size) {
                expandRequired = true;
                break;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (expandRequired && !haveControl()) {
            LOGE(MODULE_COMMON, "Expand required but memory doesn't have control.");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        if (expandRequired) {
            release();
            rc = allocAndCopy(info, true);
            if (FAILED(rc)) {
                LOGE(MODULE_COMMON, "Failed to alloc copy by given frame, %d", rc);
            }
        } else {
            rc = copyOnly(info);
            if (FAILED(rc)) {
                LOGE(MODULE_COMMON, "Failed to copy by given frame, %d", rc);
            }
        }
    }

    return rc;
}

bool FrameInfoEx::haveControl()
{
    bool result = false;

    for (uint32_t i = 0; i < ARRAYSIZE(allocated); i++) {
        if (allocated[i]) {
            result = true;
            if (i > 0) {
                LOGW(MODULE_COMMON, "Not considerd such situation, "
                    "plane[0] not allocated, but plane[%d] allocated, "
                    "might cause problem later.", i);
            }
            break;
        }
    }

    return result;
}

int32_t FrameInfoEx::gainControl(FrameInfoEx &info)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!info.haveControl()) {
            LOGE(MODULE_COMMON, "rhs don't have control ability.");
            rc = NOT_EXIST;
        }
    }

    if (SUCCEED(rc)) {
        if (haveControl()) {
            LOGE(MODULE_COMMON, "this also have control ability, release first.");
            rc = ALREADY_EXISTS;
        }
    }

    if (SUCCEED(rc)) {
        for (uint32_t i = 0; i < ARRAYSIZE(allocated); i++) {
            allocated[i] = info.allocated[i];
            info.allocated[i] = false;
        }
        allocatedSize = info.allocatedSize;
        info.allocatedSize = 0LL;
    }

    return rc;
}

int32_t FrameInfoEx::releaseControlTo(FrameInfoEx &info)
{
    return info.gainControl(*this);
}

int32_t FrameInfoEx::assignInfoAndControl(FrameInfoEx &info)
{
    assign(info);
    for (int32_t i = 0; i < MAX_PLANE_SUPPORT; i++) {
        allocated[i] = info.allocated[i];
        info.allocated[i] = false;
    }

    return NO_ERROR;
}

void FrameInfoEx::overrideFramePtrManuallyInDanger(void *ptr)
{
    if (haveControl()) {
        release();
    }
    planes[0].ptr = ptr;
}

FrameInfoEx::FrameInfoEx() :
    FrameInfo(),
    allocatedSize(0LL)
{
    for (int32_t i = 0; i < MAX_PLANE_SUPPORT; i++) {
        allocated[i] = false;
    }
}

FrameInfoEx::FrameInfoEx(const FrameInfo &rhs) :
    allocatedSize(0LL)
{
    assign(rhs);
}

FrameInfoEx::FrameInfoEx(const FrameInfoEx &rhs) :
    allocatedSize(0LL)
{
    assign(rhs);
}

FrameInfoEx &FrameInfoEx::operator=(const FrameInfo &rhs)
{
    if (this != &rhs) {
        allocatedSize = 0LL;
        assign(rhs);
    }

    return *this;
}

FrameInfoEx &FrameInfoEx::operator=(const FrameInfoEx &rhs)
{
    if (this != &rhs) {
        allocatedSize = 0LL;
        assign(rhs);
    }

    return *this;
}

FrameInfoEx::~FrameInfoEx()
{
    release();
}

int32_t FrameInfoEx::release()
{
    for (int32_t i = 0; i < MAX_PLANE_SUPPORT; i++) {
        if (allocated[i]) {
            SECURE_FREE(planes[i].ptr);
        }
        allocated[i] = false;
        planes[i].w = planes[i].h = 0;
        planes[i].stride = planes[i].scanline = 0;
        planes[i].size = 0;
    }
    allocatedSize = 0LL;

    return NO_ERROR;
}


};
