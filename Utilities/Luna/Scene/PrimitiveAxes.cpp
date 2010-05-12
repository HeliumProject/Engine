#include "Precompile.h"
#include "PrimitiveAxes.h"

#include "Pick.h"

#include "Editor/Orientation.h"

using namespace Math;
using namespace Luna;

PrimitiveAxes::PrimitiveAxes(ResourceTracker* tracker)
: PrimitiveTemplate (tracker)
{
  SetElementCount( 6 );
  SetElementType( ElementTypes::PositionColored );

  m_Length = 50.0f;

  m_ColorX = D3DCOLOR_ARGB(255, 255, 0, 0);
  m_ColorY = D3DCOLOR_ARGB(255, 0, 255, 0);
  m_ColorZ = D3DCOLOR_ARGB(255, 0, 0, 255);

  m_Bounds.minimum = Vector3 (0.0f, 0.0f, 0.0f);
  m_Bounds.maximum = Vector3 (m_Length, m_Length, m_Length);
}

void PrimitiveAxes::SetColor(AxesFlags axes, u32 c)
{
  if ((axes & MultipleAxes::X) == MultipleAxes::X)
    m_ColorX = c;

  if ((axes & MultipleAxes::Y) == MultipleAxes::Y)
    m_ColorY = c;

  if ((axes & MultipleAxes::Z) == MultipleAxes::Z)
    m_ColorZ = c;
}

void PrimitiveAxes::SetColor(u32 c)
{
  m_ColorX = c;
  m_ColorY = c;
  m_ColorZ = c;
}

void PrimitiveAxes::SetRGB()
{
  m_ColorX = D3DCOLOR_ARGB(255, 255, 0, 0);
  m_ColorY = D3DCOLOR_ARGB(255, 0, 255, 0);
  m_ColorZ = D3DCOLOR_ARGB(255, 0, 0, 255);
}

void PrimitiveAxes::Update()
{
  m_Bounds.minimum = Vector3 (0.0f, 0.0f, 0.0f);
  m_Bounds.maximum = Vector3 (m_Length, m_Length, m_Length);

  m_Vertices.clear();
  m_Vertices.push_back(PositionColored (0.0f, 0.0f, 0.0f, m_ColorX));
  m_Vertices.push_back(PositionColored (m_Length, 0.0f, 0.0f, m_ColorX));
  m_Vertices.push_back(PositionColored (0.0f, 0.0f, 0.0f, m_ColorY));
  m_Vertices.push_back(PositionColored (0.0f, m_Length, 0.0f, m_ColorY));
  m_Vertices.push_back(PositionColored (0.0f, 0.0f, 0.0f, m_ColorZ));
  m_Vertices.push_back(PositionColored (0.0f, 0.0f, m_Length, m_ColorZ));

  __super::Update();
}

void PrimitiveAxes::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
{
  if (!SetState())
    return;

  m_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

  m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex(), 3);
  args->m_LineCount += 3;

  m_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void PrimitiveAxes::DrawAxes( DrawArgs* args, AxesFlags axes ) const
{
  if (!SetState())
    return;

  m_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

  if (axes & MultipleAxes::X)
  {
    m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex(), 1);
    args->m_LineCount += 1;
  }

  if (axes & MultipleAxes::Y)
  {
    m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex()+2, 1);
    args->m_LineCount += 1;
  }

  if (axes & MultipleAxes::Z)
  {
    m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex()+4, 1);
    args->m_LineCount += 1;
  }

  m_Device->SetRenderState(D3DRS_LIGHTING, TRUE);
}

void PrimitiveAxes::DrawViewport( DrawArgs* args, const Luna::Camera* camera ) const
{
  Matrix4 projection, inverseProjection;
  camera->GetOrthographicProjection(projection);
  inverseProjection = projection.Inverted();

  // render in view space
  m_Device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix4::Identity);
  m_Device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&Matrix4::Identity);
  m_Device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&projection);

  {
    // get a point in the lower left hand corner in camera coordinates
    Vector3 pos;
    pos.x = (((2.0f * 50) / camera->GetWidth()) - 1);
    pos.y = -(((2.0f * (camera->GetHeight() - 50)) / camera->GetHeight()) - 1);
    pos.z = 1.0f;

    // transform
    inverseProjection.TransformVertex(pos);

    float s = camera->GetOffset();
    Matrix4 world = Matrix4 (Scale (s, s, s)) * camera->GetOrientation() * Matrix4 (pos) * Matrix4 (Vector3 (OutVector * 100));
    m_Device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&world);

    // draw the axes
    Draw( args );
  }

  // restore matrix state
  m_Device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&camera->GetView());
  m_Device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&camera->GetProjection());
}

bool PrimitiveAxes::Pick( PickVisitor* pick, const bool* solid ) const
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

AxesFlags PrimitiveAxes::PickAxis(const Math::Matrix4& transform, Line pick, float err)
{
  Vector3 offset;
  float minX = m_Length, minY = m_Length, minZ = m_Length;

  Vector3 axisOrigin (Vector3::Zero);
  Vector3 axisEnd (m_Length, 0.f, 0.f);

  transform.TransformVertex (axisOrigin);
  transform.TransformVertex (axisEnd);

  if (pick.IntersectsSegment (axisOrigin, axisEnd, err, NULL, &offset))
  {
    f32 dist = offset.Length();
    if (dist > 0.0f && dist < minX)
    {
      minX = dist;
    }
  }

  axisEnd = Vector3(0.f, m_Length, 0.f);
  transform.TransformVertex (axisEnd);

  if (pick.IntersectsSegment (axisOrigin, axisEnd, err, NULL, &offset))
  {
    f32 dist = offset.Length();
    if (dist > 0.0f && dist < minY)
    {
      minY = dist;
    }
  }

  axisEnd = Vector3(0.f, 0.f, m_Length);
  transform.TransformVertex (axisEnd);

  if (pick.IntersectsSegment (axisOrigin, axisEnd, err, NULL, &offset))
  {
    f32 dist = offset.Length();
    if (dist > 0.0f && dist < minZ)
    {
      minZ = dist;
    }
  }

  if ((minX == minY) && (minY == minZ))
  {
    return MultipleAxes::None;
  }

  if (minX <= minY && minX <= minZ)
  {
    return MultipleAxes::X;
  }

  if (minY <= minX && minY <= minZ)
  {
    return MultipleAxes::Y;
  }

  if (minZ <= minX && minZ <= minY)
    return MultipleAxes::Z;

  return MultipleAxes::None;
}