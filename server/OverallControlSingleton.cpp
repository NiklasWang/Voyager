#include "Common.h"
#include "OverallControlSingleton.h"
#include "AutoLock.h"

namespace voyager {

uint32_t OverallControlSingleton::gCnt = 0;

OverallControlSingleton *OverallControlSingleton::gThis = NULL;

pthread_mutex_t OverallControlSingleton::gInsLock = PTHREAD_MUTEX_INITIALIZER;

OverallControlSingleton *OverallControlSingleton::getInstance()
{
    if (ISNULL(gThis)) {
        AutoExternalLock l(&gInsLock);
        if (ISNULL(gThis)) {
            gThis = new OverallControlSingleton();
            if (ISNULL(gThis)) {
                LOGE(MODULE_OVERALL_CONTROL, "Failed to create overall control sigleton.");
            }
        }
    }

    if (NOTNULL(gThis)) {
        gCnt++;
    }

    return gThis;
}

uint32_t OverallControlSingleton::removeInstance()
{
    ASSERT_LOG(MODULE_OVERALL_CONTROL, gCnt > 0, "Instance not got.");
    ASSERT_LOG(MODULE_OVERALL_CONTROL, NOTNULL(gThis), "Instance not created.");

    gCnt--;
    if (gCnt == 0 && NOTNULL(gThis)) {
        AutoExternalLock l(&gInsLock);
        delete gThis;
        gThis = NULL;
    }

    return gCnt;
}

OverallControlSingleton::OverallControlSingleton()
{
}

OverallControlSingleton::~OverallControlSingleton()
{
}

};

