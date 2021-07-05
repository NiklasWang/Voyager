#include "Identifier.h"
#include "Atomic.h"

namespace voyager {

IdType Identifier::mInstanceCnt = 0;

Identifier::Identifier(ModuleType module, const char *name, const char *version) :
    mName(name),
    mModule(module)
{
    mName += " ";
    mName += version;
    utils_atomic_inc(&mInstanceCnt);
    mId = mInstanceCnt;
}

Identifier::Identifier(const Identifier &rhs)
{
    *this = rhs;
}

Identifier &Identifier::operator=(const Identifier &rhs)
{
    mName = rhs.name();
    utils_atomic_inc(&mInstanceCnt);
    mId = mInstanceCnt;

    return *this;
}

IdType Identifier::myId() const
{
    return mId;
}

IdType Identifier::id() const
{
    return myId();
}

const char *Identifier::myName() const
{
    return mName.c_str();
}

const char *Identifier::me() const
{
    return myName();
}

const char *Identifier::name() const
{
    return myName();
}

const char *Identifier::whoamI() const
{
    return myName();
}

ModuleType Identifier::getModule() const
{
    return mModule;
}

void Identifier::dump() const
{
    LOGI(MODULE_COMMON, "My identifier, id %d name %d", myId(), whoamI());
    return;
}

};

