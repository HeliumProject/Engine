#include "EditorScenePch.h"
#include "PrimitiveCone.h"

#include "Graphics/BufferedDrawer.h"
#include "EditorScene/Pick.h"

using namespace Helium;
using namespace Helium::Editor;

PrimitiveCone::PrimitiveCone()
{
	SetElementType( VertexElementTypes::SimpleVertex );

	m_Length = 1.0f;
	m_Radius = 0.5f;
	m_Steps = 24;

	m_Bounds.minimum = Vector3 (-m_Radius, -m_Radius, -m_Length);
	m_Bounds.maximum = Vector3 (m_Radius, m_Radius, m_Length);
}

uint32_t PrimitiveCone::GetWireVertCount() const
{
	if (m_Steps == 0)
	{
		return m_WireVertCount = 0;
	}
	else
	{
		return m_WireVertCount = m_Steps+1+8;
	}
}

uint32_t PrimitiveCone::GetPolyVertCount() const
{
	if (m_Steps == 0)
	{
		return m_PolyVertCount = 0;
	}
	else
	{
		return m_PolyVertCount = (m_Steps*2)+4;
	}
}

void PrimitiveCone::Update()
{
	m_Bounds.minimum = Vector3 (-m_Radius, -m_Radius, -m_Length);
	m_Bounds.maximum = Vector3 (m_Radius, m_Radius, m_Length);

	SetElementCount( GetWireVertCount() + GetPolyVertCount() );
	m_Vertices.clear();

	float stepAngle = (float32_t)HELIUM_TWOPI / (float32_t)(m_Steps);


	//
	// Wire
	//

	float32_t halfLength = m_Length * 0.5f;

	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, 0.0f, halfLength ) );
	m_Vertices.push_back( Helium::SimpleVertex( m_Radius, 0.0f, -halfLength ) );

	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, 0.0f, halfLength ) );
	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, m_Radius, -halfLength ) );

	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, 0.0f, halfLength ) );
	m_Vertices.push_back( Helium::SimpleVertex( -m_Radius, 0.0f, -halfLength ) );

	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, 0.0f, halfLength ) );
	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, -m_Radius, -halfLength ) );

	for (int x=0; x<=m_Steps; x++)
	{
		float theta = (float32_t)(x) * stepAngle;
		m_Vertices.push_back( Helium::SimpleVertex( Cos( theta ) * m_Radius, Sin( theta ) * m_Radius, -halfLength ) );
	}


	//
	// Poly
	//

	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, 0.0f, halfLength ) );
	for (int x=0; x<=m_Steps; x++)
	{
		float theta = (float32_t)(x) * stepAngle;
		m_Vertices.push_back( Helium::SimpleVertex( Cos( theta ) * m_Radius, Sin( theta ) * m_Radius, -halfLength ) );
	}

	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, 0.0f, -halfLength ) );
	for (int x=m_Steps; x>=0; x--)
	{
		float theta = (float32_t)(x) * stepAngle;
		m_Vertices.push_back( Helium::SimpleVertex( Cos( theta ) * m_Radius, Sin( theta ) * m_Radius, -halfLength ) );
	}

	Base::Update();
}

void PrimitiveCone::Draw(
	Helium::BufferedDrawer* drawInterface,
	DrawArgs* args,
	Helium::Color materialColor,
	const Simd::Matrix44& transform,
	const bool* solid,
	const bool* transparent ) const
{
	if (transparent ? *transparent : m_IsTransparent)
	{
		if( materialColor.GetA() == 0 )
		{
			materialColor.SetA( 0x80 );
		}
	}

	if (solid ? *solid : m_IsSolid)
	{
		drawInterface->DrawUntextured(
			Helium::RENDERER_PRIMITIVE_TYPE_TRIANGLE_FAN,
			transform,
			m_Buffer,
			NULL,
			GetBaseIndex() + m_WireVertCount,
			m_Steps + 2,
			0,
			m_Steps,
			materialColor );
		drawInterface->DrawUntextured(
			Helium::RENDERER_PRIMITIVE_TYPE_TRIANGLE_FAN,
			transform,
			m_Buffer,
			NULL,
			GetBaseIndex() + m_WireVertCount + m_Steps + 2,
			m_Steps + 2,
			0,
			m_Steps,
			materialColor );
		args->m_TriangleCount += (m_Steps*2);
	}
	else
	{
		drawInterface->DrawUntextured(
			Helium::RENDERER_PRIMITIVE_TYPE_LINE_LIST,
			transform,
			m_Buffer,
			NULL,
			GetBaseIndex(),
			8,
			0,
			4,
			materialColor,
			Helium::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
		drawInterface->DrawUntextured(
			Helium::RENDERER_PRIMITIVE_TYPE_LINE_STRIP,
			transform,
			m_Buffer,
			NULL,
			GetBaseIndex() + 8,
			m_Steps + 2,
			0,
			m_Steps + 1,
			materialColor,
			Helium::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
		args->m_TriangleCount += (m_Steps*2);
	}
}

bool PrimitiveCone::Pick( PickVisitor* pick, const bool* solid ) const
{
	float stepAngle = (float32_t)HELIUM_TWOPI / (float32_t)(m_Steps);
	float sf=0, u=0, v=0;

	Vector3 v0, v1, v2;

	// cone
	for (int i=0; i<m_Steps; i++)
	{
		float theta = (float32_t)(i) * stepAngle;

		v0 = Vector3 (0.0f, 0.0f, m_Length/2.0f);
		v1 = Vector3 ((float32_t)(cos(theta)) * m_Radius, (float32_t)(sin(theta)) * m_Radius, -m_Length/2.0f);
		v2 = Vector3 ((float32_t)(cos(theta+stepAngle)) * m_Radius, (float32_t)(sin(theta+stepAngle)) * m_Radius, -m_Length/2.0f);

		if (pick->PickTriangle(v0, v1, v2))
		{
			return true;
		}
	}

	return false;
}