#include "Precompile.h"
#include "PrimitiveLocator.h"

#include "Pick.h"

using namespace Editor;

PrimitiveLocator::PrimitiveLocator(ResourceTracker* tracker)
: PrimitiveTemplate(tracker)
{
  SetElementCount( 6 );
  SetElementType( ElementTypes::Position );

  m_Length = 1.0f;
}

void PrimitiveLocator::Update()
{
  m_Bounds.minimum = Math::Vector3 (-m_Length, -m_Length, -m_Length);
  m_Bounds.maximum = Math::Vector3 (m_Length, m_Length, m_Length);

  m_Vertices.clear();

  m_Vertices.push_back(Position (-m_Length, 0.0f, 0.0f));
  m_Vertices.push_back(Position (m_Length, 0.0f, 0.0f));

  m_Vertices.push_back(Position (0.0f, -m_Length, 0.0f));
  m_Vertices.push_back(Position (0.0f, m_Length, 0.0f));

  m_Vertices.push_back(Position (0.0f, 0.0f, -m_Length));
  m_Vertices.push_back(Position (0.0f, 0.0f, m_Length));

  __super::Update();
}

void PrimitiveLocator::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
{
  if (!SetState())
    return;

  m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex(), 3);
  args->m_LineCount += 3;
}

bool PrimitiveLocator::Pick( PickVisitor* pick, const bool* solid ) const
{
  for (size_t i=0; i<m_Vertices.size(); i+=2)
  {
    if (pick->PickSegment(m_Vertices[i].m_Position, m_Vertices[i+1].m_Position, 0.0f))
    {
      return true;
    }
  }

  return false;
}