#ifndef _OBJECT_BUFFER_HANDLER_H_
#define _OBJECT_BUFFER_HANDLER_H_

#include <stdint.h>

class ObjectBufferHandler {

public:
    virtual int32_t dump() = 0;
    virtual int32_t dump(const char *file, const char *func, const int32_t line) = 0;

    virtual int32_t flush() = 0;

    virtual bool full() = 0;

    virtual ~ObjectBufferHandler() {}
};


#endif
