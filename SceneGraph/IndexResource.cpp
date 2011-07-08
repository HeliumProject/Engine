#include "SceneGraphPch.h"
#include "IndexResource.h"

using namespace Helium;
using namespace Helium::SceneGraph;

Profile::MemoryPoolHandle IndexResource::s_MemoryPool;

IndexResource::IndexResource()
: Resource( ResourceTypes::Index )
, m_ElementType( IndexElementTypes::Unknown )
{

}

IndexResource::~IndexResource()
{

}

uint32_t IndexResource::GetElementSize() const
{
    return IndexElementSizes[ m_ElementType ];
}

uint8_t* IndexResource::Lock() 
{
    void* data = m_Buffer->Map(
        IsDynamic() ? Helium::RENDERER_BUFFER_MAP_HINT_DISCARD : Helium::RENDERER_BUFFER_MAP_HINT_NONE );
    HELIUM_ASSERT( data );

    return static_cast< uint8_t* >( data );
}

void IndexResource::Unlock() 
{
    m_Buffer->Unmap();
}

bool IndexResource::Allocate() 
{
    uint32_t size = GetElementCount() * IndexElementSizes[ GetElementType() ];
    if ( size == 0 )
    {
        return false; 
    }

    Helium::Renderer* pRenderer = Helium::Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );

    m_Buffer = pRenderer->CreateIndexBuffer(
        size,
        ( IsDynamic() ? Helium::RENDERER_BUFFER_USAGE_DYNAMIC : Helium::RENDERER_BUFFER_USAGE_STATIC ),
        IndexElementFormats[ GetElementType() ] );
    HELIUM_ASSERT( m_Buffer );
    if ( m_Buffer )
    {
        Profile::Memory::Allocate( s_MemoryPool, size );
    }

    return ( m_Buffer != NULL );
}

void IndexResource::Release() 
{
    if ( m_Buffer )
    {
        m_Buffer.Release();
        Profile::Memory::Deallocate( s_MemoryPool, GetElementCount() * IndexElementSizes[ GetElementType() ] );
    }
}
