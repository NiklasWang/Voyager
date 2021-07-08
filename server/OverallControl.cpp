#include "OverallControl.h"

namespace voyager {

#define CHECK_SET_LAYOUT() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (ISNULL(mLayout)) { \
            __rc = NOT_INITED; \
        } \
        __rc; \
    })

int32_t OverallControl::addServer(const char *path, const char *name, int32_t maxConnection)
{
    int32_t rc = CHECK_SET_LAYOUT();
    int32_t existingIndex = -1;

    if (SUCCEED(rc)) {
        rc = searchServer(path, name, existingIndex);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to search server infos, %d", rc);
        } else {
            if (existingIndex != -1) {
                LOGE(mModule, "Server existed in overall control, %s %s", path, name);
                rc = ALREADY_EXISTS;
            }
        }
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < ARRAYSIZE(mLayout->sockets.server); i++) {
            if (mLayout->sockets.server[i].type ==
                OverallControlLayout::Sockets::Server::Type::SERVER_TYPE_MAX_INVALID) {
                mLayout->sockets.server[i].type =
                    OverallControlLayout::Sockets::Server::Type::SERVER_TYPE_LOCAL;
                strcpy(mLayout->sockets.server[i].address.local.path, path);
                strcpy(mLayout->sockets.server[i].address.local.name, name);
                mLayout->sockets.server[i].currentConnections = 0;
                mLayout->sockets.server[i].maxConnection = maxConnection;
                break;
            }
        }
    }

    return rc;
}

int32_t OverallControl::removeServer(const char *path, const char *name)
{
    int32_t rc = CHECK_SET_LAYOUT();
    int32_t existingIndex = -1;

    if (SUCCEED(rc)) {
        rc = searchServer(path, name, existingIndex);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to search server info, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (existingIndex != -1) {
            mLayout->sockets.server[existingIndex].type ==
                OverallControlLayout::Sockets::Server::Type::SERVER_TYPE_MAX_INVALID;
        } else {
            LOGE(mModule, "Server not existed in overall control, %s %s", path, name);
            rc = NOT_EXIST;
        }
    }

    return rc;
}

int32_t OverallControl::addClient(const char *path, const char *name)
{
    int32_t rc = CHECK_SET_LAYOUT();
    int32_t existingIndex = -1;

    if (SUCCEED(rc)) {
        rc = searchClient(path, name, existingIndex);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to search client info, %d", rc);
        } else {
            if (existingIndex != -1) {
                LOGE(mModule, "Client existed in overall control, %s %s", path, name);
                rc = ALREADY_EXISTS;
            }
        }
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < ARRAYSIZE(mLayout->sockets.client); i++) {
            if (mLayout->sockets.client[i].type ==
                OverallControlLayout::Sockets::Client::Type::CLIENT_TYPE_MAX_INVALID) {
                mLayout->sockets.client[i].type =
                    OverallControlLayout::Sockets::Client::Type::CLIENT_TYPE_LOCAL;
                strcpy(mLayout->sockets.client[i].connect.local.path, path);
                strcpy(mLayout->sockets.client[i].connect.local.name, name);
                break;
            }
        }
    }

    return rc;
}

int32_t OverallControl::removeClient(const char *path, const char *name)
{
    int32_t rc = CHECK_SET_LAYOUT();
    int32_t existingIndex = -1;

    if (SUCCEED(rc)) {
        rc = searchClient(path, name, existingIndex);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to search client info, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (existingIndex != -1) {
            mLayout->sockets.client[existingIndex].type ==
                OverallControlLayout::Sockets::Client::Type::CLIENT_TYPE_MAX_INVALID;
        } else {
            LOGE(mModule, "Client not existed in overall control, %s %s", path, name);
            rc = NOT_EXIST;
        }
    }

    return rc;
}

int32_t OverallControl::addServer(const char *ip, int32_t port, int32_t maxConnection)
{
    int32_t rc = CHECK_SET_LAYOUT();
    int32_t existingIndex = -1;

    if (SUCCEED(rc)) {
        rc = searchServer(ip, port, existingIndex);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to search server infos, %d", rc);
        } else {
            if (existingIndex != -1) {
                LOGE(mModule, "Server existed in overall control, %s %d", ip, port);
                rc = ALREADY_EXISTS;
            }
        }
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < ARRAYSIZE(mLayout->sockets.server); i++) {
            if (mLayout->sockets.server[i].type ==
                OverallControlLayout::Sockets::Server::Type::SERVER_TYPE_MAX_INVALID) {
                mLayout->sockets.server[i].type =
                    OverallControlLayout::Sockets::Server::Type::SERVER_TYPE_IP;
                strcpy(mLayout->sockets.server[i].address.ip.ip, ip);
                mLayout->sockets.server[i].address.ip.port = port;
                mLayout->sockets.server[i].currentConnections = 0;
                mLayout->sockets.server[i].maxConnection = maxConnection;
                break;
            }
        }
    }

    return rc;
}

int32_t OverallControl::removeServer(const char *ip, int32_t port)
{
    int32_t rc = CHECK_SET_LAYOUT();
    int32_t existingIndex = -1;

    if (SUCCEED(rc)) {
        rc = searchServer(ip, port, existingIndex);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to search server info, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (existingIndex != -1) {
            mLayout->sockets.server[existingIndex].type ==
                OverallControlLayout::Sockets::Server::Type::SERVER_TYPE_MAX_INVALID;
        } else {
            LOGE(mModule, "Server not existed in overall control, %s %d", ip, port);
            rc = NOT_EXIST;
        }
    }

    return rc;
}

int32_t OverallControl::addClient(const char *ip, int32_t port)
{
    int32_t rc = CHECK_SET_LAYOUT();
    int32_t existingIndex = -1;

    if (SUCCEED(rc)) {
        rc = searchClient(ip, port, existingIndex);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to search client info, %d", rc);
        } else {
            if (existingIndex != -1) {
                LOGE(mModule, "Client existed in overall control, %s %d", ip, port);
                rc = ALREADY_EXISTS;
            }
        }
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < ARRAYSIZE(mLayout->sockets.client); i++) {
            if (mLayout->sockets.client[i].type ==
                OverallControlLayout::Sockets::Client::Type::CLIENT_TYPE_MAX_INVALID) {
                mLayout->sockets.client[i].type =
                    OverallControlLayout::Sockets::Client::Type::CLIENT_TYPE_IP;
                strcpy(mLayout->sockets.client[i].connect.ip.ip, ip);
                mLayout->sockets.client[i].connect.ip.port = port;
                break;
            }
        }
    }

    return rc;
}

int32_t OverallControl::removeClient(const char *ip, int32_t port)
{
    int32_t rc = CHECK_SET_LAYOUT();
    int32_t existingIndex = -1;

    if (SUCCEED(rc)) {
        rc = searchClient(ip, port, existingIndex);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to search client info, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        if (existingIndex != -1) {
            mLayout->sockets.client[existingIndex].type ==
                OverallControlLayout::Sockets::Client::Type::CLIENT_TYPE_MAX_INVALID;
        } else {
            LOGE(mModule, "Client not existed in overall control, %s %d", ip, port);
            rc = NOT_EXIST;
        }
    }

    return rc;
}

int32_t OverallControl::searchServer(const char *path, const char *name, int32_t &index)
{
    int32_t rc = CHECK_SET_LAYOUT();
    index = -1;

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < ARRAYSIZE(mLayout->sockets.server); i++) {
            if (mLayout->sockets.server[i].type ==
                OverallControlLayout::Sockets::Server::Type::SERVER_TYPE_LOCAL           &&
                COMPARE_SAME_STRING(path, mLayout->sockets.server[i].address.local.path) &&
                COMPARE_SAME_STRING(name, mLayout->sockets.server[i].address.local.name) {
                index = i;
                break;
            }
        }
    }

    return rc;
}

int32_t OverallControl::searchClient(const char *path, const char *name, int32_t &index)
{
    int32_t rc = CHECK_SET_LAYOUT();
    index = -1;

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < ARRAYSIZE(mLayout->sockets.client); i++) {
            if (mLayout->sockets.client[i].type ==
                OverallControlLayout::Sockets::Client::Type::CLIENT_TYPE_LOCAL           &&
                COMPARE_SAME_STRING(path, mLayout->sockets.client[i].address.local.path) &&
                COMPARE_SAME_STRING(name, mLayout->sockets.client[i].address.local.name) {
                index = i;
                break;
            }
        }
    }

    return rc;
}

int32_t OverallControl::searchServer(const char *ip, int32_t port, int32_t &index)
{
    int32_t rc = CHECK_SET_LAYOUT();
    index = -1;

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < ARRAYSIZE(mLayout->sockets.server); i++) {
            if (mLayout->sockets.server[i].type ==
                OverallControlLayout::Sockets::Server::Type::SERVER_TYPE_IP       &&
                COMPARE_SAME_STRING(ip, mLayout->sockets.server[i].address.ip.ip) &&
                port == mLayout->sockets.server[i].address.ip.port) {
                index = i;
                break;
            }
        }
    }

    return rc;
}

int32_t OverallControl::searchClient(const char *ip, int32_t port, int32_t &index)
{
    int32_t rc = CHECK_SET_LAYOUT();
    index = -1;

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < ARRAYSIZE(mLayout->sockets.client); i++) {
            if (mLayout->sockets.client[i].type ==
                OverallControlLayout::Sockets::Client::Type::CLIENT_TYPE_IP       &&
                COMPARE_SAME_STRING(ip, mLayout->sockets.client[i].connect.ip.ip) &&
                port == mLayout->sockets.client[i].connect.ip.port) {
                index = i;
                break;
            }
        }
    }

    return rc;
}

void OverallControl::setLayout(const OverallControlLayout *layout)
{
    ASSERT_LOG(mModule, ISNULL(layout), "Layout can't be nullptr");

    mLayout = layout;

    return;
}

int32_t OverallControl::initLayout()
{
    int32_t rc = CHECK_SET_LAYOUT();

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < ARRAYSIZE(mLayout->sockets.server); i++) {
            mLayout->sockets.server[i].type =
                OverallControlLayout::Sockets::Server::Type::SERVER_TYPE_MAX_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < ARRAYSIZE(mLayout->sockets.client); i++) {
            mLayout->sockets.client[i].type =
                OverallControlLayout::Sockets::Client::Type::CLIENT_TYPE_MAX_INVALID;
        }
    }

    return rc;
}

void OverallControl::dump(const char *prefix)
{
    return;
}

OverallControl::OverallControl() :
    Identifier(MODULE_OVERALL_CONTROL, "OverallControl", "1.0.0"),
    mLayout(nullptr)
{
}
    
OverallControl::~OverallControl()
{
}

};
