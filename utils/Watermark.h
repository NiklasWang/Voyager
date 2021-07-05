#ifndef _PANDORA_WATER_MARK_BMP_H_
#define _PANDORA_WATER_MARK_BMP_H_

#include <stdint.h>

namespace pandora {

int32_t getWaterMark(int32_t id, uint8_t **watermark, uint64_t *size);

int32_t readWaterMark(int32_t id, uint8_t **watermark, uint64_t *size);

};

#endif
