/*#include "Precompile.h"*/
#include "PrimitivePointer.h"

#include "Pipeline/SceneGraph/Pick.h"

#include "Orientation.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitivePointer::PrimitivePointer()
{
    SetElementCount( 12 );
    SetElementType( VertexElementTypes::SimpleVertex );

    m_Bounds.minimum = SetupVector(-1, 0, -2);
    m_Bounds.maximum = SetupVector( 1, 1,  0);
}

void PrimitivePointer::Update()
{
    m_Vertices.clear();

    Vector3 position;

    position = SetupVector( -1.0f, 0.0f, -2.0f );
    m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    position = SetupVector( 0.0f, 1.0f, -2.0f );
    m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    position = SetupVector( 1.0f, 0.0f, -2.0f );
    m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    position = SetupVector( 0.0f, 0.0f, 0.0f );
    m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    position = SetupVector( -1.0f, 0.0f, -2.0f );
    m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    position = SetupVector( 0.0f, 1.0f, -2.0f );
    m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    position = SetupVector( 0.0f, 0.0f, 0.0f );
    m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    position = SetupVector( -1.0f, 0.0f, -2.0f );
    m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    position = SetupVector( 1.0f, 0.0f, -2.0f );
    m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );

    Base::Update();
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