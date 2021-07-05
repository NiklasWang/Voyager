#include <stdio.h>
#include <stdarg.h>
#include <map>
#include <algorithm>

#include "Memory.h"
#include "CheckPoint.h"

#define CHECKPOINT_NAME_MAX_LEN 4096

namespace voyager {

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
        std::list<std::pair<std::string, TrackNode *> > &list =
            none ? mNoneList : mList;
        for (auto &&iter : list) {
            if (iter.first == stripName) {
                LOGD(mModule, "CheckPoint already exists, %s %s %s.",
                    stripName.c_str(), blur ? "blur" : "non-blur",
                    none ? "none" : "have");
                rc = ALREADY_EXISTS;
            }
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
        }
    }

    if (SUCCEED(rc)) {
        if (none) {
            mNoneList.push_back(std::make_pair(node->name, node));
        } else {
            mList.push_back(std::make_pair(node->name, node));
        }
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
        for (auto &&pair : mNoneList) {
            const std::string &key = pair.first;
            if (key == stripName) {
                mExceptionalCheckPoints.push_back(std::make_pair(
                    stripName, std::string("Reached but shouldn't.")));
                break;
            } else if (stripName.find(key) != std::string::npos) {
                TrackNode *tmp = pair.second;
                if (NOTNULL(tmp) && tmp->blur) {
                    mExceptionalCheckPoints.push_back(std::make_pair(
                        stripName, std::string("Reached blurry but shouldn't.")));
                    break;
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        for (auto &&itr : mList) {
            const std::string &key = itr.first;
            if (key == stripName) {
                node = itr.second;
                break;
            } else if (stripName.find(key) != std::string::npos) {
                TrackNode *tmp = itr.second;
                if (NOTNULL(tmp) && tmp->blur) {
                    blurMatchedNodes[key] = tmp;
                }
            }
        }
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
        if (NOTNULL(node)) {
            LOGD(mModule, "Reach checkpoint: [%s]", stripName.c_str());
            auto iter = mList.begin();
            while (iter != mList.end()) {
                if (iter->first == node->name) {
                    if (NOTNULL(iter->second)) {
                        SECURE_DELETE(iter->second);
                    }
                    iter = mList.erase(iter);
                    break;
                } else {
                    iter++;
                }
            }
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

int32_t CheckPoint::track(std::function<int32_t ()> func, bool ignoreFuncReturn)
{
    int32_t rc = NO_ERROR;
    mFuncRc = func();

    if (SUCCEED(rc)) {
        for (auto &&itr : mList) {
            mExceptionalCheckPoints.push_back(std::make_pair(
                itr.first, std::string("Not Reached.")));
        }
        if (mExceptionalCheckPoints.size() > 0) {
            LOGE(mModule, "Found exceptional CheckPoint, %d",
                mExceptionalCheckPoints.size());
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

int32_t CheckPoint::dump()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!mExceptionalCheckPoints.size()) {
            rc = JUMP_DONE;
        }
    }

    if (SUCCEED(rc)) {
        LOGI(mModule, "Dump all exceptional checkpoints:");
        for (auto &&itr : mExceptionalCheckPoints) {
            LOGI(mModule, " - Name: %s, %s", itr.first.c_str(), itr.second.c_str());
        }
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

CheckPoint::CheckPoint() :
    Identifier(MODULE_UTILS, "CheckPoint", "v1.0.0"),
    mFuncRc(0)
{
}

void CheckPoint::clear()
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
    mExceptionalCheckPoints.clear();
    mFuncRc = 0;
}

CheckPoint::~CheckPoint()
{
    clear();
}

CheckPoint::TrackNode::TrackNode(const std::string _name) :
    name(_name),
    blur(false)
{
}

};

