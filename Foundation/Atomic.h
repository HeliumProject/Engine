#pragma once

#include "Foundation/API.h"

#include "Platform/Utility.h"
#include "Memory/SmartPtr.h"

namespace Helium
{
    class FOUNDATION_API AtomicRefCountBase HELIUM_ABSTRACT : NonCopyable
    {
    private:
        mutable int32_t m_RefCount;

    protected:
        AtomicRefCountBase();
        virtual ~AtomicRefCountBase();

    public:
        int GetRefCount() const;
        void IncrRefCount() const;
        void DecrRefCount() const;
    };
}
