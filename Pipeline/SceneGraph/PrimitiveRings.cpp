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

    float stepAngle = (float32_t)HELIUM_TWOPI / (float32_t)(m_Steps);

    for (uint32_t x=0; x<m_Steps; x++)
    {
        float theta = (float32_t)(x) * stepAngle;
        m_Vertices.push_back(Position(SetupVector(0.0f, (float32_t)(cos(theta)) * m_Radius, (float32_t)(sin(theta)) * m_Radius)));
        m_Vertices.push_back(Position(SetupVector(0.0f, (float32_t)(cos(theta + stepAngle)) * m_Radius, (float32_t)(sin(theta + stepAngle)) * m_Radius)));
    }

    for (uint32_t y=0; y<m_Steps; y++)
    {
        float theta = (float32_t)(y) * stepAngle;
        m_Vertices.push_back(Position(SetupVector((float32_t)(cos(theta)) * m_Radius, 0.0f, (float32_t)(sin(theta)) * m_Radius)));
        m_Vertices.push_back(Position(SetupVector((float32_t)(cos(theta + stepAngle)) * m_Radius, 0.0f, (float32_t)(sin(theta + stepAngle)) * m_Radius)));
    }

    for (uint32_t z=0; z<m_Steps; z++)
    {
        float theta = (float32_t)(z) * stepAngle;
        m_Vertices.push_back(Position(SetupVector((float32_t)(cos(theta)) * m_Radius, (float32_t)(sin(theta)) * m_Radius, 0.0f)));
        m_Vertices.push_back(Position(SetupVector((float32_t)(cos(theta + stepAngle)) * m_Radius, (float32_t)(sin(theta + stepAngle)) * m_Radius, 0.0f)));
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