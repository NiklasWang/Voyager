#ifndef _LOG_H_
#define _LOG_H_

#include "LogImpl.h"

namespace pandora {

#ifdef ENABLE_LOGGER

#define LOGD(module, fmt, args...) PLOGD(module, fmt, ##args)
#define LOGI(module, fmt, args...) PLOGI(module, fmt, ##args)
#define LOGW(module, fmt, args...) PLOGW(module, fmt, ##args)
#define LOGE(module, fmt, args...) PLOGE(module, fmt, ##args)
#define LOGF(module, fmt, args...) PLOGF(module, fmt, ##args)
#define LOGDIF(module, cond, fmt, args...) PLOGDIF(module, cond, fmt, ##args)
#define LOGIIF(module, cond, fmt, args...) PLOGIIF(module, cond, fmt, ##args)
#define LOGWIF(module, cond, fmt, args...) PLOGWIF(module, cond, fmt, ##args)
#define LOGEIF(module, cond, fmt, args...) PLOGEIF(module, cond, fmt, ##args)
#define LOGFIF(module, cond, fmt, args...) PLOGFIF(module, cond, fmt, ##args)
#define ASSERT_LOG(module, cond, fmt, args...) ASSERT_PLOG(module, cond, fmt, ##args)

#define ENABLE_LOG_SAVING()  PENABLE_LOG_SAVING()
#define DISABLE_LOG_SAVING() PDISABLE_LOG_SAVING()
#define RESTORE_LOG_SAVING() PRESTORE_LOG_SAVING()

#else

#define LOGD(module, fmt, args...)
#define LOGI(module, fmt, args...)
#define LOGW(module, fmt, args...)
#define LOGE(module, fmt, args...)
#define LOGF(module, fmt, args...)
#define LOGDIF(module, cond, fmt, args...)
#define LOGIIF(module, cond, fmt, args...)
#define LOGWIF(module, cond, fmt, args...)
#define LOGEIF(module, cond, fmt, args...)
#define LOGFIF(module, cond, fmt, args...)
#define ASSERT_LOG(module, cond, fmt, args...)

#define ENABLE_LOG_SAVING()
#define DISABLE_LOG_SAVING()
#define RESTORE_LOG_SAVING()

#endif


struct EnableLogSaving {
    EnableLogSaving() {
        ENABLE_LOG_SAVING();
    }
    ~EnableLogSaving() {
        RESTORE_LOG_SAVING();
    }
};

struct DisableLogSaving {
    DisableLogSaving() {
        DISABLE_LOG_SAVING();
    }
    ~DisableLogSaving() {
        RESTORE_LOG_SAVING();
    }
};

};

#endif
