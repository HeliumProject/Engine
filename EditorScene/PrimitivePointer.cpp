#include "SceneGraphPch.h"
#include "PrimitivePointer.h"

#include "Graphics/BufferedDrawer.h"
#include "SceneGraph/Pick.h"

#include "Orientation.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitivePointer::PrimitivePointer()
{
	SetElementCount( 12 );
	SetElementType( VertexElementTypes::SimpleVertex );

	m_Bounds.minimum = SetupVector(-1, 0, -2);
	m_Bounds.maximum = SetupVector( 1, 1,  0);
}

void PrimitivePointer::Update()
{
	m_Vertices.clear();

	Vector3 position;

	position = SetupVector( -1.0f, 0.0f, -2.0f );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	position = SetupVector( 0.0f, 1.0f, -2.0f );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	position = SetupVector( 1.0f, 0.0f, -2.0f );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	position = SetupVector( 0.0f, 0.0f, 0.0f );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	position = SetupVector( -1.0f, 0.0f, -2.0f );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	position = SetupVector( 0.0f, 1.0f, -2.0f );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	position = SetupVector( 0.0f, 0.0f, 0.0f );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	position = SetupVector( -1.0f, 0.0f, -2.0f );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	position = SetupVector( 1.0f, 0.0f, -2.0f );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );

	Base::Update();
}

void PrimitivePointer::Draw(
	Helium::BufferedDrawer* drawInterface,
	DrawArgs* args,
	Helium::Color materialColor,
	const Simd::Matrix44& transform,
	const bool* solid,
	const bool* transparent ) const
{
	HELIUM_ASSERT( drawInterface );

	drawInterface->DrawUntextured(
		Helium::RENDERER_PRIMITIVE_TYPE_LINE_LIST,
		transform,
		m_Buffer,
		NULL,
		GetBaseIndex(),
		12,
		0,
		6,
		materialColor,
		Helium::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
	args->m_LineCount += 6;
}

bool PrimitivePointer::Pick( PickVisitor* pick, const bool* solid ) const
{
	for (size_t i=0; i<m_Vertices.size(); i+=2)
	{
		const Helium::SimpleVertex& vertex0 = m_Vertices[ i ];
		const Helium::SimpleVertex& vertex1 = m_Vertices[ i + 1 ];
		Vector3 position0( vertex0.position[ 0 ], vertex0.position[ 1 ], vertex0.position[ 2 ] );
		Vector3 position1( vertex1.position[ 0 ], vertex1.position[ 1 ], vertex1.position[ 2 ] );
		if ( pick->PickSegment( position0, position1 ) )
		{
			return true;
		}
	}

	return false;
}