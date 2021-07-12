#ifndef _VOYAGER_CLIENT_H_
#define _VOYAGER_CLIENT_H_

#include <string>

#include "ClientIntf.h"

namespace voyager {

class ClientCore;

class Client :
    public ClientIntf
{
public:

    virtual int32_t send(void *dat, int64_t len) override;
    virtual int32_t send(int32_t fd, int64_t len) override;
    virtual int32_t send(void *dat, int64_t len, int32_t format) override;
    virtual int32_t send(int32_t event, int32_t arg1, int32_t arg2) override;
    virtual bool    requested(RequestType type) override;

public:
    Client(const char *name);
    virtual ~Client();

private:
    Client(const Client &rhs);
    Client &operator=(const Client &rhs);

private:
    ClientCore *mCore;
    std::string mName;
};

};

#endif
