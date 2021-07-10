#ifndef _VOYAGER_SERVER_H_
#define _VOYAGER_SERVER_H_

#include <string>

#include "ServerIntf.h"

namespace voyager {

class ServerImpl;

class Server :
    public ServerIntf {
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
    Server(const char *name, bool enableOverallControl = true);
    virtual ~Server();

private:
    Server(const Server &rhs);
    Server &operator=(const Server &rhs);

private:
    std::string mName;
    bool        mEnableOverallControl;
    ServerImpl *mImpl;
};

};

#endif
