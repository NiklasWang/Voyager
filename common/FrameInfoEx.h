#ifndef _FRAME_INFO_EX_H_
#define _FRAME_INFO_EX_H_

#include "Interface.h"

namespace pandora {

struct FrameInfoEx :
    public FrameInfo
{
public:
    int32_t copy(const FrameInfo &info);
    int32_t copy(const FrameInfoEx &info);
    int32_t alloc(const FrameInfo &info);
    int32_t alloc(const FrameInfoEx &info);
    int32_t assign(const FrameInfo &info);
    int32_t assign(const FrameInfoEx &info);
    int32_t copyOnly(const FrameInfo &info);
    int32_t copyOnly(const FrameInfoEx &info);
    int32_t copyAutoExpand(const FrameInfoEx &info);
    int32_t assignInfoAndControl(FrameInfoEx &info);
    bool    haveControl();
    int32_t gainControl(FrameInfoEx &info);
    int32_t releaseControlTo(FrameInfoEx &info);
    void    overrideFramePtrManuallyInDanger(void *ptr);
    int32_t release();

public:
    FrameInfoEx();
    FrameInfoEx(const FrameInfo &rhs);
    FrameInfoEx(const FrameInfoEx &rhs);
    FrameInfoEx &operator=(const FrameInfo &rhs);
    FrameInfoEx &operator=(const FrameInfoEx &rhs);
    virtual ~FrameInfoEx();

private:
    int32_t allocAndCopy(const FrameInfo &info, bool copy);

public:
    bool    allocated[MAX_PLANE_SUPPORT];
    int64_t allocatedSize;
};

};

#endif
