#include <stdio.h>
#include <stdarg.h>
#include <algorithm>

#include "Memory.h"
#include "CheckPointOrderFixed.h"

#define CHECKPOINT_NAME_MAX_LEN 4096

namespace pandora {

CheckPointOrderFixed gFixOrderCheckPoints;

int32_t CheckPointOrderFixed::request(const std::string &name)
{
    return request(name, false, false);
}

int32_t CheckPointOrderFixed::requestBlur(const std::string &name)
{
    return request(name, true, false);
}

int32_t CheckPointOrderFixed::requestNone(const std::string &name)
{
    return request(name, false, true);
}

int32_t CheckPointOrderFixed::requestNoneBlur(const std::string &name)
{
    return request(name, true, true);
}

int32_t CheckPointOrderFixed::request(const std::string &name, bool blur, bool none)
{
    int32_t rc = NO_ERROR;
    std::string stripName = name;

    if (SUCCEED(rc)) {
        stripName.erase(0, stripName.find_first_not_of(' '));
        stripName.erase(stripName.find_last_not_of(' ') + 1);
        // Allow 2 same checkpoints exist in order fixed checkpoints
        // But not allow in none list
        if (none) {
            for (auto &&iter : mNoneList) {
                if (iter.first == stripName) {
                    LOGD(mModule, "checkpoints already exist in none list, %s %s.",
                        stripName.c_str(), blur ? "blur" : "non blur");
                    rc = ALREADY_EXISTS;
                    break;
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        TrackNode *node = new TrackNode(stripName);
        if (ISNULL(node)) {
            LOGE(mModule, "Failed to new track node.");
            rc = NO_MEMORY;
        } else {
            node->name = stripName;
            node->blur = blur;
        }
        if (none) {
            mNoneList.push_back(std::make_pair(stripName, node));
        } else {
            mList.push_back(std::make_pair(stripName, node));
        }
    }

    return rc;
}

int32_t CheckPointOrderFixed::request(bool fmt, const char *format, ...)
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
        rc = request(buf, false, false);
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

int32_t CheckPointOrderFixed::reach(const std::string &name)
{
    int32_t rc = NO_ERROR;
    std::string stripName = name;
    bool firstMatched = false;

    if (SUCCEED(rc)) {
        stripName.erase(0, stripName.find_first_not_of(' '));
        stripName.erase(stripName.find_last_not_of(' ') + 1);
    }

    if (SUCCEED(rc)) {
        for (auto &&pair : mNoneList) {
            const std::string &key = pair.first;
            if (key == stripName) {
                mExceptionCheckPoints.push_back(std::make_pair(
                    stripName, std::string("Reached but shouldn't.")));
                break;
            } else if (stripName.find(key) != std::string::npos) {
                TrackNode *tmp = pair.second;
                if (NOTNULL(tmp) && tmp->blur) {
                    mExceptionCheckPoints.push_back(std::make_pair(
                        stripName, std::string("Reached blurry but shouldn't.")));
                    break;
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        if (mList.size()) {
            const std::string &key = (*mList.begin()).first;
            if (key == stripName) {
                firstMatched = true;
            } else if (stripName.find(key) != std::string::npos) {
                TrackNode *tmp = (*mList.begin()).second;
                if (NOTNULL(tmp) && tmp->blur) {
                    firstMatched = true;
                }
            }
        }
        if (firstMatched) {
            LOGD(mModule, "Reach checkpoint: [%s]", stripName.c_str());
            TrackNode *node = (*mList.begin()).second;
            if (NOTNULL(node)) {
                SECURE_DELETE(node);
            }
            mList.erase(mList.begin());
        }
    }

    if (SUCCEED(rc)) {
        if (!firstMatched) {
            bool disordered = false;
            for (auto &&pair : mList) {
                const std::string &key = pair.first;
                if (key == stripName) {
                    disordered = true;
                    break;
                } else if (stripName.find(key) != std::string::npos) {
                    TrackNode *tmp = pair.second;
                    if (NOTNULL(tmp) && tmp->blur) {
                        disordered = true;
                        break;
                    }
                }
            }
            if (disordered) {
                mExceptionCheckPoints.push_back(std::make_pair(stripName,
                    std::string("Disordered with ") + (*mList.begin()).first));
            }
        }
    }

    return rc;
}

int32_t CheckPointOrderFixed::reach(const char *format, ...)
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

int32_t CheckPointOrderFixed::track(std::function<int32_t ()> func, bool ignoreFuncReturn)
{
    int32_t rc = NO_ERROR;
    mFuncRc = func();

    if (SUCCEED(rc)) {
        for (auto &&itr : mList) {
            mExceptionCheckPoints.push_back(std::make_pair(
                itr.first, std::string("Not Reached.")));
        }
        if (mExceptionCheckPoints.size() > 0) {
            LOGE(mModule, "Found exceptional CheckPointOrderFixed, %d", mExceptionCheckPoints.size());
            dump();
            rc = TEST_FAILED;
        }
    }

    if (SUCCEED(rc)) {
        clear();
        if (!ignoreFuncReturn) {
            rc = mFuncRc;
        }
    }

    return rc;
}

int32_t CheckPointOrderFixed::dump()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!mExceptionCheckPoints.size()) {
            rc = JUMP_DONE;
        }
    }

    if (SUCCEED(rc)) {
        LOGI(mModule, "Dump all exceptional fixed order checkpoints:");
        for (auto &&itr : mExceptionCheckPoints) {
            LOGI(mModule, " - Name: %s, %s", itr.first.c_str(), itr.second.c_str());
        }
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

CheckPointOrderFixed::CheckPointOrderFixed( ) :
    Identifier(MODULE_UTILS, "CheckPointOrderFixed", "v1.0.0"),
    mFuncRc(0)
{
}

void CheckPointOrderFixed::clear()
{
    auto itr = mList.begin();
    while (itr != mList.end()) {
        TrackNode *node = itr->second;
        if (NOTNULL(node)) {
            SECURE_DELETE(node);
        }
        itr = mList.erase(itr);
    }
    auto itrn = mNoneList.begin();
    while (itrn != mNoneList.end()) {
        TrackNode *node = itrn->second;
        if (NOTNULL(node)) {
            SECURE_DELETE(node);
        }
        itrn = mNoneList.erase(itrn);
    }
    mList.clear();
    mNoneList.clear();
    mExceptionCheckPoints.clear();
    mFuncRc = 0;
}

CheckPointOrderFixed::~CheckPointOrderFixed()
{
    clear();
}

CheckPointOrderFixed::TrackNode::TrackNode(const std::string _name) :
    name(_name),
    blur(false)
{
}

};

