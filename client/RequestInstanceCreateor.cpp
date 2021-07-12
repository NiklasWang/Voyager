#include "ClientCore.h"

#include "DataClient.h"
#include "FdClient.h"
#include "FrameClient.h"
#include "EventClient.h"

namespace voyager {

RequestHandler *ClientCore::createHandler(RequestType type, const std::string &name)
{
    RequestHandler *request = nullptr;

    switch (type) {
        case DATA: {
            request = new DataClient(type, name);
        } break;
        case FD: {
            request = new FdClient(type, name);
        } break;
        case FRAME: {
            request = new FrameClient(type, name);
        } break;
        case EVENT: {
            request = new EventClient(type, name);
        } break;
        default: {
            LOGE(mModule, "Invalid request type %d", type);
        } break;
    };

    return request;
}

};
