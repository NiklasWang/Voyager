#ifndef _PANDORA_INTERFACE_INTERNAL_H_
#define _PANDORA_INTERFACE_INTERNAL_H_

#include "Interface.h"

namespace pandora {

bool checkValid(Format format);
bool checkValid(Usecase type);
bool checkValid(Source source);


Format  getValidType(Format format);
Usecase getValidType(Usecase usecase);
Source  getValidType(Source source);


const char *getFormatName(Format format);
const char *getUsecaseName(Usecase type);
const char *getSourceName(Source source);


int32_t GetNumberOfPlanes(Format format);

bool isYuvSeries(Format format);

bool isSolidFrameSize(Format format);

const char * const getExtension(Format format);

};

#endif
