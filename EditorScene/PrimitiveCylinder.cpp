#include "SceneGraphPch.h"
#include "PrimitiveCylinder.h"

#include "Graphics/BufferedDrawer.h"
#include "SceneGraph/Pick.h"

#include "Orientation.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitiveCylinder::PrimitiveCylinder()
	: m_VerticalOrientation( true )
{
	m_Length = 2.0f;
	m_LengthSteps = 5;
}

int PrimitiveCylinder::GetWireVertCount() const
{
	return m_RadiusSteps*2 * m_LengthSteps;
}

int PrimitiveCylinder::GetPolyVertCount() const
{
	return (m_RadiusSteps*2)+2 + (m_RadiusSteps+2)*2;
}

void PrimitiveCylinder::Update()
{
	m_Bounds.minimum = Vector3 (-m_Radius, -m_Length/2.f, -m_Radius);
	m_Bounds.maximum = Vector3 (m_Radius, m_Length/2.f, m_Radius);

	SetElementCount( GetWireVertCount() + GetPolyVertCount() );
	m_Vertices.clear();

	float sideValue = 0.0f;
	float upValue = 0.0f;

	Vector3 position;

	//
	// Wire
	//

	float stepAngle = (float32_t)HELIUM_TWOPI / (float32_t)(m_RadiusSteps);
	float stepLength = m_Length/(float32_t)(m_LengthSteps-1);

	for (int l=0; l<m_LengthSteps; l++)
	{
		for (int s=0; s<m_RadiusSteps; s++)
		{
			float theta = (float32_t)(s) * stepAngle;

			sideValue = m_VerticalOrientation ? Sin(theta) * m_Radius : -m_Length/2.0f + stepLength*(float32_t)(l);
			upValue   = m_VerticalOrientation ? -m_Length/2.0f + stepLength*(float32_t)(l) : Sin(theta) * m_Radius;

			position = SetupVector( sideValue, upValue, Cos(theta) * m_Radius );
			m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );

			sideValue = m_VerticalOrientation ? Sin(theta + stepAngle) * m_Radius : -m_Length/2.0f + stepLength*(float32_t)(l);
			upValue   = m_VerticalOrientation ? -m_Length/2.0f + stepLength*(float32_t)(l) : Sin(theta + stepAngle) * m_Radius;

			position = SetupVector( sideValue, upValue, Cos(theta + stepAngle) * m_Radius );
			m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
		}
	}

	//
	// Poly
	//

	// sides
	for (int x=0; x<=m_RadiusSteps; x++)
	{
		float theta = (float32_t)(x) * stepAngle;

		sideValue = m_VerticalOrientation ? Sin(theta) * m_Radius : m_Length/2.0f;
		upValue   = m_VerticalOrientation ? m_Length/2.0f : Sin(theta) * m_Radius;

		position = SetupVector( sideValue, upValue, Cos(theta) * m_Radius );
		m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );

		sideValue = m_VerticalOrientation ? Sin(theta) * m_Radius : -m_Length/2.0f;
		upValue   = m_VerticalOrientation ? -m_Length/2.0f : Sin(theta) * m_Radius;

		position = SetupVector( sideValue, upValue, Cos(theta) * m_Radius );
		m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	}

	// top
	position = SetupVector( m_Length/2.0f, 0.0f, 0.0f );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	for (int x=0; x<=m_RadiusSteps; x++)
	{
		float theta = (float32_t)(x) * stepAngle;

		sideValue = m_VerticalOrientation ? Sin(theta) * m_Radius : m_Length/2.0f;
		upValue   = m_VerticalOrientation ?  m_Length/2.0f : Sin(theta) * m_Radius;

		position = SetupVector( sideValue, upValue, Cos(theta) * m_Radius );
		m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	}

	// bottom, construct backward to wrap polys correctly
	position = SetupVector( -m_Length/2.0f, 0.0f, 0.0f );
	m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	for (int x=m_RadiusSteps; x>=0; x--)
	{
		float theta = (float32_t)(x) * stepAngle;

		sideValue = m_VerticalOrientation ? Sin(theta) * m_Radius : -m_Length/2.0f;
		upValue   = m_VerticalOrientation ?  -m_Length/2.0f : Sin(theta) * m_Radius;

		position = SetupVector( sideValue, upValue, Cos(theta) * m_Radius );
		m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
	}

	Base::Update();
}

void PrimitiveCylinder::Draw(
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
			Helium::RENDERER_PRIMITIVE_TYPE_TRIANGLE_STRIP,
			transform,
			m_Buffer,
			NULL,
			GetBaseIndex() + GetWireVertCount(),
			m_RadiusSteps * 2 + 2,
			0,
			m_RadiusSteps * 2,
			materialColor );
		args->m_TriangleCount += (m_RadiusSteps*2);
		drawInterface->DrawUntextured(
			Helium::RENDERER_PRIMITIVE_TYPE_TRIANGLE_FAN,
			transform,
			m_Buffer,
			NULL,
			GetBaseIndex() + GetWireVertCount() + m_RadiusSteps * 2 + 2,
			m_RadiusSteps + 2,
			0,
			m_RadiusSteps,
			materialColor );
		args->m_TriangleCount += (m_RadiusSteps);
		drawInterface->DrawUntextured(
			Helium::RENDERER_PRIMITIVE_TYPE_TRIANGLE_FAN,
			transform,
			m_Buffer,
			NULL,
			GetBaseIndex() + GetWireVertCount() + m_RadiusSteps * 3 + 4,
			m_RadiusSteps + 2,
			0,
			m_RadiusSteps,
			materialColor );
		args->m_TriangleCount += (m_RadiusSteps);
	}
	else
	{
		drawInterface->DrawUntextured(
			Helium::RENDERER_PRIMITIVE_TYPE_LINE_LIST,
			transform,
			m_Buffer,
			NULL,
			GetBaseIndex(),
			m_RadiusSteps * m_LengthSteps + 2,
			0,
			m_RadiusSteps * m_LengthSteps,
			materialColor,
			Helium::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
		args->m_LineCount += (m_RadiusSteps*m_LengthSteps);
	}
}

bool PrimitiveCylinder::Pick( PickVisitor* pick, const bool* solid ) const
{
	if (solid ? *solid : m_IsSolid)
	{
		return pick->PickSegment(SetupVector(0.0f,-m_Length/2.0f,0.0f), SetupVector(0.0f,m_Length/2.0f,0.0f), m_Radius);
	}
	else
	{
		if (pick->PickSegment(SetupVector(0.0f, -m_Length/2.0f, 0.0f), SetupVector(0.0f, m_Length/2.0f, 0.0f), m_Radius))
		{
			return (pick->GetHits().back()->GetIntersectionDistance() < m_Radius);
		}
	}

	return false;
}