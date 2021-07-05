#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include <stdint.h>

namespace voyager {

class ProgressBar
{
public:
    int32_t update(uint32_t currnetFrame);
    int32_t print();
    bool finished();
    bool refresh();

public:
    ProgressBar(int32_t caseId, uint32_t totalCase, uint32_t totalFrame);
    ~ProgressBar();

private:
    uint32_t mTotalFrameCount;
    uint32_t mTotalCaseCount;
    int32_t  mId;
    int32_t  mPercentage;
    bool     mRefresh;
    char     mBar[101];
};

};

#endif

