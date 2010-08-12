/*#include "Precompile.h"*/
#include "PrimitiveCube.h"

#include "Core/Scene/Pick.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;

PrimitiveCube::PrimitiveCube(ResourceTracker* tracker)
: PrimitiveTemplate(tracker)
{
  SetElementType( ElementTypes::Position );
  SetElementCount( 60 );

  m_Bounds.minimum = Vector3 (-1.0f, -1.0f, -1.0f);
  m_Bounds.maximum = Vector3 (1.0f, 1.0f, 1.0f);
}

void PrimitiveCube::Update()
{
  m_Vertices.clear();

  V_Vector3 vertices;
  m_Bounds.GetVertices( vertices );
  m_Bounds.GetWireframe( vertices, (V_Vector3&)m_Vertices, false );
  m_Bounds.GetTriangulated( vertices, (V_Vector3&)m_Vertices, false );

  __super::Update();
}

void PrimitiveCube::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
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
    m_Device->DrawPrimitive(D3DPT_TRIANGLELIST, (UINT)(GetBaseIndex() + 24), 12);
    args->m_TriangleCount += 12;
  }
  else
  {
    m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex(), 12);
    args->m_LineCount += 12;
  }

  if (transparent ? *transparent : m_IsTransparent)
  {
    m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
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
      if (pick->PickSegment(m_Vertices[i].m_Position, m_Vertices[i+1].m_Position))
      {
        return true;
      }
    }

    return false;
  }

  return false;
}