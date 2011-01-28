/*#include "Precompile.h"*/
#include "VertexResource.h" 

using namespace Helium;
using namespace Helium::SceneGraph;

Profile::MemoryPoolHandle VertexResource::s_MemoryPool;

VertexResource::VertexResource( ResourceTracker* tracker )
: Resource( ResourceTypes::Vertex, tracker )
, m_ElementType( VertexElementTypes::Unknown )
, m_LockedVerts( NULL )
{

}

uint32_t VertexResource::GetElementSize() const
{
    return VertexElementSizes[ m_ElementType ];
}

uint8_t* VertexResource::Lock() 
{
    void* data = m_Buffer->Map(
        IsDynamic() ? Lunar::RENDERER_BUFFER_MAP_HINT_DISCARD : Lunar::RENDERER_BUFFER_MAP_HINT_NONE );
    HELIUM_ASSERT( data );

    return static_cast< uint8_t* >( data );
}

void VertexResource::Unlock() 
{
    m_Buffer->Unmap();
}

bool VertexResource::SetState() const 
{
    if ( GetElementCount() > 0 )
    {
        if ( m_Buffer == NULL )
        {
            HELIUM_BREAK();
            return false;
        }

        if ( m_Buffer != m_Tracker->GetVertices() )
        {
#ifdef SCENE_DEBUG_RESOURCES
            Log::Print("Setting vertices to 0x%p\n", m_Buffer);
#endif
            m_Device->SetStreamSource( 0, m_Buffer, 0, (UINT)ElementSizes[ GetElementType() ] );
            m_Tracker->SetVertices( m_Buffer );
        }

        if ( ElementFormats[ GetElementType() ] != m_Tracker->GetVertexFormat() )
        {
#ifdef SCENE_DEBUG_RESOURCES
            Log::Print("Setting FVF to 0x%x\n", ElementFormats[ m_ElementType ]);
#endif
            m_Device->SetFVF( ElementFormats[ GetElementType() ] );
            m_Tracker->SetVertexFormat( ElementFormats[ GetElementType() ] );
        }
    }

    return true;
}

bool VertexResource::Allocate() 
{
    uint32_t size = GetElementCount() * VertexElementSizes[ GetElementType() ];
    if ( size == 0 )
    {
        return false; 
    }

    Lunar::Renderer* pRenderer = Lunar::Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    m_Buffer = pRenderer->CreateVertexBuffer(
        size,
        ( IsDynamic() ? Lunar::RENDERER_BUFFER_USAGE_DYNAMIC : Lunar::RENDERER_BUFFER_USAGE_STATIC ) );
    HELIUM_ASSERT( m_Buffer );
    if ( m_Buffer )
    {
        Profile::Memory::Allocate( s_MemoryPool, size );
    }

    return ( m_Buffer != NULL );
}

void VertexResource::Release() 
{
    if ( m_Buffer )
    {
        m_Buffer.Release();
        Profile::Memory::Deallocate( s_MemoryPool, GetElementCount() * VertexElementSizes[ GetElementType() ] );
    }
}
