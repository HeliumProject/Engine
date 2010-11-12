#pragma once

#include "Foundation/API.h"

#include "Platform/Types.h"
#include "Memory/SmartPtr.h"

namespace Helium
{
    class FOUNDATION_API AtomicRefCountBase HELIUM_ABSTRACT : public Helium::IRefCount<AtomicRefCountBase>
    {
    private:
        mutable int32_t m_RefCount;

    protected:
        AtomicRefCountBase();
        virtual ~AtomicRefCountBase();

    public:
        virtual int GetRefCount() const HELIUM_OVERRIDE;
        virtual void IncrRefCount() const HELIUM_OVERRIDE;
        virtual void DecrRefCount() const HELIUM_OVERRIDE;
    };
}
