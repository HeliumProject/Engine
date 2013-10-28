#include "EditorScenePch.h"
#include "PrimitiveRings.h"

#include "Graphics/BufferedDrawer.h"
#include "EditorScene/Pick.h"

#include "Orientation.h"

using namespace Helium;
using namespace Helium::Editor;

PrimitiveRings::PrimitiveRings()
{
	SetElementType( VertexElementTypes::SimpleVertex );

	m_Radius = 1.0f;
	m_Steps = 36;

	m_Bounds.minimum = Vector3 (-m_Radius, -m_Radius, -m_Radius);
	m_Bounds.maximum = Vector3 (m_Radius, m_Radius, m_Radius);
}

void PrimitiveRings::Update()
{
	m_Bounds.minimum = Vector3 (-m_Radius, -m_Radius, -m_Radius);
	m_Bounds.maximum = Vector3 (m_Radius, m_Radius, m_Radius);

	SetElementCount( m_Steps*6 );
	m_Vertices.clear();

	float32_t stepAngle = (float32_t)HELIUM_TWOPI / (float32_t)(m_Steps);

	for (uint32_t x=0; x<m_Steps; x++)
	{
		float32_t theta = (float32_t)(x) * stepAngle;
		Vector3 position = SetupVector( 0.0f, Cos( theta ) * m_Radius, Sin( theta ) * m_Radius );
		m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
		position = SetupVector( 0.0f, Cos( theta + stepAngle ) * m_Radius, Sin( theta + stepAngle ) * m_Radius );
		m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	}

	for (uint32_t y=0; y<m_Steps; y++)
	{
		float32_t theta = (float32_t)(y) * stepAngle;
		Vector3 position = SetupVector( Cos( theta ) * m_Radius, 0.0f, Sin( theta ) * m_Radius );
		m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
		position = SetupVector( Cos( theta + stepAngle ) * m_Radius, 0.0f, Sin( theta + stepAngle ) * m_Radius );
		m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	}

	for (uint32_t z=0; z<m_Steps; z++)
	{
		float32_t theta = (float32_t)(z) * stepAngle;
		Vector3 position = SetupVector( Cos( theta ) * m_Radius, Sin( theta ) * m_Radius, 0.0f );
		m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
		position = SetupVector( Cos( theta + stepAngle ) * m_Radius, Sin( theta + stepAngle ) * m_Radius, 0.0f );
		m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	}

	Base::Update();
}

void PrimitiveRings::Draw(
	BufferedDrawer* drawInterface,
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
		m_Steps * 6,
		0,
		m_Steps * 3,
		materialColor );
}

bool PrimitiveRings::Pick( PickVisitor* pick, const bool* solid ) const
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