#include "Common.h"
#include "InterfaceImpl.h"
#include "Logs.h"
#include "Memory.h"

namespace pandora {

static const char * const gFormatStr[] = {
    [FORMAT_YUV_NV21_SEMI_PLANAR] = "nv21sp",
    [FORMAT_YUV_NV12_SEMI_PLANAR] = "nv12sp",
    [FORMAT_YUV_PLANAR]           = "yuv",
    [FORMAT_YUV_MONO]             = "mono",
    [FORMAT_JPEG]                 = "jpg",
    [FORMAT_HEIF]                 = "heif",
    [FORMAT_BAYER]                = "bayer",
    [FORMAT_TEXTURE]              = "texture",
    [FORMAT_MAX_INVALID]          = "MAX FORMAT_MAX_INVALID",
};

bool checkValid(Format format)
{
    bool rc = false;

    if (format >= 0 && format < FORMAT_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

Format getValidType(Format format)
{
    return checkValid(format) ? format : FORMAT_MAX_INVALID;
}

const char *getFormatName(Format format)
{
    return gFormatStr[getValidType(format)];
}


static const char * const gUsecaseStr[] = {
    [USECASE_ANY]         = "any usecase",
    [USECASE_PREVIEW]     = "preview",
    [USECASE_SNAPSHOT]    = "snapshot",
    [USECASE_VIDEO]       = "video",
    [USECASE_RENDER]      = "render",
    [USECASE_TEST]        = "test usecase",
    [USECASE_MAX_INVALID] = "USECASE_MAX_INVALID",
};

bool checkValid(Usecase type)
{
    bool rc = false;

    if (type >= 0 && type < USECASE_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

Usecase getValidType(Usecase type)
{
    return checkValid(type) ? type : USECASE_MAX_INVALID;
}

const char *getUsecaseName(Usecase type)
{
    return gUsecaseStr[getValidType(type)];
}


static const char * const gSourceStr[] = {
    [SOURCE_ANY]               = "any source",
    [SOURCE_CAM_BACK_MAIN]     = "back main cam",
    [SOURCE_CAM_BACK_TELE]     = "back tele cam",
    [SOURCE_CAM_BACK_WIDE]     = "back wide cam",
    [SOURCE_CAM_BACK_CUSTOM0]  = "back custom0",
    [SOURCE_CAM_FRONT_MAIN]    = "front main cam",
    [SOURCE_CAM_FRONT_TELE]    = "front tele cam",
    [SOURCE_CAM_FRONT_WIDE]    = "front wide cam",
    [SOURCE_CAM_FRONT_CUSTOM0] = "front custom0",
    [SOURCE_SIMULATOR]         = "simulator source",
    [SOURCE_CUSTOM0]           = "custom0 source",
    [SOURCE_MAX_INVALID]       = "SOURCE_MAX_INVALID",
};

bool checkValid(Source source)
{
    bool rc = false;

    if (source >= 0 && source < SOURCE_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

Source getValidType(Source source)
{
    return checkValid(source) ? source : SOURCE_MAX_INVALID;
}

const char *getSourceName(Source source)
{
    return gSourceStr[getValidType(source)];
}


int32_t GetNumberOfPlanes(Format format)
{
    int32_t result = -1;

    switch(format) {
        case FORMAT_YUV_NV21_SEMI_PLANAR:
        case FORMAT_YUV_NV12_SEMI_PLANAR:
            result = 2;
            break;
        case FORMAT_YUV_PLANAR:
            result = 3;
            break;
        case FORMAT_YUV_MONO:
        case FORMAT_JPEG:
        case FORMAT_HEIF:
        case FORMAT_BAYER:
        case FORMAT_TEXTURE:
            result = 1;
            break;
        case FORMAT_MAX_INVALID:
        default:
            result = -1;
            break;
    };

    return result;
}

bool isYuvSeries(Format format)
{
    bool result = false;

    switch (format) {
        case FORMAT_YUV_NV21_SEMI_PLANAR:
        case FORMAT_YUV_NV12_SEMI_PLANAR:
        case FORMAT_YUV_PLANAR:
        case FORMAT_YUV_MONO: {
            result = true;
        }; break;
        case FORMAT_JPEG:
        case FORMAT_HEIF:
        case FORMAT_BAYER:
        case FORMAT_TEXTURE:
        case FORMAT_MAX_INVALID:
        default: {
            result = false;
        }; break;
    }

    return result;
}

bool isSolidFrameSize(Format format)
{
    return isYuvSeries(format);
}

LegacyFrameInfo::LegacyFrameInfo() :
    ptr(nullptr),
    w(0),
    h(0),
    stride(0),
    scanline(0),
    size(0),
    format(FORMAT_YUV_NV21_SEMI_PLANAR),
    ts(0ULL)
{
}

bool LegacyFrameInfo::valid()
{
    bool result = true;

    result &= NOTNULL(ptr);
    result &= NOTZERO(w);
    result &= NOTZERO(h);
    result &= NOTZERO(stride);
    result &= NOTZERO(scanline);
    result &= NOTZERO(size);
    result &= NOTZERO(ts);

    return result;
}

FrameInfo::FrameInfo() :
    ts(0ULL)
{
    for (int32_t i = 0; i < MAX_PLANE_SUPPORT; i++) {
        planes[i].ptr = nullptr;
        planes[i].w = planes[i].h = 0;
        planes[i].stride = planes[i].scanline = 0;
        planes[i].size = 0;
    }
}

void FrameInfo::release()
{
    ts = 0ULL;
    for (int32_t i = 0; i < MAX_PLANE_SUPPORT; i++) {
        if (NOTNULL(planes[i].ptr)) {
            SECURE_FREE(planes[i].ptr);
            planes[i].w = planes[i].h = 0;
            planes[i].stride = planes[i].scanline = 0;
            planes[i].size = 0;
        }
    }
}

bool FrameInfo::valid() const
{
    bool result = true;
    int32_t num = GetNumberOfPlanes(format);

    if (num <= 0) {
        result = false;
    }

    result &= NOTZERO(ts);
    for (int32_t i = 0; i < num; i++) {
        result &= NOTNULL(planes[num].ptr);
        result &= NOTZERO(planes[num].w);
        result &= NOTZERO(planes[num].h);
        result &= NOTZERO(planes[num].stride);
        result &= NOTZERO(planes[num].scanline);
        result &= NOTZERO(planes[num].size);
    }

    return result;
}

FrameInfo::FrameInfo(const LegacyFrameInfo &legacy)
{
    if (checkValid(legacy.format)) {
        format = legacy.format;
        ts     = legacy.ts;
        if (legacy.format == FORMAT_YUV_NV21_SEMI_PLANAR ||
            legacy.format == FORMAT_YUV_NV12_SEMI_PLANAR) {
            planes[0].ptr = legacy.ptr;
            planes[0].w   = legacy.w;
            planes[0].h   = legacy.h;
            planes[0].stride   = legacy.stride;
            planes[0].scanline = legacy.scanline;
            planes[0].size     = planes[0].stride * planes[0].scanline;
            planes[1].ptr = (uint8_t *)planes[0].ptr + planes[0].size;
            planes[1].w   = planes[0].w;
            planes[1].h   = planes[0].h / 2;
            planes[1].stride   = planes[0].stride;
            planes[1].scanline = planes[0].scanline / 2;
            planes[1].size     = planes[1].stride * planes[1].scanline;
            LOGD(MODULE_COMMON, "plane[1] scanline might not correct.");
        } else if (legacy.format == FORMAT_JPEG ||
            legacy.format == FORMAT_HEIF ||
            legacy.format == FORMAT_BAYER) {
            planes[0].ptr = legacy.ptr;
            planes[0].w   = legacy.w;
            planes[0].h   = legacy.h;
            planes[0].stride   = legacy.stride;
            planes[0].scanline = legacy.scanline;
            planes[0].size     = legacy.size;
        } else if (legacy.format == FORMAT_YUV_PLANAR ||
            legacy.format == FORMAT_YUV_MONO) {
            planes[0].ptr = legacy.ptr;
            planes[0].w   = legacy.w;
            planes[0].h   = legacy.h;
            planes[0].stride   = legacy.stride;
            planes[0].scanline = legacy.scanline;
            planes[0].size     = planes[0].stride * planes[0].scanline;
            planes[1].ptr = (uint8_t *)planes[0].ptr + planes[0].size;
            planes[1].w   = planes[0].w / 2;
            planes[1].h   = planes[0].h / 2;
            planes[1].stride   = planes[0].stride / 2;
            planes[1].scanline = planes[0].scanline / 2;
            planes[1].size     = planes[1].stride * planes[1].scanline;
            planes[2].ptr = (uint8_t *)planes[1].ptr + planes[1].size;
            planes[2].w   = planes[0].w / 2;
            planes[2].h   = planes[0].h / 2;
            planes[2].stride   = planes[0].stride / 2;
            planes[2].scanline = planes[0].scanline / 2;
            planes[2].size     = planes[2].stride * planes[2].scanline;
            LOGD(MODULE_COMMON, "plane[1] and plane[2] stride, scanline might not correct.");
        }
        for (int32_t i = GetNumberOfPlanes(legacy.format);
            i < (int32_t)ARRAYSIZE(planes); i++) {
            planes[i].ptr    = nullptr;
            planes[i].w      = planes[i].h = 0;
            planes[i].stride = planes[i].scanline = 0;
            planes[i].size   = 0LL;
        }
    }
}

const char * const getExtension(Format format)
{
    static const char * const name[] = {
        [FORMAT_YUV_NV21_SEMI_PLANAR] = "nv21",
        [FORMAT_YUV_NV12_SEMI_PLANAR] = "nv12",
        [FORMAT_YUV_PLANAR]           = "yuv",
        [FORMAT_YUV_MONO]             = "mono",
        [FORMAT_JPEG]                 = "jpg",
        [FORMAT_HEIF]                 = "heif",
        [FORMAT_BAYER]                = "bayer",
        [FORMAT_TEXTURE]              = "texture",
        [FORMAT_MAX_INVALID]          = "error",
    };

    return name[getValidType(format)];
}


};
