#include "ServerCore.h"

#include "DataServer.h"
#include "FdServer.h"
#include "FrameServer.h"
#include "EventServer.h"

namespace voyager {

RequestHandler *ServerCore::createHandler(RequestType type)
{
    RequestHandler *request = nullptr;

    switch (type) {
        case DATA: {
            request = new DataServer(this);
        } break;
        case FD: {
            request = new FdServer(this);
        } break;
        case FRAME: {
            request = new FrameServer(this);
        } break;
        case EVENT: {
            request = new EventServer(this);
        } break;
        default: {
            LOGE(mModule, "Invalid request type %d", type);
        } break;
    };

    return request;
}

};
