/*#include "Precompile.h"*/
#include "PrimitiveRings.h"

#include "Pipeline/SceneGraph/Pick.h"

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

    float32_t stepAngle = (float32_t)HELIUM_TWOPI / (float32_t)(m_Steps);

    for (uint32_t x=0; x<m_Steps; x++)
    {
        float32_t theta = (float32_t)(x) * stepAngle;
        Vector3 position = SetupVector( 0.0f, Cos( theta ) * m_Radius, Sin( theta ) * m_Radius );
        m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
        position = SetupVector( 0.0f, Cos( theta + stepAngle ) * m_Radius, Sin( theta + stepAngle ) * m_Radius );
        m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    }

    for (uint32_t y=0; y<m_Steps; y++)
    {
        float32_t theta = (float32_t)(y) * stepAngle;
        Vector3 position = SetupVector( Cos( theta ) * m_Radius, 0.0f, Sin( theta ) * m_Radius );
        m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
        position = SetupVector( Cos( theta + stepAngle ) * m_Radius, 0.0f, Sin( theta + stepAngle ) * m_Radius );
        m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    }

    for (uint32_t z=0; z<m_Steps; z++)
    {
        float32_t theta = (float32_t)(z) * stepAngle;
        Vector3 position = SetupVector( Cos( theta ) * m_Radius, Sin( theta ) * m_Radius, 0.0f );
        m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
        position = SetupVector( Cos( theta + stepAngle ) * m_Radius, Sin( theta + stepAngle ) * m_Radius, 0.0f );
        m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
    }

    Base::Update();
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