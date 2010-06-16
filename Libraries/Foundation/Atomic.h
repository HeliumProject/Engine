#pragma once

#include "Foundation/API.h"

#include "Platform/Types.h"
#include "Platform/Compiler.h"
#include "Memory/SmartPtr.h"

namespace Foundation
{
  class FOUNDATION_API AtomicRefCountBase NOC_ABSTRACT : public Nocturnal::IRefCount<AtomicRefCountBase>
  {
  private:
    mutable i32 m_RefCount;

  protected:
    AtomicRefCountBase();
    virtual ~AtomicRefCountBase();

  public:
    virtual int GetRefCount() const NOC_OVERRIDE;
    virtual void IncrRefCount() const NOC_OVERRIDE;
    virtual void DecrRefCount() const NOC_OVERRIDE;
  };
}
