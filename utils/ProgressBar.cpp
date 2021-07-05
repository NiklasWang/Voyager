#include "ProgressBar.h"
#include "Common.h"

namespace pandora {

ProgressBar::ProgressBar(int32_t caseId, uint32_t totalCase, uint32_t totalFrame) :
    mTotalFrameCount(totalFrame),
    mTotalCaseCount(totalCase),
    mId(caseId),
    mPercentage(0),
    mRefresh(false)
{
    memset(mBar, 0, ARRAYSIZE(mBar));
}

ProgressBar::~ProgressBar()
{
    mTotalFrameCount = 0;
}

int32_t ProgressBar::update(uint32_t currnetFrame)
{
    int32_t rc = NO_ERROR;
    int32_t i = currnetFrame * 100 / mTotalFrameCount;

    mRefresh = false;
    if ((currnetFrame * 100 / mTotalFrameCount) < ((currnetFrame + 1) * 100 / mTotalFrameCount)) {
        mBar[i] = '#';
        mBar[i+1] = 0;
        mRefresh= true;
    }

    if (currnetFrame == mTotalFrameCount - 1) {
        while (i < 100) {
            mBar[i] = '#';
            mBar[i+1] = 0;
            mRefresh= true;
            i++;
        }
    }

    mPercentage = i;
    return rc;
}

int32_t ProgressBar::print()
{
    int32_t rc = NO_ERROR;
    const char lable[] = "|/-\\";

    printf("[%d/%d] [%-100s][%d%%][%c]\n", mId, mTotalCaseCount, mBar, mPercentage, lable[mPercentage % 4]);
    mRefresh = false;

    return rc;
}

bool ProgressBar::finished()
{
    return mPercentage == 100;
}

bool ProgressBar::refresh()
{
    return mRefresh;
}

};

