#include "EditorScenePch.h"
#include "Resource.h"

#include "Foundation/Profile.h"
#include "Foundation/Log.h"

#include "Rendering/RIndexBuffer.h"
#include "Rendering/RVertexBuffer.h"

#include <algorithm>

using namespace Helium::Editor;

void Resource::Create()
{
	if ( !m_IsCreated )
	{
		m_IsCreated = true;

		if (Allocate())
		{
			Populate();
		}
	}
}

void Resource::Delete()
{
	if ( m_IsCreated )
	{
		m_IsCreated = false;

		Release();
	}
}

void Resource::Update()
{
	if (m_IsCreated)
	{
		if (m_IsDirty || !IsDynamic())
		{
			Delete();
			Create();
		}
		else
		{
			Populate();
		}
	}
	else
	{
		Create();
	}
}

void Resource::Populate()
{
	if (m_Populator.Valid())
	{
		PopulateArgs args ( m_Type, m_BaseIndex = 0, Lock() );

		if (args.m_Buffer)
		{
			uint32_t offset = args.m_Offset;

			m_Populator.Invoke( &args );

			HELIUM_ASSERT( args.m_Offset == ( offset + GetElementSize() * m_ElementCount ) );

			Unlock();
		}
		else
		{
			HELIUM_BREAK(); // this is BAD
		}
	}
}

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

	return ( m_Buffer != NULL );
}

void IndexResource::Release() 
{
	if ( m_Buffer )
	{
		m_Buffer.Release();
	}
}

VertexResource::VertexResource()
	: Resource( ResourceTypes::Vertex )
	, m_ElementType( VertexElementTypes::Unknown )
	, m_LockedVerts( NULL )
{

}

VertexResource::~VertexResource()
{

}

uint32_t VertexResource::GetElementSize() const
{
	return VertexElementSizes[ m_ElementType ];
}

uint8_t* VertexResource::Lock() 
{
	void* data = m_Buffer->Map(
		IsDynamic() ? Helium::RENDERER_BUFFER_MAP_HINT_DISCARD : Helium::RENDERER_BUFFER_MAP_HINT_NONE );
	HELIUM_ASSERT( data );

	return static_cast< uint8_t* >( data );
}

void VertexResource::Unlock() 
{
	m_Buffer->Unmap();
}

bool VertexResource::Allocate() 
{
	uint32_t size = GetElementCount() * VertexElementSizes[ GetElementType() ];
	if ( size == 0 )
	{
		return false; 
	}

	Helium::Renderer* pRenderer = Helium::Renderer::GetStaticInstance();
	if ( pRenderer )
	{
		m_Buffer = pRenderer->CreateVertexBuffer(
			size,
			( IsDynamic() ? Helium::RENDERER_BUFFER_USAGE_DYNAMIC : Helium::RENDERER_BUFFER_USAGE_STATIC ) );
		HELIUM_ASSERT( m_Buffer );
	}

	return ( m_Buffer != NULL );
}

void VertexResource::Release() 
{
	if ( m_Buffer )
	{
		m_Buffer.Release();
	}
}
