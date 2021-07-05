#include "BufItem.h"
#include "Memory.h"
#include "FrameHelper.h"

namespace pandora {

template <typename T>
BufItem<T>::BufItem(
    typename AlgTraits<T>::TaskType &task, int64_t allocsize) :
    mBuf(NULL),
    mSize(0),
    mModule(MODULE_BUFFER_MANAGER),
    mTask(task)
{
    if (NOTNULL(FrameHelper::data(task)) && allocsize > 0LL) {
        replace(task, allocsize);
    } else {
        LOGE(mModule, "Failed to create buf item, buf %p size %d",
            FrameHelper::data(mTask), allocsize);
    }
}

template <typename T>
int64_t BufItem<T>::replace(
    typename AlgTraits<T>::TaskType &task, int64_t allocsize)
{
    int32_t rc = NO_ERROR;
    int64_t size = FrameHelper::size(task);
    ASSERT_LOG(mModule, size <= allocsize,
        "Invalid data size %d and alloc size %d", size, allocsize);

    if (allocsize != mSize) {
        if (NOTNULL(mBuf)) {
            SECURE_FREE(mBuf);
        }
        mBuf = Malloc(allocsize);
        if (NOTNULL(mBuf)) {
            mSize = allocsize;
        } else {
            LOGE(mModule, "Failed to create buf item %d bytes", allocsize);
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        FrameHelper::refresh(mTask, (uint8_t *)mBuf,
            FrameHelper::w(task), FrameHelper::h(task),
            task.format, mSize);
        FrameHelper::copy(mTask, task);
    }

    return SUCCEED(rc) ? mSize : -1LL;
}

template <typename T>
BufItem<T>::~BufItem()
{
    if (NOTNULL(mBuf)) {
        SECURE_FREE(mBuf);
    }
}

template <typename T>
BufItem<T>::BufItem(const BufItem<T> &rhs)
{
    BufItem<T>(rhs.mTask);
}

template <typename T>
BufItem<T> &BufItem<T>::operator=(const BufItem<T> &rhs)
{
    if (this != &rhs) {
        if (NOTNULL(mBuf)) {
            SECURE_FREE(mBuf);
        }

        void *buf = rhs.getBuf();
        size_t size = rhs.getSize();
        if (NOTNULL(buf) && size > 0) {
            mBuf = Malloc(size);
            if (NOTNULL(mBuf)) {
                mSize = size;
                memcpy(mBuf, buf, size);
                mTask = rhs.mTask;
                mTask.data = mBuf;
            } else {
                LOGE(mModule, "Failed to create buf item %d bytes", size);
            }
        } else {
            LOGE(mModule, "Failed to create buf item, buf %p size %d",
                buf, size);
        }
    }

    return *this;
}

template <typename T>
bool BufItem<T>::operator==(const BufItem<T> &rhs) const
{
    bool rc = false;
    if ((rhs.getSize() == mSize) &&
        (!memcmp(rhs.getBuf(), mBuf, rhs.getSize()))) {
        rc = true;
    }

    return rc;
}

template <typename T>
bool BufItem<T>::operator!=(const BufItem<T> &rhs) const
{
    return !(*this == rhs);
}

template <typename T>
void *BufItem<T>::getBuf() const
{
    return mBuf;
}

template <typename T>
typename AlgTraits<T>::TaskType BufItem<T>::getTask() const
{
    return mTask;
}

template <typename T>
int64_t BufItem<T>::getSize() const
{
    return mSize;
}

};

