#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <sys/time.h>

#include "LogImpl.h"

#define DBG_LOG_MAX_LEN 4096

#include "signal.h"

#ifdef PRINT_LOGCAT_LOG
#include <android/log.h>

#undef LOG_TAG
#define LOG_TAG PROJNAME VERSION
#endif

namespace voyager {

#ifndef FS_LOG_FULL_PATH
#define FS_LOG_FULL_PATH      "./voyager.log"
#endif
#define FS_LOG_FULL_PATH_LAST FS_LOG_FULL_PATH ".bak"
#define MAX_PROCESS_NAME_LEN  16
#define LOG_MAX_LEN_PER_LINE  10240 // Bytes

bool gSavingLog = true;
bool gLastSavingLogStatus = gSavingLog;

int8_t gDebugController[][LOG_TYPE_MAX_INVALID + 1] = {
    // NONE,  DBG,  INF, WARN,  ERR, FATA, INVA
    {     0,    0,    1,    1,    1,    1,    0}, // MODULE_OTHERS
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SOCKET
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_CLIENT
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SERVER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_CLIENT_IMPL
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SERVER_IMPL
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_CLIENT_CORE
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SERVER_CORE
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_OVERALL_CONTRO
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_BUFFER_MANAGER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_ION_HELPER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_FS_HELPER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SERVER_HANDLER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_CLIENT_HANDLER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_DATA_SERVER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_DATA_CLIENT
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_FD_SERVER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_FD_CLIENT
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_FRAME_SERVER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_FRAME_CLIENT
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_EVENT_SERVER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_EVENT_CLIENT

    {     0,    0,    1,    1,    1,    1,    0}, // MODULE_COMMON
    {     0,    0,    1,    1,    1,    1,    0}, // MODULE_UTILS
    {     0,    0,    1,    1,    1,    1,    0}, // MODULE_THREAD_POOL
    {     0,    0,    1,    1,    1,    1,    0}, // MODULE_MEMORY_POOL
    {     1,    1,    1,    1,    1,    1,    1}, // MODULE_MAX_INVALID
};

static const char *const gLogType[] = {
    [LOG_TYPE_NONE]        = "<NONE>",
    [LOG_TYPE_DEBUG]       = "< DBG>",
    [LOG_TYPE_INFO]        = "< INF>",
    [LOG_TYPE_WARN]        = "<WARN>",
    [LOG_TYPE_ERROR]       = "< ERR>",
    [LOG_TYPE_FATAL]       = "<FATA>",
    [LOG_TYPE_MAX_INVALID] = "<INVA>",
};

static char    gProcess[PATH_MAX] = { '\0' };
static int32_t gLogfd = -1;
static char    gLogLine[LOG_MAX_LEN_PER_LINE];
static pthread_mutex_t gWriteLock = PTHREAD_MUTEX_INITIALIZER;

bool checkLogPrint(ModuleType module, LogType level)
{
    return gDebugController[getValidType(module)][level];
}

static int32_t getMaxInvalidId(char *text, int32_t len)
{
    int32_t i = 0;

    for (; i < len; i++) {
        if (!((text[i] == '/') ||
            (text[i] == '.') || (text[i] == '_') ||
            (text[i] >= '0' && text[i] <= '9') ||
            (text[i] >= 'A' && text[i] <= 'Z') ||
            (text[i] >= 'a' && text[i] <= 'z'))) {
            text[i + 1] = '\0';
            break;
        }
    }

    return i;
}

char *getProcessName()
{
    if (gProcess[0] == '\0') {
        pid_t pid = getpid();
        char path[32];
        char text[PATH_MAX] = { '\0' };
        int  length;

        sprintf(path, "/proc/%d/cmdline", pid);
        int32_t fd = open(path, O_RDONLY);
        if (fd > 0) {
            ssize_t len = read(fd, text, PATH_MAX);
            if (len > 0) {
                text[len] = text[getMaxInvalidId(text, len)] = '\0';
                char *index = strrchr(text, '/');
                if (index != NULL) {
                    strcpy(gProcess, index + 1);
                }
            }
            close(fd);
        }

        if (gProcess[0] == '\0') {
            text[0] = '\0';
            strcpy(path, "/proc/self/exe");
            ssize_t len = readlink(path, text, PATH_MAX);
            if (len > 0) {
                text[len] = text[getMaxInvalidId(text, len)] = '\0';
                char *index = strrchr(text, '/');
                if (index != NULL) {
                    strcpy(gProcess, index + 1);
                }
            }
        }

        if (gProcess[0] == '\0') {
            strcpy(gProcess, "Unknown");
        }

        length = strlen(gProcess);
        if (length > MAX_PROCESS_NAME_LEN) {
            memmove(gProcess,
                gProcess + length + 1 - MAX_PROCESS_NAME_LEN,
                MAX_PROCESS_NAME_LEN + 1);
        }
    }

    return gProcess;
}

static bool checkValid(LogType type)
{
    bool rc = false;

    if (type >= 0 && type < LOG_TYPE_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

static LogType getValidType(LogType type)
{
    return checkValid(type) ? type : LOG_TYPE_MAX_INVALID;
}

const char *getLogType(LogType type)
{
    return gLogType[getValidType(type)];
}

int32_t __log_vsnprintf(char* pdst, int32_t size,
    const char* pfmt, va_list argptr)
{
    int32_t written = 0;

    pdst[0] = '\0';
    written = vsnprintf(pdst, size, pfmt, argptr);

    if ((written >= size) && (size > 0)) {
       // Message length exceeds the buffer limit size
       written = size - 1;
       pdst[size - 1] = '\0';
    }

    return written;
}

void print_log(const LogType logt, const char *fmt,
    char *process, const char *module, const char *type,
    const char *func, const int line, const char *buf);

void save_log(const char *fmt, char *process,
    const char *module, const char *type,
    const char *func, const int line, const char *buf);

void enable_log_saving()
{
    gLastSavingLogStatus = gSavingLog;
    gSavingLog = true;
}

void disable_log_saving()
{
    gLastSavingLogStatus = gSavingLog;
    gSavingLog = false;
}

void restore_log_saving()
{
    gSavingLog = gLastSavingLogStatus;
}

void __debug_log(ModuleType module, LogType type,
    const char *func, const int line, const char *fmt, ...)
{
    char    buf[DBG_LOG_MAX_LEN];
    va_list args;

    va_start(args, fmt);
    __log_vsnprintf(buf, DBG_LOG_MAX_LEN, fmt, args);
    va_end(args);

    print_log(type, "%s %s%s 0x%x: %s:+%d: %s\n",
        getProcessName(), getModuleShortName(module),
        getLogType(type), func, line, buf);

    save_log("%s %s%s 0x%x: %s:+%d: %s\n",
        getProcessName(), getModuleShortName(module),
        getLogType(type), func, line, buf);
}

void __assert_log(ModuleType module, unsigned char cond,
    const char *func, const int line, const char *fmt, ...)
{
    char    buf[DBG_LOG_MAX_LEN];
    va_list args;

    if (cond == 0) {
        va_start(args, fmt);
        __log_vsnprintf(buf, DBG_LOG_MAX_LEN, fmt, args);
        va_end(args);

        print_log(LOG_TYPE_ERROR, "[<! ASSERT !>]%s %s%s 0x%x: %s:+%d: %s\n",
            getProcessName(), getModuleShortName(module),
            "<ASSERT>", func, line, buf);

        save_log("[<! ASSERT !>]%s %s%s 0x%x: %s:+%d: %s\n",
            getProcessName(), getModuleShortName(module),
            "<ASSERT>", func, line, buf);

        save_log("[<! ASSERT !>] Process will suicide now.\n",
            getProcessName(), getModuleShortName(MODULE_OTHERS),
            getLogType(LOG_TYPE_FATAL), __FUNCTION__, __LINE__, buf);
        raise(SIGTRAP);
    }
}

void save_log(const char *fmt, char *process,
    const char *module, const char *type,
    const char *func, const int line, const char *buf)
{
    if (!gSavingLog) {
        return;
    }

    if (gLogfd == -1) {
        if (!access(FS_LOG_FULL_PATH_LAST, F_OK)) {
            if (unlink(FS_LOG_FULL_PATH_LAST)) {
                print_log(LOG_TYPE_ERROR, fmt,
                    process, getModuleShortName(MODULE_OTHERS),
                    getLogType(LOG_TYPE_ERROR), __FUNCTION__, __LINE__,
                    "Failed to remove last log file " FS_LOG_FULL_PATH_LAST);
            }
        }
    }

    if (gLogfd == -1) {
        if (!access(FS_LOG_FULL_PATH, F_OK)) {
            if (rename(FS_LOG_FULL_PATH, FS_LOG_FULL_PATH_LAST)) {
                print_log(LOG_TYPE_ERROR, fmt,
                    process, getModuleShortName(MODULE_OTHERS),
                    getLogType(LOG_TYPE_ERROR), __FUNCTION__, __LINE__,
                    "Failed to rename log file " FS_LOG_FULL_PATH
                    " to " FS_LOG_FULL_PATH_LAST);
            }
        }
    }

    if (gLogfd == -1) {
        if (!access(FS_LOG_FULL_PATH, F_OK)) {
            if (unlink(FS_LOG_FULL_PATH)) {
                print_log(LOG_TYPE_ERROR, fmt,
                    process, getModuleShortName(MODULE_OTHERS),
                    getLogType(LOG_TYPE_ERROR), __FUNCTION__, __LINE__,
                    "Failed to remove curr log file " FS_LOG_FULL_PATH);
            }
        }
    }

    if (gLogfd == -1) {
        gLogfd = open(FS_LOG_FULL_PATH, O_RDWR | O_CREAT | O_TRUNC, 0777);
        if (gLogfd < 0) {
            print_log(LOG_TYPE_ERROR, fmt,
                process, getModuleShortName(MODULE_OTHERS),
                getLogType(LOG_TYPE_ERROR), __FUNCTION__, __LINE__,
                "Failed to create file " FS_LOG_FULL_PATH " for logs.");
        }
    }

    if (gLogfd > 0) {
        time_t t  = time(NULL);
        struct tm* local = localtime(&t);
        char timeBuf[32];
        strftime(timeBuf, sizeof(timeBuf) - 1, "%Y-%m-%d %H:%M:%S", local);
        struct timeval tv;
        gettimeofday(&tv, NULL);

        pthread_mutex_lock(&gWriteLock);
        snprintf(gLogLine, sizeof(gLogLine) - 1, "%s.%03ld pid %d ",
            timeBuf, tv.tv_usec / 1000, getpid());
        int32_t cnt = strlen(gLogLine);
        snprintf(gLogLine + cnt, sizeof(gLogLine) - cnt - 1,
            fmt, process, module, type,
            std::this_thread::get_id(), func, line, buf);
        cnt = strlen(gLogLine);
        ssize_t len = write(gLogfd, gLogLine, cnt);
        if (cnt > len) {
            char tmp[255];
            sprintf(tmp, "Log len %d bytes, written %ld bytes.",
                cnt, len);
            print_log(LOG_TYPE_ERROR, fmt,
                process, getModuleShortName(MODULE_OTHERS),
                getLogType(LOG_TYPE_ERROR), __FUNCTION__, __LINE__, tmp);
        }
        pthread_mutex_unlock(&gWriteLock);
    }

    return;
}

#ifdef PRINT_LOGCAT_LOG
void print_log(const LogType logt, const char *fmt,
    char *process, const char *module, const char *type,
    const char *func, const int line, const char *buf)
{
    android_LogPriority logPriority = ANDROID_LOG_DEBUG;

    switch (logt) {
        case LOG_TYPE_NONE:
            logPriority = ANDROID_LOG_DEFAULT;
            break;
        case LOG_TYPE_DEBUG:
            logPriority = ANDROID_LOG_DEBUG;
            break;
        case LOG_TYPE_INFO:
            logPriority = ANDROID_LOG_INFO;
            break;
        case LOG_TYPE_WARN:
            logPriority = ANDROID_LOG_WARN;
            break;
        case LOG_TYPE_ERROR:
            logPriority = ANDROID_LOG_ERROR;
            break;
        case LOG_TYPE_FATAL:
            logPriority = ANDROID_LOG_FATAL;
            break;
        case LOG_TYPE_MAX_INVALID:
        default:
            logPriority = ANDROID_LOG_DEFAULT;
            break;
    }

    __android_log_print(logPriority, LOG_TAG,
        fmt, process, module, type, std::this_thread::get_id(),
        func, line, buf);

    return;
}
#endif

#ifndef PRINT_LOGCAT_LOG
void print_log(const LogType logt, const char *fmt,
    char *process, const char *module, const char *type,
    const char *func, const int line, const char *buf)
{
    printf(fmt, process, module, type,
        std::this_thread::get_id(), func, line, buf);

    return;
}
#endif

};

