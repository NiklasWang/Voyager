#ifndef _VOYAGER_INTERFACE_INTERNAL_H_
#define _VOYAGER_INTERFACE_INTERNAL_H_

#include "ServerIntf.h"

namespace voyager {

bool checkValid(RequestType type);

RequestType getValidType(RequestType type);

const char *getRequestName(RequestType type);

};

#endif
