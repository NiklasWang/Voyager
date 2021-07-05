#ifndef _PANDORA_INTERFACE__
#define _PANDORA_INTERFACE__

#include <stdint.h>
#include <stddef.h>

namespace pandora {

enum Format {
    FORMAT_YUV_NV21_SEMI_PLANAR,
    FORMAT_YUV_NV12_SEMI_PLANAR,
    FORMAT_YUV_PLANAR,
    FORMAT_YUV_MONO,
    FORMAT_JPEG,
    FORMAT_HEIF,
    FORMAT_BAYER,
    FORMAT_TEXTURE,
    FORMAT_MAX_INVALID,
};

enum Usecase {
    USECASE_ANY,
    USECASE_PREVIEW,
    USECASE_SNAPSHOT,
    USECASE_VIDEO,
    USECASE_RENDER,
    USECASE_TEST,
    USECASE_MAX_INVALID,
};

enum Source {
    SOURCE_ANY,
    SOURCE_CAM_BACK_MAIN,
    SOURCE_CAM_BACK_TELE,
    SOURCE_CAM_BACK_WIDE,
    SOURCE_CAM_BACK_CUSTOM0,
    SOURCE_CAM_FRONT_MAIN,
    SOURCE_CAM_FRONT_TELE,
    SOURCE_CAM_FRONT_WIDE,
    SOURCE_CAM_FRONT_CUSTOM0,
    SOURCE_SIMULATOR,
    SOURCE_CUSTOM0,
    SOURCE_MAX_INVALID
};

struct LegacyFrameInfo {
    void   *ptr;
    int32_t w;
    int32_t h;
    int32_t stride;
    int32_t scanline;
    size_t  size;
    Format  format;
    int64_t ts;   // ms * 1e6 from 01/01/1970 00:00:00

public:
    LegacyFrameInfo();
    bool valid();
};

#define MAX_PLANE_SUPPORT 5

struct FrameInfo {
    Format  format;
    int64_t ts;
    struct Plane {
        void   *ptr;
        int32_t w;
        int32_t h;
        int32_t stride;
        int32_t scanline;
        int64_t size;
    } planes[MAX_PLANE_SUPPORT];

public:
    FrameInfo();
    FrameInfo(const LegacyFrameInfo &legacy);
    void release();
    bool valid() const;
};


class Interface {
public:
    virtual const char *version() = 0;

    virtual int32_t request(void *tags, Source source = SOURCE_ANY) = 0;
    virtual int32_t update(void *metadata, Source source = SOURCE_ANY) = 0;

    virtual int32_t process(const FrameInfo &frame,
        Source source = SOURCE_ANY, Usecase usecase = USECASE_ANY) = 0;
    virtual int32_t process(const FrameInfo &in, const FrameInfo &out,
        Source source = SOURCE_ANY, Usecase usecase = USECASE_ANY) = 0;

    virtual int32_t process(const LegacyFrameInfo &frame,
        Source source = SOURCE_ANY, Usecase usecase = USECASE_ANY) = 0;
    virtual int32_t process(const LegacyFrameInfo &in, const LegacyFrameInfo &out,
        Source source = SOURCE_ANY, Usecase usecase = USECASE_ANY) = 0;

    virtual ~Interface() {}
};

};

#endif
