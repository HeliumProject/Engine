#include "Precompile.h"
#include "PrimitiveCircle.h"

#include "Pick.h"

using namespace Math;
using namespace Editor;

PrimitiveCircle::PrimitiveCircle(ResourceTracker* tracker)
: PrimitiveRadius(tracker)
, m_HackyRotateFlag( false )
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

  float stepAngle = (f32)(Math::Pi)*2.0f / (f32)(m_RadiusSteps);

  for (int x=0; x<m_RadiusSteps; x++)
  {
    float theta = (f32)(x) * stepAngle;

    if( !m_HackyRotateFlag )
    {
      m_Vertices.push_back(Position (0.0f, (f32)(cos(theta)) * m_Radius, (f32)(sin(theta)) * m_Radius));
      m_Vertices.push_back(Position (0.0f, (f32)(cos(theta + stepAngle)) * m_Radius, (f32)(sin(theta + stepAngle)) * m_Radius));
    }
    else
    {
      m_Vertices.push_back(Position ((f32)(sin(theta)) * m_Radius, (f32)(cos(theta)) * m_Radius, 0.0f ) );
      m_Vertices.push_back(Position ((f32)(sin(theta + stepAngle)) * m_Radius, (f32)(cos(theta + stepAngle)) * m_Radius, 0.0f ) );
    }    
  }

  m_Vertices.push_back(Position (Vector3::Zero));

  for (int x=0; x<m_RadiusSteps; x++)
  {
    float theta = (f32)(x) * stepAngle;

    if( !m_HackyRotateFlag )
    {
      m_Vertices.push_back(Position (0.0f, (f32)(cos(theta)) * m_Radius, (f32)(sin(theta)) * m_Radius));
    }
    else
    {
      m_Vertices.push_back(Position ((f32)(sin(theta)) * m_Radius, (f32)(cos(theta)) * m_Radius, 0.0f ) );
    }

    if (x+1 >= m_RadiusSteps)
    {
      if( !m_HackyRotateFlag )
      {
        m_Vertices.push_back(Position (0.0f, (f32)(cos(theta + stepAngle)) * m_Radius, (f32)(sin(theta + stepAngle)) * m_Radius));
      }
      else
      {
        m_Vertices.push_back(Position ((f32)(sin(theta + stepAngle)) * m_Radius, (f32)(cos(theta + stepAngle)) * m_Radius, 0.0f  ) );
      }
    }
  }

  __super::Update();
}

void PrimitiveCircle::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
{
  if (!SetState())
    return;

  m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex(), m_RadiusSteps);

  args->m_LineCount += m_RadiusSteps;
}

void PrimitiveCircle::DrawFill( DrawArgs* args ) const
{
  if (!SetState())
    return;

  D3DCULL cull;
  m_Device->GetRenderState(D3DRS_CULLMODE, (DWORD*)&cull);
  {
    m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, (UINT)GetBaseIndex() + (m_RadiusSteps * 2), m_RadiusSteps);
    m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  }
  m_Device->SetRenderState(D3DRS_CULLMODE, cull);

  args->m_TriangleCount += m_RadiusSteps;
}

void PrimitiveCircle::DrawHiddenBack(DrawArgs* args, const Editor::Camera* camera, const Matrix4& m) const
{
  if (!SetState())
    return;

  int i = 0, count = 0;
  float stepAngle = (f32)(Math::Pi)*2.0f / (f32)(m_RadiusSteps);
  
  Vector3 position (m.t.x, m.t.y, m.t.z);

  Vector3 cameraVector;
  camera->GetPosition(cameraVector);
  cameraVector -= position;
  cameraVector.Normalize();

  for (int x=0; x<m_RadiusSteps; x++)
  {
    float theta = (f32)(x) * stepAngle;

    // circle point 1
    Vector3 v1 (0.0f, (f32)(cos(theta)) * m_Radius, (f32)(sin(theta)) * m_Radius);

    // circle point 2
    Vector3 v2 (0.0f, (f32)(cos(theta + stepAngle)) * m_Radius, (f32)(sin(theta + stepAngle)) * m_Radius);

    // middle point of circle segment
    Vector3 v = (v1 + v2) * 0.5f;

    // in global space
    m.TransformVertex(v);

    v -= position;
    v.Normalize();

    // if not pointing away from the camera vector, render
    if (v.Dot(cameraVector) > 1.0f - Math::CriticalDotProduct)
    {
      m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex()+i, 1);
      count++;
    }

    // increment vertex offset
    i+=2;
  }

  args->m_LineCount += count;
}

bool PrimitiveCircle::Pick( PickVisitor* pick, const bool* solid ) const
{
  for (size_t i=0; i<m_Vertices.size(); i+=2)
  {
    if (pick->PickSegment(m_Vertices[i].m_Position, m_Vertices[i+1].m_Position))
    {
      return true;
    }
  }

  return false;
}