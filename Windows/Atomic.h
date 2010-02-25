#pragma once

#include "API.h"

#include "Common/Types.h"
#include "Common/Compiler.h"
#include "Common/Memory/Smartptr.h"

namespace Windows
{
  class WINDOWS_API AtomicRefCountBase NOC_ABSTRACT : public Nocturnal::IRefCount<AtomicRefCountBase>
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
