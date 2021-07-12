#ifndef _REQUEST_HANDLER_H_
#define _REQUEST_HANDLER_H_

#include "Common.h"
#include "RequestHandlerIntf.h"
#include "ServerIntf.h"
#include "CallbackThread.h"
#include "ThreadPoolEx.h"
#include "SocketServerStateMachine.h"

namespace voyager {

class RequestHandler :
    public RequestHandlerIntf,
    public Identifier,
    public noncopyable {
public:

    virtual int32_t onClientReady(int32_t clientfd, const std::string &privateMsg) override;
    virtual int32_t enqueue(int32_t fd) override;
    virtual int32_t enqueue(void *dat) = 0;
    virtual int32_t enqueue(void *dat, int32_t format) = 0;
    virtual int32_t abort() override;

    virtual RequestType getType() override;
    virtual const char *getName() override;

public:
    virtual int32_t construct() override;
    virtual int32_t destruct() override;
    RequestHandler(RequestType type, const char *name, CallbackIntf *cb);
    virtual ~RequestHandler();

protected:
    virtual int32_t onClientSent(int32_t fd, const std::string &privateMsg) = 0;
    virtual int32_t startServerLoop(int32_t clientfd, const std::string &privateMsg);
    virtual int32_t exitServerLoop();
    bool checkFdExists(int32_t fd);
    bool checkFdExists(void *ptr);
    void addFdRecord(int32_t fd);
    void addFdRecord(int32_t fd, void *ptr);
    int32_t removeFdRecord(int32_t fd);
    int32_t removeFdRecord(void *ptr);

protected:
    bool          mConstructed;
    std::string   mName;
    RequestType   mType;
    CallbackIntf *mCb;
    ThreadPoolEx *mThreads;
    SocketServerStateMachine  mSSSM;
    BufferMgr     mBufMgr;
    std::map<int32_t, void *> mFds;
};

};

#endif

