#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <regex>

#include "EventServer.h"
#include "protocol.h"

namespace voyager {

int32_t EventServer::startServerLoop(Semaphore &serverReadySem)
{
    int32_t rc = NO_ERROR;
    int32_t clientfd = -1;

    if (SUCCEED(rc)) {
        rc = mSSSM.waitForConnect(&clientfd);
        if (FAILED(rc) || RESULTNOT(rc, USER_ABORTED)) {
            LOGE(mModule, "Failed to wait for client connection");
        }
        if (rc == USER_ABORTED) {
            LOGI(mModule, "Stop wait connect, user aborted.");
        }
        serverReadySem.signal();
    }

    if (SUCCEED(rc)) {
        do {
            int32_t processRc = NO_ERROR;
            char msgBuf[SOCKET_DATA_MAX_LEN];
            RESETRESULT(rc);

            if (SUCCEED(rc)) {
                msgBuf[0] = '\0';
                rc = mSSSM.receiveMsg(msgBuf, sizeof(msgBuf));
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to receive msg from socket sm, %d", rc);
                } else if (FAILED(rc) && RESULTNOT(rc, USER_ABORTED)) {
                    rc = mSSSM.sendMsg(SOCKT_SERVER_FAILED_RECEIVE,
                        strlen(SOCKT_SERVER_FAILED_RECEIVE));
                    if (FAILED(rc)) {
                        LOGE(mModule, "Failed to send msg %s to client, %d",
                            SOCKT_SERVER_FAILED_RECEIVE, rc);
                    }
                }
            }
            
            if (SUCCEED(rc)) {
                rc = processRc = onClientSent(-1, msgBuf);
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed process received fd, %d", rc);
                }
            }

            if (SUCCEED(rc)) {
                std::string msg = SUCCEED(processRc) ?
                    SOCKET_SERVER_PROCESSED : SOCKET_SERVER_PROCESS_FAILED;
                rc = mSSSM.sendMsg(msg.c_str(), msg.length());
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to send msg %s to client, %d",
                        msg.c_str(), rc);
                }
            }
        } while (rc != USER_ABORTED);
    }

    close(clientfd);

    return rc;
}

int32_t EventServer::onClientSent(int32_t fd, const std::string &msg)
{
    int32_t rc = NO_ERROR;
    int32_t event = -1, arg1 = -1, arg2 = -1;
    std::vector<std::string> words;

    if (SUCCEED(rc)) {
        std::regex wsre("\\;");
        std::vector<std::string> _words(
            std::sregex_token_iterator(msg.begin(), msg.end(), wsre, -1),
            std::sregex_token_iterator());
        _words.erase(
            std::remove_if(
                _words.begin(), _words.end(),
                [](std::string const &s) { return s.empty(); }),
                _words.end());
        if (_words.size() != 4) {
            LOGE(mModule, "Wrong event msg format, %s", msg.c_str());
            rc = BAD_PROTOCOL;
        } else {
            words = _words;
        }
    }

    if (SUCCEED(rc)) {
        std::stringstream evtss(words[2]);
        std::stringstream arg1ss(words[3]);
        std::stringstream arg2ss(words[4]);
        evtss >> event;
        arg1ss >> arg1;
        arg2ss >> arg2;
    }

    if (SUCCEED(rc)) {
        rc = mCb->send(event, arg1, arg2);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send event %d %d %d, %d",
                event, arg1, arg2, rc);
        }
    }

    return rc;
}

int32_t EventServer::enqueue(int32_t fd)
{
    return NOT_SUPPORTED;
}

int32_t EventServer::enqueue(void *dat)
{
    return NOT_SUPPORTED;
}

int32_t EventServer::enqueue(void *dat, int32_t format)
{
    return NOT_SUPPORTED;
}

EventServer::EventServer(CallbackIntf *cb) :
    RequestHandler(EVENT, cb)
{
}

EventServer::~EventServer()
{
}

};

