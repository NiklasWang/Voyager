#ifndef _VOYAGER_SERVER_H_
#define _VOYAGER_SERVER_H_

#include "ServerIntf.h"

namespace voyager {

class ServerImpl;

class Server :
    public ServerIntf {
public:
    int32_t request(RequestType type);
    int32_t abort(RequestType type);
    int32_t enqueue(RequestType type, int32_t id);

    int32_t setCallback(RequestCbFunc requestCb);
    int32_t setCallback(EventCbFunc eventCb);
    int32_t setCallback(DataCbFunc dataCb);

public:
    Server();
    virtual ~Server();

private:
    Server(const Server &rhs);
    Server &operator=(const Server &rhs);

private:
    ServerImpl *mImpl;
};

};

#endif
