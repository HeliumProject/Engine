/*#include "Precompile.h"*/
#include "PrimitiveRings.h"

#include "Core/SceneGraph/Pick.h"

#include "Orientation.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitiveRings::PrimitiveRings(ResourceTracker* tracker)
: PrimitiveTemplate(tracker)
{
  SetElementType( ElementTypes::Position );

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

  float stepAngle = (f32)(Pi)*2.0f / (f32)(m_Steps);

  for (u32 x=0; x<m_Steps; x++)
  {
    float theta = (f32)(x) * stepAngle;
    m_Vertices.push_back(Position(SetupVector(0.0f, (f32)(cos(theta)) * m_Radius, (f32)(sin(theta)) * m_Radius)));
    m_Vertices.push_back(Position(SetupVector(0.0f, (f32)(cos(theta + stepAngle)) * m_Radius, (f32)(sin(theta + stepAngle)) * m_Radius)));
  }

  for (u32 y=0; y<m_Steps; y++)
  {
    float theta = (f32)(y) * stepAngle;
    m_Vertices.push_back(Position(SetupVector((f32)(cos(theta)) * m_Radius, 0.0f, (f32)(sin(theta)) * m_Radius)));
    m_Vertices.push_back(Position(SetupVector((f32)(cos(theta + stepAngle)) * m_Radius, 0.0f, (f32)(sin(theta + stepAngle)) * m_Radius)));
  }

  for (u32 z=0; z<m_Steps; z++)
  {
    float theta = (f32)(z) * stepAngle;
    m_Vertices.push_back(Position(SetupVector((f32)(cos(theta)) * m_Radius, (f32)(sin(theta)) * m_Radius, 0.0f)));
    m_Vertices.push_back(Position(SetupVector((f32)(cos(theta + stepAngle)) * m_Radius, (f32)(sin(theta + stepAngle)) * m_Radius, 0.0f)));
  }

  __super::Update();
}

void PrimitiveRings::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
{
  if (!SetState())
    return;

  m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex(), m_Steps*3);
  args->m_LineCount += (m_Steps*3);
}

bool PrimitiveRings::Pick( PickVisitor* pick, const bool* solid ) const
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