/*#include "Precompile.h"*/
#include "PrimitivePointer.h"

#include "Core/Scene/Pick.h"

#include "Orientation.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;

PrimitivePointer::PrimitivePointer(ResourceTracker* tracker)
: PrimitiveTemplate(tracker)
{
  SetElementCount( 12 );
  SetElementType( ElementTypes::Position );

  m_Bounds.minimum = SetupVector(-1, 0, -2);
  m_Bounds.maximum = SetupVector( 1, 1,  0);
}

void PrimitivePointer::Update()
{
  m_Vertices.clear();

  m_Vertices.push_back(Position(SetupVector(-1, 0, -2)));
  m_Vertices.push_back(Position(SetupVector( 0, 1, -2)));
  m_Vertices.push_back(Position(SetupVector( 0, 1, -2)));
  m_Vertices.push_back(Position(SetupVector( 1, 0, -2)));
  m_Vertices.push_back(Position(SetupVector( 1, 0, -2)));
  m_Vertices.push_back(Position(SetupVector( 0, 0,  0)));
  m_Vertices.push_back(Position(SetupVector( 0, 0,  0)));
  m_Vertices.push_back(Position(SetupVector(-1, 0, -2)));
  m_Vertices.push_back(Position(SetupVector( 0, 1, -2)));
  m_Vertices.push_back(Position(SetupVector( 0, 0,  0)));
  m_Vertices.push_back(Position(SetupVector(-1, 0, -2)));
  m_Vertices.push_back(Position(SetupVector( 1, 0, -2)));

  __super::Update();
}

void PrimitivePointer::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
{
  if (!SetState())
    return;

  m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex(), 6);
  args->m_LineCount += 6;
}

bool PrimitivePointer::Pick( PickVisitor* pick, const bool* solid ) const
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