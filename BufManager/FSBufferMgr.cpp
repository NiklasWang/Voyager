#include "FSBufferMgr.h"

#define FS_BUFFER_PATH "/tmp"

namespace voyager {

FSBufferMgr::FSBufferMgr() :
    Identifier(MODULE_FS_HELPER, "FSBufferMgr", "1.0.0"),
    mPageSize(0)
{
    mPageSize = sysconf(_SC_PAGESIZE);
    if (mPageSize <= 0) {
        LOGE(mModule, "Failed to get page size, %d", mPageSize);
        mPageSize = 4096;
    }
}

FSBufferMgr::~FSBufferMgr()
{
    clear();
}

int32_t FSBufferMgr::allocate(void **buf, int64_t len)
{
    int32_t fd = -1;
    return allocate(buf, len, &fd);
}

int32_t FSBufferMgr::allocate(void **buf, int64_t len, int32_t *fd)
{
    int32_t rc = NO_ERROR;
    Buffer buffer;
    int64_t aligned = len;

    if (SUCCEED(rc)) {
        aligned = align_len_to_size(len, mPageSize);
        rc = allocate(&buffer, aligned);
        if (SUCCEED(rc)) {
            *buf = buffer.ptr;
            *fd  = buffer.fd;
            mBuffers.push_back(buffer);
        }
    }

    return rc;
}

int32_t FSBufferMgr::allocate(Buffer *buf, int64_t len)
{
    int32_t rc = NO_ERROR;
    int32_t fd = -1;
    void *addr = NULL;
    std::string fsName;

    if (SUCCEED(rc)) {
        fsName  = FS_BUFFER_PATH;
        fsName += "/";
        fsName += PROJNAME;
        rc = access(fsName.c_str(), R_OK | W_OK);
        if (FAILED(rc)) {
            LOGI(mModule, "FS file dir %s does not exist, create it.", DUMP_PATH);
            unlink(fsName.c_str());
            rc = mkdir(fsName.c_str(), 0755);
            if (FAILED(rc)) {
                LOGE(mModule, "mkdir %s failed, %s.",
                    fsName.c_str(), strerror(errno));
                rc = UNKNOWN_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = removeNotOccupiedFiles(fsName);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to remove not occupied files, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        static int32_t id = 0;
        fsName += "/";
        fsName += whoamI();
        fsName += "_";
        fsName += "allocate";
        fsName += "_";
        fsName += std::to_string(id++);
        fsName += "_";
        fsName += std::to_string(len);
        fsName += "_";
        fsName += TimeUtils::getDateTime();
        fsName += ".share";
    }

    if (SUCCEED(rc)) {
        fd = open(fsName.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0755);
        if (fd < 0) {
            LOGE(mModule, "Failed to create file %s, %s",
                fsName.c_str(), strerror(errno));
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            LOGE(mModule, "Failed to mmap on fd %d len %d", fd, len);
            close(fd);
            rc = SYS_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        buf->ptr = addr;
        buf->len = len;
        buf->fd  = fd;
        buf->fsName = fsName;
    }

    return rc;
}

int32_t FSBufferMgr::removeNotOccupiedFiles(const std::string &basePath)
{
    int32_t rc = NO_ERROR;
    struct dirent *dp = nullptr;
    DIR *dir = nullptr;
    std::list<std::string> files;

    if (SUCCEED(rc)) {
        dir = opendir(basePath.c_str());
        if (ISNULL(dir)) {
            LOGE(mModule, "Failed to open dir %s", basePath.c_str());
            rc = NOT_EXIST;
        }
    }

    if (SUCCEED(rc)) {
        while ((dp = readdir(dir)) != NULL) {
            std::string path = basePath;
            path += "/";
            path += dp->d_name;
            if (dp->d_type & DT_DIR) {
                if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                    rc = removeNotOccupiedFiles(path);
                    if (FAILED(rc)) {
                        LOGE(mModule, "Failed to remove not occupied files in dir %s.", path.c_str());
                    }
                }
            } else if (dp->d_type & DT_REG) {
                if (access(path, R_OK | W_OK) != 0) {
                    files.push_back(path);
                }
            }
        }
        closedir(dir);
    }

    if (SUCCEED(rc)) {
        for (auto &&file : files) {
            if (unlink(file.c_str()) != 0) {
                LOGE(mModule, "Failed to remove file %s", file.c_str());
                rc = SYS_ERROR;
                break;
            }
        }
    }

    return rc;
}

int32_t FSBufferMgr::import(void **buf, int32_t fd, int64_t len)
{
    int32_t rc = NO_ERROR;
    Buffer buffer;

    if (SUCCEED(rc)) {
        rc = import(&buffer, fd, len);
        if (SUCCEED(rc)) {
            *buf = buffer.ptr;
            mBuffers.push_back(buffer);
        }
    }

    return rc;
}

int32_t FSBufferMgr::import(Buffer *buf, int32_t fd, int64_t len)
{
    int32_t rc = NO_ERROR;
    void *addr = NULL;

    if (SUCCEED(rc)) {
        if (fd <= 0 || len <= 0LL) {
            LOGE(mModule, "Invalid fd %d or len %d", fd, len);
        }
    }

    if (SUCCEED(rc)) {
        addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            LOGE(mModule, "Failed to mmap with fd %d", fd);
            rc = SYS_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        buf->ptr = addr;
        buf->len = len;
        buf->fd  = fd;
        buf->fsName = "";
    }

    return rc;
}

int32_t FSBufferMgr::flush(void *buf)
{
    Buffer *buffer = findBuf(buf);

    msync(buffer->ptr, buffer->len, MS_SYNC);

    return flush(buffer->fd);
}

int32_t FSBufferMgr::flush(int32_t fd)
{
    int32_t rc = NO_ERROR;

    if (fsync(fd) != 0) {
        LOGE(mModule, "Failed to flush fd %d", fd);
        rc = SYS_ERROR;
    }

    return rc;
}

FSBufferMgr::Buffer *FSBufferMgr::findBuf(void *buf)
{
    Buffer *result = NULL;

    for (auto &itr : mBuffers) {
        if (buf == itr.ptr) {
            result = &itr;
            break;
        }
    }

    return result;
}

FSBufferMgr::Buffer *FSBufferMgr::findBuf(int32_t fd)
{
    Buffer *result = NULL;

    for (auto &itr : mBuffers) {
        if (fd == itr.fd) {
            result = &itr;
            break;
        }
    }

    return result;
}

int32_t FSBufferMgr::release(void *buf)
{
    int32_t rc = NO_ERROR;
    Buffer *buffer = findBuf(buf);

    if (ISNULL(buffer)) {
        LOGE(mModule, "Failed to find memory.");
        rc = NO_MEMORY;
    }

    if (SUCCEED(rc)) {
        rc = release(buffer);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to release buf, %d", rc);
        }
    }

    return rc;
}

int32_t FSBufferMgr::release(int32_t fd)
{
    Buffer *buffer = findBuf(fd);
    return release(buffer->ptr);
}

int32_t FSBufferMgr::release(Buffer *buf)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (NOTNULL(buf->ptr)) {
            munmap(buf->ptr, buf->len);
        } else {
            rc = PARAM_INVALID;
        }
        if (buf->fd > 0) {
            close(buf->fd);
        } else {
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        buf->ptr = NULL;
        buf->len = 0;
        buf->fd = -1;
        buf->fsName = "";
    }

    return rc;
}

void FSBufferMgr::clear()
{
    while (mBuffers.begin() != mBuffers.end()) {
        auto iter = mBuffers.begin();
        Buffer *buf = &(*iter);
        if (NOTNULL(buf)) {
            release(buf);
        }
        mBuffers.erase(iter);
    }
}

};

