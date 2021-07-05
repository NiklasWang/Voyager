#include "ProgressBars.h"
#include "Common.h"

namespace pandora {

ProgressBars::ProgressBars(uint32_t totalCases) :
    mTotalCaseCount(totalCases),
    mFinished(false),
    mWorkListSize(0)
{
}

ProgressBars::~ProgressBars()
{
}

void ProgressBars::add(int32_t id, uint32_t totalFrame)
{
    ProgressBar bar(id, mTotalCaseCount, totalFrame);
    mWorkList.insert(std::pair<int32_t, ProgressBar>(id, bar));
}

int32_t ProgressBars::update(int32_t id, uint32_t frameNum)
{
    int32_t rc = NO_ERROR;

    for (auto it = mWorkList.begin(); it != mWorkList.end(); it++) {
        if (it->first == id) {
            it->second.update(frameNum);
        }
    }

    return rc;
}

int32_t ProgressBars::print()
{
    int32_t rc = NO_ERROR;

    for (auto it = mWorkList.begin(); it != mWorkList.end(); it++) {
        if (it->second.refresh()) {
            if (mWorkListSize < mWorkList.size()) {
                for (uint32_t i = 0; i < mWorkList.size() - mWorkListSize; i++) {
                    printf("i %u\n", i);
                }
                mWorkListSize = mWorkList.size();
            } else {
                mWorkListSize = mWorkList.size();
            }

            for (auto it = mWorkList.begin(); it != mWorkList.end(); it++) {
                printf("\033[1A");
                printf("\033[K");
            }

            for (auto it = mDoneList.begin(); it != mDoneList.end(); it++) {
                printf("\033[1A");
                printf("\033[K");
            }

            for (auto it = mDoneList.begin(); it != mDoneList.end(); it++) {
                it->second.print();
            }

            for (auto it = mWorkList.begin(); it != mWorkList.end(); it++) {
                it->second.print();
                if (it->second.finished()) {
                    mDoneList.insert(std::pair<int32_t, ProgressBar>(it->first, it->second));
                    mWorkList.erase(it);
                    it--;
                }
                if (mWorkList.size() == 0) {
                    break;
                }
            }
            break;
        }
    }

    if (mDoneList.size() == mTotalCaseCount) {
        mFinished = true;
    }

    return rc;
}

bool ProgressBars::finished()
{
    return mFinished;
}


}

