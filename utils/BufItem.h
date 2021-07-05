#ifndef _BUF_ITEM_H_
#define _BUF_ITEM_H_

#include "Common.h"
#include "GlobalTraits.h"
#include "RefBase.h"

namespace voyager {

template <typename T>
struct BufItem :
    public RefBase {
public:
    void *getBuf() const;
    typename AlgTraits<T>::TaskType getTask() const;
    int64_t getSize() const;
    int64_t replace(typename AlgTraits<T>::TaskType &task, int64_t allocsize);

public:
    BufItem(typename AlgTraits<T>::TaskType &task, int64_t allocsize);
    ~BufItem();
    BufItem(const BufItem<T> &rhs);
    BufItem<T> &operator=(const BufItem<T> &rhs);
    bool operator==(const BufItem<T> &rhs) const;
    bool operator!=(const BufItem<T> &rhs) const;

private:
    void   *mBuf;
    int64_t mSize;
    ModuleType mModule;
    typename AlgTraits<T>::TaskType mTask;
};

};

#include "BufItem.hpp"

#endif
