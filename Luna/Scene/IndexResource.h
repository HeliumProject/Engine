#pragma once

#include "Resource.h" 

namespace Luna
{

  class IndexResource : public Resource
  {
  public:
    static Profile::MemoryPoolHandle s_MemoryPool;
    IDirect3DIndexBuffer9*           m_Buffer; 

    IndexResource( ResourceTracker* tracker ); 

    IDirect3DIndexBuffer9* GetBuffer() const
    {
      return m_Buffer; 
    }

    virtual u8* Lock() NOC_OVERRIDE; 
    virtual void Unlock() NOC_OVERRIDE; 
    virtual bool SetState() const NOC_OVERRIDE; 

  protected:
    virtual bool Allocate() NOC_OVERRIDE; 
    virtual void Release() NOC_OVERRIDE; 
  };

  typedef Nocturnal::SmartPtr<IndexResource> IndexResourcePtr;

}
