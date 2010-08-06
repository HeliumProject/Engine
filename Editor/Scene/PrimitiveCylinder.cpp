#include "Precompile.h"
#include "PrimitiveCylinder.h"

#include "Editor/Scene/Pick.h"

#include "Orientation.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Editor;

PrimitiveCylinder::PrimitiveCylinder(ResourceTracker* tracker)
: PrimitiveRadius(tracker)
, m_VerticalOrientation( true )
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

  //
  // Wire
  //

  float stepAngle = (f32)(Math::Pi)*2.0f / (f32)(m_RadiusSteps);
  float stepLength = m_Length/(f32)(m_LengthSteps-1);

  for (int l=0; l<m_LengthSteps; l++)
  {
    for (int s=0; s<m_RadiusSteps; s++)
    {
      float theta = (f32)(s) * stepAngle;

      sideValue = m_VerticalOrientation ? (f32)(sin(theta)) * m_Radius : -m_Length/2.0f + stepLength*(f32)(l);
      upValue   = m_VerticalOrientation ? -m_Length/2.0f + stepLength*(f32)(l) : (f32)(sin(theta)) * m_Radius;

      m_Vertices.push_back(Position(SetupVector( sideValue, upValue, (f32)(cos(theta)) * m_Radius)) );

      sideValue = m_VerticalOrientation ? (f32)(sin(theta + stepAngle)) * m_Radius : -m_Length/2.0f + stepLength*(f32)(l);
      upValue   = m_VerticalOrientation ? -m_Length/2.0f + stepLength*(f32)(l) : (f32)(sin(theta + stepAngle)) * m_Radius;

      m_Vertices.push_back(Position(SetupVector( sideValue, upValue, (f32)(cos(theta + stepAngle)) * m_Radius)));
    }
  }

  //
  // Poly
  //

  // sides
  for (int x=0; x<=m_RadiusSteps; x++)
  {
    float theta = (f32)(x) * stepAngle;

    sideValue = m_VerticalOrientation ? (f32)(sin(theta)) * m_Radius : m_Length/2.0f;
    upValue   = m_VerticalOrientation ? m_Length/2.0f : (f32)(sin(theta)) * m_Radius;

    m_Vertices.push_back(Position (SetupVector( sideValue, upValue, (f32)(cos(theta)) * m_Radius)));

    sideValue = m_VerticalOrientation ? (f32)(sin(theta)) * m_Radius : -m_Length/2.0f;
    upValue   = m_VerticalOrientation ? -m_Length/2.0f : (f32)(sin(theta)) * m_Radius;

    m_Vertices.push_back(Position (SetupVector( sideValue, upValue, (f32)(cos(theta)) * m_Radius)));
  }

  // top
  m_Vertices.push_back(Position (SetupVector(m_Length/2.0f, 0.0f, 0.0f)));
  for (int x=0; x<=m_RadiusSteps; x++)
  {
    float theta = (f32)(x) * stepAngle;

    sideValue = m_VerticalOrientation ? (f32)(sin(theta)) * m_Radius : m_Length/2.0f;
    upValue   = m_VerticalOrientation ?  m_Length/2.0f : (f32)(sin(theta)) * m_Radius;

    m_Vertices.push_back(Position (SetupVector( sideValue, upValue, (f32)(cos(theta)) * m_Radius)));
  }

  // bottom, construct backward to wrap polys correctly
  m_Vertices.push_back(Position (SetupVector(-m_Length/2.0f, 0.0f, 0.0f)));
  for (int x=m_RadiusSteps; x>=0; x--)
  {
    float theta = (f32)(x) * stepAngle;

    sideValue = m_VerticalOrientation ? (f32)(sin(theta)) * m_Radius : -m_Length/2.0f;
    upValue   = m_VerticalOrientation ?  -m_Length/2.0f : (f32)(sin(theta)) * m_Radius;

    m_Vertices.push_back(Position (SetupVector( sideValue, upValue, (f32)(cos(theta)) * m_Radius)));
  }

  __super::Update();
}

void PrimitiveCylinder::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
{
  if (!SetState())
    return;

  if (transparent ? *transparent : m_IsTransparent)
  {
    D3DMATERIAL9 m;
    ZeroMemory(&m, sizeof(m));

    m_Device->GetMaterial(&m);
    m.Ambient.a = m.Ambient.a < 0.0001 ? 0.5f : m.Ambient.a;
    m.Diffuse = m.Ambient;
    m_Device->SetMaterial(&m);

    m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  }

  if (solid ? *solid : m_IsSolid)
  {
    m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, (UINT)(GetBaseIndex() + GetWireVertCount()), m_RadiusSteps*2);
    args->m_TriangleCount += (m_RadiusSteps*2);
    m_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, (UINT)((GetBaseIndex() + m_RadiusSteps*2+2) + GetWireVertCount()), m_RadiusSteps);
    args->m_TriangleCount += (m_RadiusSteps);
    m_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, (UINT)((GetBaseIndex() + m_RadiusSteps*2+2+m_RadiusSteps+2) + GetWireVertCount()), m_RadiusSteps);
    args->m_TriangleCount += (m_RadiusSteps);
  }
  else
  {
    m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex(), m_RadiusSteps*m_LengthSteps);
    args->m_LineCount += (m_RadiusSteps*m_LengthSteps);
  }

  if (transparent ? *transparent : m_IsTransparent)
  {
    m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
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