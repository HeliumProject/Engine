#include "Precompile.h"
#include "VertexResource.h" 

namespace Luna
{
  Profile::MemoryPoolHandle VertexResource::s_MemoryPool;

  VertexResource::VertexResource( ResourceTracker* tracker )
    : Resource ( ResourceTypes::Vertex, tracker )
    , m_Buffer(NULL)
    , m_LockedVerts(NULL)
  {

  }


  u8* VertexResource::Lock() 
  {
    u8* data = NULL;
    u32 lockFlags = (!IsManaged() && IsDynamic()) ? D3DLOCK_DISCARD : 0;
    HRESULT result = m_Buffer->Lock(0, 0, (void**)&data, lockFlags);
    NOC_ASSERT(SUCCEEDED(result));

    return data;
  }

  void VertexResource::Unlock() 
  {
    m_Buffer->Unlock();
  }

  bool VertexResource::SetState() const 
  {
    if ( GetElementCount() > 0 )
    {
      if ( m_Buffer == NULL )
      {
        NOC_BREAK();
        return false;
      }

      if ( m_Buffer != m_Tracker->GetVertices() )
      {
#ifdef LUNA_DEBUG_RESOURCES
        Console::Print("Setting vertices to 0x%p\n", m_Buffer);
#endif
        m_Device->SetStreamSource( 0, m_Buffer, 0, (UINT)ElementSizes[ GetElementType() ] );
        m_Tracker->SetVertices( m_Buffer );
      }

      if ( ElementFormats[ GetElementType() ] != m_Tracker->GetVertexFormat() )
      {
#ifdef LUNA_DEBUG_RESOURCES
        Console::Print("Setting FVF to 0x%x\n", ElementFormats[ m_ElementType ]);
#endif
        m_Device->SetFVF( ElementFormats[ GetElementType() ] );
        m_Tracker->SetVertexFormat( ElementFormats[ GetElementType() ] );
      }
    }

    return true;
  }

  bool VertexResource::Allocate() 
  {
    UINT size = GetElementCount() * ElementSizes[ GetElementType() ];

    if(size == 0)
    {
      return false; 
    }

    DWORD usage = D3DUSAGE_WRITEONLY;
    if (IsDynamic())
    {
      usage |= D3DUSAGE_DYNAMIC;
    }

    D3DFORMAT format = (D3DFORMAT)ElementFormats[ GetElementType() ];

    D3DPOOL pool = IsManaged() ? D3DPOOL_MANAGED : D3DPOOL_DEFAULT;

    HRESULT result = m_Device->CreateVertexBuffer( size, usage, format, pool, &m_Buffer, NULL );
    bool success = SUCCEEDED(result);
    NOC_ASSERT(success);

    Profile::Memory::Allocate( s_MemoryPool, size );

    return success;

  }

  void VertexResource::Release() 
  {
    if (m_Buffer)
    {
      m_Buffer->Release();
      m_Buffer = NULL;
    }

    Profile::Memory::Deallocate( s_MemoryPool, GetElementCount() * ElementSizes[ GetElementType() ] );
  }
}
