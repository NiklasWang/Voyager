#ifndef _SERVER_CLIENT_CONTROL_H_
#define _SERVER_CLIENT_CONTROL_H_

#include "common.h"
#include "configuration.h"
#include "SiriusServerIntf.h"

namespace sirius {

class ServerClientControl :
    public noncopyable {
public:
    int32_t setRequest(RequestType type, bool required = false);
    bool requested(RequestType type);
    int32_t getUsedMem(RequestType type, int32_t *fd);
    int32_t setMemStatus(RequestType type, int32_t fd, bool fresh = false);
    int32_t getMemStatus(RequestType type, int32_t fd, bool *fresh);
    int32_t setMemSize(RequestType type, int32_t size);
    int32_t getMemSize(RequestType type, int32_t *size);
    int32_t addMemory(RequestType type, int32_t clientfd, bool fresh = false);
    int32_t getHeader(Header &header);
    int32_t setHeader(Header &header);
    int32_t resetCtrlMem(RequestType type);

public:
    int32_t getTotoalSize();
    int32_t init(void *mem, int32_t size, bool init = false);
    bool    ready();

public:
    ServerClientControl();
    virtual ~ServerClientControl();

private:
    struct RequestMemory;
    int32_t findClientMemory(RequestType type,
        int32_t fd, RequestMemory **mem);

private:
    enum MemoryStatus {
        MEMORY_STAT_USED,
        MEMORY_STAT_FRESH,
    };

    struct RequestMemory {
        int32_t fd;
        MemoryStatus stat;
        int64_t ts;
        pthread_mutex_t l;
    };

    struct MemoryBlock {
        bool          requested;
        int32_t       memNum;
        int32_t       size;
        RequestMemory mems[REQUEST_HANDLER_MAX_MEMORY_NUM];
    };

    struct ControlMemory {
        Header      header;
        MemoryBlock request[REQUEST_TYPE_MAX_INVALID];
    };

private:
    ModuleType     mModule;
    ControlMemory *mCtl;
    bool           mReady;
};

};

#endif
