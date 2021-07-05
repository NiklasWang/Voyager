#ifndef _LOG_IMPL_H_
#define _LOG_IMPL_H_

#include "Version.h"
#include "Modules.h"

namespace pandora {

enum LogType {
    LOG_TYPE_NONE,
    LOG_TYPE_DEBUG,
    LOG_TYPE_INFO,
    LOG_TYPE_WARN,
    LOG_TYPE_ERROR,
    LOG_TYPE_FATAL,
    LOG_TYPE_MAX_INVALID,
};

void __debug_log(ModuleType module, LogType type,
    const char *func, const int line, const char *fmt, ...);

void __assert_log(ModuleType module, unsigned char cond,
    const char *func, const int line, const char *fmt, ...);

bool checkLogPrint(ModuleType module, LogType level);

#undef PLOGx
#define PLOGx(module, level, fmt, args...)     \
    do {                                       \
        if (checkLogPrint(module, level)) { \
            __debug_log(module, level, __func__, __LINE__, fmt, ##args); \
        }                                      \
    } while(0);

#undef PLOGD
#define PLOGD(module, fmt, args...)                \
    PLOGx(module, LOG_TYPE_DEBUG, fmt, ##args)
#undef PLOGI
#define PLOGI(module, fmt, args...)                \
    PLOGx(module, LOG_TYPE_INFO, fmt, ##args)
#undef PLOGW
#define PLOGW(module, fmt, args...)                \
    PLOGx(module, LOG_TYPE_WARN, fmt, ##args)
#undef PLOGE
#define PLOGE(module, fmt, args...)                \
    PLOGx(module, LOG_TYPE_ERROR, fmt, ##args)
#undef PLOGF
#define PLOGF(module, fmt, args...)                \
    PLOGx(module, LOG_TYPE_FATAL, fmt, ##args)
#undef PLOGDIF
#define PLOGDIF(module, cond, fmt, args...)        \
    do { if (cond) { PLOGx(module, LOG_TYPE_DEBUG, fmt, ##args) } } while(0)
#undef PLOGIIF
#define PLOGIIF(module, cond, fmt, args...)        \
    do { if (cond) { PLOGx(module, LOG_TYPE_INFO, fmt, ##args) } } while(0)
#undef PLOGWIF
#define PLOGWIF(module, cond, fmt, args...)        \
    do { if (cond) { PLOGx(module, LOG_TYPE_WARN, fmt, ##args) } } while(0)
#undef PLOGEIF
#define PLOGEIF(module, cond, fmt, args...)        \
    do { if (cond) { PLOGx(module, LOG_TYPE_ERROR, fmt, ##args) } } while(0)
#undef PLOGFIF
#define PLOGFIF(module, cond, fmt, args...)        \
    do { if (cond) { PLOGx(module, LOG_TYPE_FATAL, fmt, ##args) } } while(0)

#undef ASSERT_PLOG
#define ASSERT_PLOG(module, cond, fmt, args...)     \
    __assert_log(module, cond, __func__, __LINE__, fmt, ##args)


void enable_log_saving();
void disable_log_saving();
void restore_log_saving();

#define PENABLE_LOG_SAVING()  enable_log_saving()
#define PDISABLE_LOG_SAVING() disable_log_saving()
#define PRESTORE_LOG_SAVING() restore_log_saving()


};


#endif
