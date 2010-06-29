#pragma once

#include "Resource.h" 

namespace Luna
{

  class VertexResource : public Resource
  {
  public:
    static Profile::MemoryPoolHandle s_MemoryPool;
    IDirect3DVertexBuffer9*          m_Buffer; 

    VertexResource( ResourceTracker* tracker ); 

    IDirect3DVertexBuffer9* GetBuffer() const
    {
      return m_Buffer;
    }

    virtual u8* Lock() NOC_OVERRIDE; 
    virtual void Unlock() NOC_OVERRIDE; 
    virtual bool SetState() const NOC_OVERRIDE; 

  protected:
    virtual bool Allocate() NOC_OVERRIDE;
    virtual void Release() NOC_OVERRIDE;

    u8* m_LockedVerts; 

  };

  typedef Nocturnal::SmartPtr<VertexResource> VertexResourcePtr;

}
