#ifndef _VOYAGER_SERVER_INTF_H_
#define _VOYAGER_SERVER_INTF_H_

#include "ClientIntf.h"

namespace voyager {

typedef int32_t (*DataCbFunc)(void *dat, int64_t len);
typedef int32_t (*FdCbFunc)(int32_t fd, int64_t len);
typedef int32_t (*FrameCbFunc)(void *dat, int64_t len, int32_t format);
typedef int32_t (*EventCbFunc)(int32_t event, int32_t arg1, int32_t arg2);


class ServerIntf {
public:
    virtual int32_t request(DataCbFunc dataCbFunc) = 0;
    virtual int32_t enqueue(void *dat) = 0;
    virtual int32_t request(FdCbFunc fdCbFunc) = 0;
    virtual int32_t enqueue(int32_t fd) = 0;
    virtual int32_t request(FrameCbFunc frameCbFunc) = 0;
    virtual int32_t enqueue(void *dat, int32_t format) = 0;
    virtual int32_t request(EventCbFunc eventCbFunc) = 0;
    virtual int32_t cancel(RequestType type) = 0;

public:
    virtual ~ServerIntf();
};

};

#endif
