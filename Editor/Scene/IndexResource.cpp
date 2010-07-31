#include "Precompile.h"
#include "IndexResource.h"

using namespace Helium;
using namespace Helium::Editor;

Profile::MemoryPoolHandle IndexResource::s_MemoryPool;

IndexResource::IndexResource( ResourceTracker* tracker )
: Resource ( ResourceTypes::Index, tracker )
, m_Buffer(NULL)
{

}

u8* IndexResource::Lock() 
{
    u8* data = NULL;
    u32 lockFlags = (!IsManaged() && IsDynamic()) ? D3DLOCK_DISCARD : 0;
    HRESULT result = m_Buffer->Lock(0, 0, (void**)&data, lockFlags);
    HELIUM_ASSERT(SUCCEEDED(result));

    return data;
}


bool IndexResource::SetState() const 
{
    if ( GetElementCount() > 0 )
    {
        if ( m_Buffer == NULL )
        {
            HELIUM_BREAK();
            return false;
        }

        if ( m_Buffer != m_Tracker->GetIndices() )
        {
#ifdef LUNA_DEBUG_RESOURCES
            Log::Print("Setting indices to 0x%p\n", m_Buffer);
#endif
            m_Device->SetIndices( m_Buffer );
            m_Tracker->SetIndices( m_Buffer );
        }
    }

    return true;
}

void IndexResource::Unlock() 
{
    m_Buffer->Unlock();
}

bool IndexResource::Allocate() 
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

    HRESULT result = m_Device->CreateIndexBuffer( size, usage, format, pool, &m_Buffer, NULL );
    bool success = SUCCEEDED(result);
    HELIUM_ASSERT(success);

    Profile::Memory::Allocate( s_MemoryPool, size );

    return success;

}

void IndexResource::Release() 
{
    if (m_Buffer)
    {
        m_Buffer->Release();
        m_Buffer = NULL;
    }

    Profile::Memory::Deallocate( s_MemoryPool, GetElementCount() * ElementSizes[ GetElementType() ] );
}
