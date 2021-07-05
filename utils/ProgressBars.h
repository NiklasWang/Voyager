#ifndef _PROGRESS_BARS_H_
#define _PROGRESS_BARS_H_

#include <map>
#include <stdint.h>

#include "ProgressBar.h"

namespace pandora {

class ProgressBars
{
public:
    void    add(int32_t id, uint32_t totalFrame);
    int32_t update(int32_t id, uint32_t frameNum);
    int32_t print();
    bool    finished();

public:
    ProgressBars(uint32_t totalCases);
    ~ProgressBars();

private:
    uint32_t mTotalCaseCount;
    bool     mFinished;
    uint32_t mWorkListSize;
    std::map<int32_t, ProgressBar> mWorkList;
    std::map<int32_t, ProgressBar> mDoneList;
};

};

#endif

