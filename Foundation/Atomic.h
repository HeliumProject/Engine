#pragma once

#include "Platform/Atomic.h"
#include "Platform/Utility.h"

namespace Helium
{
    template< typename T >
    class AtomicRefCountBase
    {
    private:
        mutable volatile int32_t m_RefCount;

    public:
        AtomicRefCountBase();
        AtomicRefCountBase( const AtomicRefCountBase& rSource );

        int32_t GetRefCount() const;
        int32_t IncrRefCount() const;
        int32_t DecrRefCount() const;

        AtomicRefCountBase& operator=( const AtomicRefCountBase& rSource );
    };
}

#include "Foundation/Atomic.inl"
