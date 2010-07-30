#include "Platform/Windows/Windows.h"
#include "Atomic.h"

using namespace Helium;

AtomicRefCountBase::AtomicRefCountBase()
: m_RefCount (0)
{

}

AtomicRefCountBase::~AtomicRefCountBase()
{

}

int AtomicRefCountBase::GetRefCount() const
{
    return m_RefCount;
}

void AtomicRefCountBase::IncrRefCount() const
{
    InterlockedIncrement( (LONG*) &m_RefCount ); 
}

void AtomicRefCountBase::DecrRefCount() const
{
    LONG result = InterlockedDecrement( (LONG*) &m_RefCount ); 

    if (result == 0)
    {
        delete this;
    }
}