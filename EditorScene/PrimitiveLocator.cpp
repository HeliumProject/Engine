#include "EditorScenePch.h"
#include "PrimitiveLocator.h"

#include "Graphics/BufferedDrawer.h"
#include "EditorScene/Pick.h"

using namespace Helium;
using namespace Helium::Editor;

PrimitiveLocator::PrimitiveLocator()
{
	SetElementCount( 6 );
	SetElementType( VertexElementTypes::SimpleVertex );

	m_Length = 1.0f;
}

void PrimitiveLocator::Update()
{
	m_Bounds.minimum = Vector3 (-m_Length, -m_Length, -m_Length);
	m_Bounds.maximum = Vector3 (m_Length, m_Length, m_Length);

	m_Vertices.clear();

	m_Vertices.push_back( Helium::SimpleVertex( -m_Length, 0.0f, 0.0f ) );
	m_Vertices.push_back( Helium::SimpleVertex( m_Length, 0.0f, 0.0f ) );

	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, -m_Length, 0.0f ) );
	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, m_Length, 0.0f ) );

	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, 0.0f, -m_Length ) );
	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, 0.0f, m_Length ) );

	Base::Update();
}

void PrimitiveLocator::Draw(
	Helium::BufferedDrawer* drawInterface,
	DrawArgs* args,
	Helium::Color materialColor,
	const Simd::Matrix44& transform,
	const bool* solid,
	const bool* transparent ) const
{
	HELIUM_ASSERT( drawInterface );
	HELIUM_ASSERT( args );

	drawInterface->DrawUntextured(
		Helium::RENDERER_PRIMITIVE_TYPE_LINE_LIST,
		transform,
		m_Buffer,
		NULL,
		GetBaseIndex(),
		6,
		0,
		3,
		materialColor,
		Helium::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
	args->m_LineCount += 3;
}

bool PrimitiveLocator::Pick( PickVisitor* pick, const bool* solid ) const
{
	size_t vertexCount = m_Vertices.size();
	for ( size_t vertexIndex = 0; vertexIndex < vertexCount; vertexIndex += 2 )
	{
		const Helium::SimpleVertex& vertex0 = m_Vertices[ vertexIndex ];
		const Helium::SimpleVertex& vertex1 = m_Vertices[ vertexIndex + 1 ];
		Vector3 position0( vertex0.position[ 0 ], vertex0.position[ 1 ], vertex0.position[ 2 ] );
		Vector3 position1( vertex1.position[ 0 ], vertex1.position[ 1 ], vertex1.position[ 2 ] );
		if ( pick->PickSegment( position0, position1, 0.0f ) )
		{
			return true;
		}
	}

	return false;
}