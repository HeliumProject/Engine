#include "EditorScenePch.h"
#include "PrimitiveCube.h"

#include "Graphics/BufferedDrawer.h"
#include "EditorScene/Pick.h"

using namespace Helium;
using namespace Helium::Editor;

PrimitiveCube::PrimitiveCube()
{
	SetElementType( VertexElementTypes::SimpleVertex );
	SetElementCount( 60 );

	m_Bounds.minimum = Vector3 (-1.0f, -1.0f, -1.0f);
	m_Bounds.maximum = Vector3 (1.0f, 1.0f, 1.0f);
}

void PrimitiveCube::Update()
{
	m_Vertices.clear();

	V_Vector3 vertices, drawVertices;
	m_Bounds.GetVertices( vertices );
	m_Bounds.GetWireframe( vertices, drawVertices, false );
	m_Bounds.GetTriangulated( vertices, drawVertices, false );

	size_t drawVertexCount = drawVertices.size();
	m_Vertices.reserve( drawVertexCount );
	for ( size_t vertexIndex = 0; vertexIndex < drawVertexCount; ++vertexIndex )
	{
		const Vector3& position = drawVertices[ vertexIndex ];
		m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	}

	Base::Update();
}

void PrimitiveCube::Draw(
	Helium::BufferedDrawer* drawInterface,
	DrawArgs* args,
	Helium::Color materialColor,
	const Simd::Matrix44& transform,
	const bool* solid,
	const bool* transparent ) const
{
	HELIUM_ASSERT( drawInterface );

	if (transparent ? *transparent : m_IsTransparent)
	{
		if ( materialColor.GetA() == 0 )
		{
			materialColor.SetA( 0x80 );
		}
	}

	if (solid ? *solid : m_IsSolid)
	{
		drawInterface->DrawUntextured(
			Helium::RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
			transform,
			m_Buffer,
			NULL,
			GetBaseIndex() + 24,
			36,
			0,
			12,
			materialColor );
		args->m_TriangleCount += 12;
	}
	else
	{
		drawInterface->DrawUntextured(
			Helium::RENDERER_PRIMITIVE_TYPE_LINE_LIST,
			transform,
			m_Buffer,
			NULL,
			GetBaseIndex(),
			24,
			0,
			12,
			materialColor,
			Helium::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
		args->m_LineCount += 12;
	}
}

bool PrimitiveCube::Pick( PickVisitor* pick, const bool* solid ) const
{
	if (solid ? *solid : m_IsSolid)
	{
		return pick->PickBox(m_Bounds);
	}
	else
	{
		for (size_t i=0; i<24; i+=2)
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

	return false;
}