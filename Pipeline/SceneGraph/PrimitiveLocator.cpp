/*#include "Precompile.h"*/
#include "PrimitiveLocator.h"

#include "Pipeline/SceneGraph/Pick.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitiveLocator::PrimitiveLocator()
{
    SetElementCount( 6 );
    SetElementType( VertexElementTypes::SimpleVertex );

    m_Length = 1.0f;
}

void PrimitiveLocator::Update()
{
    m_Bounds.minimum = Vector3 (-m_Length, -m_Length, -m_Length);
    m_Bounds.maximum = Vector3 (m_Length, m_Length, m_Length);

    m_Vertices.clear();

    m_Vertices.push_back( Lunar::SimpleVertex( -m_Length, 0.0f, 0.0f ) );
    m_Vertices.push_back( Lunar::SimpleVertex( m_Length, 0.0f, 0.0f ) );

    m_Vertices.push_back( Lunar::SimpleVertex( 0.0f, -m_Length, 0.0f ) );
    m_Vertices.push_back( Lunar::SimpleVertex( 0.0f, m_Length, 0.0f ) );

    m_Vertices.push_back( Lunar::SimpleVertex( 0.0f, 0.0f, -m_Length ) );
    m_Vertices.push_back( Lunar::SimpleVertex( 0.0f, 0.0f, m_Length ) );

    Base::Update();
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