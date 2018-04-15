#include "Precompile.h"
#include "PrimitiveCircle.h"

#include "Graphics/BufferedDrawer.h"
#include "EditorScene/Pick.h"

using namespace Helium;
using namespace Helium::Editor;

PrimitiveCircle::PrimitiveCircle()
	: m_HackyRotateFlag( false )
{
	m_Bounds.minimum = Vector3 (0.0f, -m_Radius, -m_Radius);
	m_Bounds.maximum = Vector3 (0.0f, m_Radius, m_Radius);
}

void PrimitiveCircle::Update()
{
	m_Bounds.minimum = Vector3 (-m_Radius, 0.0f, 0.0f);
	m_Bounds.maximum = Vector3 (m_Radius, 0.0f, 0.0f);

	SetElementCount( m_RadiusSteps*2 + (m_RadiusSteps + 2) );
	m_Vertices.clear();

	float stepAngle = (float32_t)HELIUM_TWOPI / (float32_t)(m_RadiusSteps);

	for (int x=0; x<m_RadiusSteps; x++)
	{
		float theta = (float32_t)(x) * stepAngle;

		if( !m_HackyRotateFlag )
		{
			m_Vertices.push_back( Helium::SimpleVertex( 0.0f, Cos( theta ) * m_Radius, Sin( theta ) * m_Radius ) );
			m_Vertices.push_back( Helium::SimpleVertex( 0.0f, Cos( theta + stepAngle ) * m_Radius, Sin( theta + stepAngle ) * m_Radius ) );
		}
		else
		{
			m_Vertices.push_back( Helium::SimpleVertex( Sin( theta ) * m_Radius, Cos( theta ) * m_Radius, 0.0f ) );
			m_Vertices.push_back( Helium::SimpleVertex( Sin( theta + stepAngle ) * m_Radius, Cos( theta + stepAngle ) * m_Radius, 0.0f ) );
		}    
	}

	m_Vertices.push_back( Helium::SimpleVertex( 0.0f, 0.0f, 0.0f ) );

	for (int x=0; x<m_RadiusSteps; x++)
	{
		float theta = (float32_t)(x) * stepAngle;

		if( !m_HackyRotateFlag )
		{
			m_Vertices.push_back( Helium::SimpleVertex( 0.0f, Cos( theta ) * m_Radius, Sin( theta ) * m_Radius ) );
		}
		else
		{
			m_Vertices.push_back( Helium::SimpleVertex( Sin( theta ) * m_Radius, Cos( theta ) * m_Radius, 0.0f ) );
		}

		if (x+1 >= m_RadiusSteps)
		{
			if( !m_HackyRotateFlag )
			{
				m_Vertices.push_back( Helium::SimpleVertex( 0.0f, Cos( theta + stepAngle ) * m_Radius, Sin( theta + stepAngle ) * m_Radius ) );
			}
			else
			{
				m_Vertices.push_back( Helium::SimpleVertex( Sin( theta + stepAngle ) * m_Radius, Cos( theta + stepAngle ) * m_Radius, 0.0f ) );
			}
		}
	}

	Base::Update();
}

void PrimitiveCircle::Draw(
	BufferedDrawer* drawInterface,
	Helium::Color materialColor,
	const Simd::Matrix44& transform,
	const bool* solid,
	const bool* transparent ) const
{
	drawInterface->DrawUntextured(
		Helium::RENDERER_PRIMITIVE_TYPE_LINE_LIST,
		transform,
		m_Buffer,
		NULL,
		GetBaseIndex(),
		m_RadiusSteps * 2,
		0,
		m_RadiusSteps,
		materialColor,
		Helium::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
}

void PrimitiveCircle::DrawFill(
	BufferedDrawer* drawInterface,
	Helium::Color materialColor,
	const Simd::Matrix44& transform ) const
{
	drawInterface->DrawUntextured(
		Helium::RENDERER_PRIMITIVE_TYPE_TRIANGLE_FAN,
		transform,
		m_Buffer,
		NULL,
		GetBaseIndex() + m_RadiusSteps * 2,
		m_RadiusSteps + 2,
		0,
		m_RadiusSteps,
		materialColor,
		Helium::RenderResourceManager::RASTERIZER_STATE_DOUBLE_SIDED );
}

void PrimitiveCircle::DrawHiddenBack(const Editor::Camera* camera, const Matrix4& m) const
{
#ifdef VIEWPORT_REFACTOR
	if (!SetState())
		return;

	int i = 0, count = 0;
	float stepAngle = (float32_t)HELIUM_TWOPI / (float32_t)(m_RadiusSteps);

	Vector3 position (m.t.x, m.t.y, m.t.z);

	Vector3 cameraVector;
	camera->GetPosition(cameraVector);
	cameraVector -= position;
	cameraVector.Normalize();

	for (int x=0; x<m_RadiusSteps; x++)
	{
		float theta = (float32_t)(x) * stepAngle;

		// circle point 1
		Vector3 v1 (0.0f, (float32_t)(cos(theta)) * m_Radius, (float32_t)(sin(theta)) * m_Radius);

		// circle point 2
		Vector3 v2 (0.0f, (float32_t)(cos(theta + stepAngle)) * m_Radius, (float32_t)(sin(theta + stepAngle)) * m_Radius);

		// middle point of circle segment
		Vector3 v = (v1 + v2) * 0.5f;

		// in global space
		m.TransformVertex(v);

		v -= position;
		v.Normalize();

		// if not pointing away from the camera vector, render
		if (v.Dot(cameraVector) > 1.0f - HELIUM_CRITICAL_DOT_PRODUCT)
		{
			m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex()+i, 1);
			count++;
		}

		// increment vertex offset
		i+=2;
	}

	args->m_LineCount += count;
#endif
}

bool PrimitiveCircle::Pick( PickVisitor* pick, const bool* solid ) const
{
	for (size_t i=0; i<m_Vertices.size(); i+=2)
	{
		if (pick->PickSegment( (const Vector3&)m_Vertices[i].position, (const Vector3&)m_Vertices[i+1].position))
		{
			return true;
		}
	}

	return false;
}