#ifndef _REQUEST_HANDLER_CLIENT_H_
#define _REQUEST_HANDLER_CLIENT_H_

#include "Common.h"
#include "ClientIntf.h"
#include "BufferMgr.h"
#include "SocketClientStateMachine.h"

namespace voyager {

class RequestHandler :
    public ClientIntf,
    virtual public Identifier,
    virtual public noncopyable {
public:

    virtual int32_t send(void *dat, int64_t len) override;
    virtual int32_t send(int32_t fd, int64_t len) override;
    virtual int32_t send(int32_t fd, int64_t len, int32_t format) override;
    virtual int32_t send(int32_t event, int32_t arg1, int32_t arg2) override;
    virtual bool    requested(RequestType type) override;
    const char *getName();
    RequestType getType();

public:
    RequestHandler(RequestType type, const std::string &name);
    virtual ~RequestHandler();
    int32_t construct();
    int32_t destruct();

private:
    int32_t connectServer();
    int32_t sendPrivateMsg(RequestType type, int64_t len);
    int32_t sendPrivateMsg(RequestType type, int64_t len, int32_t format);
    int32_t sendPrivateMsg(int32_t event, int32_t arg1, int32_t arg2);
    int32_t waitServerReply();
    int32_t allocFdAndCopy(void *dat, int64_t len, int32_t &fd);
    int32_t sendServerFdAndWaitReply(int32_t fd);
    bool    validFd(int32_t fd);

protected:
    bool        mConstructed;
    RequestType mType;
    std::string mName;
    bool        mConnected;
    SocketClientStateMachine mSC;
    BufferMgr   mBufMgr;
};

};

#endif
