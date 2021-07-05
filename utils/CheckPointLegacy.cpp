#include <stdio.h>
#include <stdarg.h>
#include <map>
#include <algorithm>

#include "Memory.h"
#include "CheckPointLegacy.h"
#include "ThreadPoolEx.h"

#define CHECKPOINT_NAME_MAX_LEN 4096

namespace pandora {

namespace legacy {

CheckPoint gDefaultCheckPoint;

int32_t CheckPoint::request(const std::string &name)
{
    return request(name, false, false);
}

int32_t CheckPoint::requestBlur(const std::string &name)
{
    return request(name, true, false);
}

int32_t CheckPoint::requestNone(const std::string &name)
{
    return request(name, false, true);
}

int32_t CheckPoint::requestNoneBlur(const std::string &name)
{
    return request(name, true, true);
}

int32_t CheckPoint::request(const std::string &name, bool blur, bool none)
{
    int32_t rc = NO_ERROR;
    TrackNode *node = nullptr;
    std::string stripName = name;

    if (SUCCEED(rc)) {
        stripName.erase(0, stripName.find_first_not_of(' '));
        stripName.erase(stripName.find_last_not_of(' ') + 1);
        auto itor = mList.begin();
        while (itor != mList.end()) {
            if (itor->first == stripName) {
                LOGD(mModule, "CheckPoint already exists, %s.", stripName.c_str());
                rc = ALREADY_EXISTS;
            }
            itor++;
        }
    }

    if (SUCCEED(rc)) {
        node = new TrackNode(stripName);
        if (ISNULL(node)) {
            LOGE(mModule, "Failed to new track node.");
            rc = NO_MEMORY;
        } else {
            node->name = stripName;
            node->blur = blur;
            node->none = none;
            node->cancelled = false;
        }
    }

    if (SUCCEED(rc)) {
        mList.push_back(std::make_pair(node->name, node));
    }

    return rc;
}

int32_t CheckPoint::request(bool fmt, const char *format, ...)
{
    int32_t rc = NO_ERROR;
    char *buf = nullptr;

    if (SUCCEED(rc)) {
        buf = (char *)Malloc(CHECKPOINT_NAME_MAX_LEN);
        if (ISNULL(buf)) {
            LOGE(mModule, "Failed to alloc check point name.");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        va_list args;
        va_start(args, format);
        int32_t written = vsnprintf(buf, CHECKPOINT_NAME_MAX_LEN, format, args);
        va_end(args);
        buf[written] = '\0';
    }

    if (SUCCEED(rc)) {
        const std::string name = buf;
        rc = request(name, false, false);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to request for checkpoint %s, %d", buf, rc);
        }
    }

    if (SUCCEED(rc) || FAILED(rc)) {
        if (NOTNULL(buf)) {
            SECURE_FREE(buf);
        }
    }

    return rc;
}

int32_t CheckPoint::reach(const std::string &name)
{
    int32_t rc = NO_ERROR;
    TrackNode *node = nullptr;
    std::string stripName = name;
    std::map<std::string, TrackNode *> blurMatchedNodes;

    if (SUCCEED(rc)) {
        stripName.erase(0, stripName.find_first_not_of(' '));
        stripName.erase(stripName.find_last_not_of(' ') + 1);
    }

    if (SUCCEED(rc)) {
        for (auto &&itr : mList) {
            const std::string &key = itr.first;
            if (key == stripName) {
                node = itr.second;
            } else if (stripName.find(key) != std::string::npos) {
                TrackNode *tmp = itr.second;
                if (NOTNULL(tmp) && tmp->blur) {
                    blurMatchedNodes[key] = tmp;
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        // Might have more than 1 blur matched nodes, match the one with longest key
        if (ISNULL(node) && blurMatchedNodes.size()) {
            auto longest = blurMatchedNodes.begin();
            for (auto itr = blurMatchedNodes.begin(); itr != blurMatchedNodes.end(); itr++) {
                if (itr->first.length() > longest->first.length()) {
                    longest = itr;
                }
            }
            node = longest->second;
        }
    }

    if (SUCCEED(rc)) {
        // It's normal if some of not concerned checkpoints reached
        if (NOTNULL(node)) {
            LOGD(mModule, "Reach checkpoint: [%s]", stripName.c_str());
            node->sem.signal();
            node->recordedSem.wait();
            node->allDoneSem.signal();
        }
    }

    return rc;
}

int32_t CheckPoint::reach(const char *format, ...)
{
    int32_t rc = NO_ERROR;
    char *buf = nullptr;

    if (SUCCEED(rc)) {
        buf = (char *)Malloc(CHECKPOINT_NAME_MAX_LEN);
        if (ISNULL(buf)) {
            LOGE(mModule, "Failed to alloc check point name.");
            rc = NO_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        va_list args;
        va_start(args, format);
        int32_t written = vsnprintf(buf, CHECKPOINT_NAME_MAX_LEN, format, args);
        va_end(args);
        buf[written] = '\0';
    }

    if (SUCCEED(rc)) {
        rc = reach(std::string(buf));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to reach for checkpoint %s, %d", buf, rc);
        }
    }

    if (SUCCEED(rc) || FAILED(rc)) {
        if (NOTNULL(buf)) {
            SECURE_FREE(buf);
        }
    }

    return rc;
}

int32_t CheckPoint::trackRandomOrder(Semaphore &sem, int32_t &cnt)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        for (auto &&itr : mList) {
            rc = mThreads->run(
                [this, &itr, &sem, &cnt]() -> int32_t {
                    TrackNode *node = itr.second;
                    sem.signal();
                    node->sem.wait();
                    if (!node->none && node->cancelled) {
                        mExceptionCheckPoints.push_back(std::make_pair(
                            node->name, std::string("Not reached.")));
                    }
                    if (node->none && !node->cancelled) {
                        mExceptionCheckPoints.push_back(std::make_pair(
                            node->name, std::string("Reached but shouldn't.")));
                    }
                    if (!node->cancelled) {
                        node->recordedSem.signal();
                        node->allDoneSem.wait();
                    }
                    SECURE_DELETE(node);
                    itr.second = nullptr;
                    sem.signal();
                    return NO_ERROR;
                }
            );
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to track on thread, %d", rc);
            } else {
                cnt++;
            }
        }
    }

    return rc;
}

int32_t CheckPoint::track(std::function<int32_t ()> func, bool ignoreFuncReturn)
{
    int32_t rc = NO_ERROR;
    Semaphore sem, funcSem, waitFuncSem;
    int32_t cnt = 0;

    if (SUCCEED(rc)) {
        if (ISNULL(mThreads)) {
            mThreads = ThreadPoolEx::getInstance();
            if (ISNULL(mThreads)) {
                LOGE(mModule, "Failed to get threadpool.");
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mThreads->run(
            [this, &funcSem, &func]() -> int32_t {
                mFuncRc = func();
                funcSem.signal();
                return NO_ERROR;
            }
        );
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to run task func on thread, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = trackRandomOrder(sem, cnt);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to track by random order, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        // Wait all tracking threads started
        for (uint32_t i = 0; i < mList.size(); i++) {
            sem.wait();
        }
    }

    if (SUCCEED(rc)) {
        rc = mThreads->run(
            [this, &funcSem, &waitFuncSem]() -> int32_t {
                // Wait func() to finish
                funcSem.wait();
                // Note: cond wait won't return immediately after signal sent
                // Wait another 1ms for cond wait to return before following force return.
                usleep(Times::Us(Times::Ms(1))());
                // Cancel waiting CheckPoint, won't reach forever
                for (auto &&itr : mList) {
                    TrackNode *node = itr.second;
                    if (NOTNULL(node)) {
                        node->cancelled = true;
                        node->sem.signal();
                    }
                }
                waitFuncSem.signal();
                return NO_ERROR;
            }
        );
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to run task func on thread, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        // Wait all tracking threads exits
        for (int32_t i = 0; i < cnt; i++) {
            sem.wait();
        }
        waitFuncSem.wait();
    }

    if (SUCCEED(rc)) {
        while (mList.begin() != mList.end()) {
            mList.erase(mList.begin());
        }
    }

    if (SUCCEED(rc)) {
        if (mExceptionCheckPoints.size() > 0) {
            LOGE(mModule, "Found exceptional checkpoints, %d", mExceptionCheckPoints.size());
            dump();
            rc = TEST_FAILED;
        }
        while (mExceptionCheckPoints.begin() != mExceptionCheckPoints.end()) {
            mExceptionCheckPoints.erase(mExceptionCheckPoints.begin());
        }
    }

    if (SUCCEED(rc)) {
        if (!ignoreFuncReturn) {
            rc = mFuncRc;
        }
    }

    return rc;
}

int32_t CheckPoint::dump()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!mExceptionCheckPoints.size()) {
            rc = JUMP_DONE;
        }
    }

    if (SUCCEED(rc)) {
        LOGI(mModule, "Dump all exceptional checkpoints:");
        for (auto &&itr : mExceptionCheckPoints) {
            LOGI(mModule, " - Name: %s, %s", itr.first.c_str(), itr.second.c_str());
        }
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

CheckPoint::CheckPoint() :
    Identifier(MODULE_UTILS, "CheckPointLegacy", "v1.0.0"),
    mThreads(nullptr)
{
}

void CheckPoint::clear()
{
    auto itr = mList.begin();
    while (itr != mList.end()) {
        TrackNode *node = itr->second;
        if (NOTNULL(node)) {
            node->sem.signal();
            SECURE_DELETE(node);
        }
        itr = mList.erase(itr);
    }
    mExceptionCheckPoints.clear();
}

CheckPoint::~CheckPoint()
{
    clear();
    if (NOTNULL(mThreads)) {
        mThreads->removeInstance();
        mThreads = nullptr;
    }
}

CheckPoint::TrackNode::TrackNode(const std::string _name) :
    name(_name),
    blur(false),
    none(false),
    cancelled(false)
{
}

};

};
