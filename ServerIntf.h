#ifndef _VOYAGER_SERVER_INTF_H_
#define _VOYAGER_SERVER_INTF_H_

#include "Data.h"

namespace voyager {

typedef int32_t (*RequestCbFunc)(RequestType type, int32_t id, void *header, void *dat);
typedef int32_t (*EventCbFunc)(int32_t event, int32_t arg1, int32_t arg2);
typedef int32_t (*DataCbFunc)(int32_t type, void *data, int32_t size);

class ServerIntf {
public:
    virtual int32_t request(RequestType type) = 0;
    virtual int32_t abort(RequestType type) = 0;
    virtual int32_t enqueue(RequestType type, int32_t id) = 0;

    virtual int32_t setCallback(RequestCbFunc requestCb) = 0;
    virtual int32_t setCallback(EventCbFunc eventCb) = 0;
    virtual int32_t setCallback(DataCbFunc dataCb) = 0;

public:
    virtual ~ServerIntf() {}
};

};

#endif
