#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>

#include "Watermark.h"
#include "Logs.h"
#include "Modules.h"
#include "Memory.h"

#define WATERMARK_INTERNAL_0_PATH "../resources/product.watermark.1220x240.dat"

#define WATERMARK_BMP_TYPE_0_PATH "../resources/Pandora.logo.bmp.832x192_24bit.bmp"
#define WATERMARK_BMP_TYPE_1_PATH "../resources/Pandora.logo.bmp.3708x833_24bit.bmp"
#define WATERMARK_BMP_TYPE_2_PATH "../resources/Pandora.logo.bmp.148x640_24bit.bmp"

namespace pandora {

static uint8_t gInternalWatermark[] = {
    #include WATERMARK_INTERNAL_0_PATH
};

static const char *const gBmpWatermark[] = {
    WATERMARK_BMP_TYPE_0_PATH,
    WATERMARK_BMP_TYPE_1_PATH,
    WATERMARK_BMP_TYPE_2_PATH
};

int32_t getWaterMark(int32_t id, uint8_t **watermark, uint64_t *size)
{
    assert(watermark != NULL);
    assert(size != NULL);

    if (id == 0) {
        *watermark = gInternalWatermark;
        *size = sizeof(gInternalWatermark);
    }

    return NO_ERROR;
}

int32_t readWaterMark(int32_t id, uint8_t **watermark, uint64_t *size)
{
    int32_t rc = NO_ERROR;
    struct stat info;
    int fd = -1;
    uint64_t readSize = 0;
    uint8_t *buf = NULL;
    char fileName[PATH_MAX];

    if (id >0 && id < static_cast<int32_t>(ARRAYSIZE(gBmpWatermark))) {
        snprintf(fileName, sizeof(fileName), "%s",
            gBmpWatermark[id]);
    } else {
        rc = NOT_SUPPORTED;
    }

    if (SUCCEED(rc)) {
        if (stat(fileName, &info) != 0) {
            LOGE(MODULE_UTILS, "stat file %s error, %s",
                fileName, strerror(errno));
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        *size = info.st_size;
        fd = open(fileName, O_RDONLY);
        if (fd < 0) {
            LOGE(MODULE_UTILS, "Open watermark failed, %s",
                strerror(errno));
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        buf = (uint8_t *)Malloc(*size);
        if (ISNULL(buf)) {
            LOGE(MODULE_UTILS, "Failed to alloc %d bytes for "
                "watermark", *size);
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        readSize = read(fd, buf, *size);
        if (readSize != *size) {
            LOGE(MODULE_UTILS, "Failed to read watermark, "
                "%lld/%lld bytes, %s", readSize, *size, strerror(errno));
            Free(buf);
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc) || FAILED(rc)) {
        if (SUCCEED(rc)) {
            *watermark = buf;
        }
        if (fd > 0) {
            close(fd);
        }
    }

    return rc;
}

};

