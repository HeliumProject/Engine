/*#include "Precompile.h"*/
#include "PrimitiveSphere.h"

#include "Pipeline/SceneGraph/Pick.h"
#include "Orientation.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitiveSphere::PrimitiveSphere(ResourceTracker* tracker)
: PrimitiveRadius(tracker)
{
    m_LengthSteps = 9;
}

int32_t PrimitiveSphere::GetWireVertCount() const
{
    if (m_RadiusSteps == 0 || m_LengthSteps == 0)
    {
        return m_WireVertCount = 0;
    }
    else
    {
        int32_t count = 0;
        int32_t dphi = 180 / m_LengthSteps;
        int32_t dtheta = 360 / m_RadiusSteps;

        for (int32_t phi=-90; phi<=90; phi+=dphi)
            for (int32_t theta=0; theta<=360-dtheta; theta+=dtheta)
                count+=2;

        return m_WireVertCount = count;
    }
}

int32_t PrimitiveSphere::GetPolyVertCount() const
{
    if (m_RadiusSteps == 0)
    {
        return m_PolyVertCount = 0;
    }
    else
    {
        int32_t count = 0;
        int32_t dtheta = 360 / m_RadiusSteps;
        int32_t dphi = 360 / m_RadiusSteps;

        for (int32_t theta=-90; theta<=90-dtheta; theta+=dtheta)
        {
            for (int32_t phi=0; phi<=360-dphi; phi+=dphi)
            {
                count+=3;

                if (theta > -90 && theta < 90)
                    count +=3;
            }
        }

        return m_PolyVertCount = count;
    }
}

void PrimitiveSphere::Update()
{
    m_Bounds.minimum = Vector3 (-m_Radius, -m_Radius, -m_Radius);
    m_Bounds.maximum = Vector3 (m_Radius, m_Radius, m_Radius);

    SetElementCount( GetWireVertCount() + GetPolyVertCount() );
    m_Vertices.clear();


    //
    // Wire
    //

    int32_t dphi = 180 / m_LengthSteps;
    int32_t dtheta = 360 / m_RadiusSteps;

    for (int32_t phi=-90; phi<=90; phi+=dphi)
    {
        for (int32_t theta=0; theta<=360-dtheta; theta+=dtheta)
        {
            float32_t sinTheta = Sin( theta * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
            float32_t sinTheta2 = Sin( ( theta + dtheta ) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
            float32_t cosTheta = Cos( theta * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
            float32_t cosTheta2 = Cos( ( theta + dtheta ) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );

            float32_t sinPhi = Sin( phi * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
            float32_t cosPhi = Cos( phi * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );

            Vector3 position = SetupVector(sinTheta * cosPhi * m_Radius,
                sinPhi * m_Radius,
                cosTheta * cosPhi * m_Radius);
            m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );

            position = SetupVector(sinTheta2 * cosPhi * m_Radius,
                sinPhi * m_Radius,
                cosTheta2 * cosPhi * m_Radius);
            m_Vertices.push_back( Lunar::SimpleVertex( position.x, position.y, position.z ) );
        }
    }


    //
    // Poly
    //

    if (m_RadiusSteps > 0)
    {
        dtheta = 360 / m_RadiusSteps;
        dphi = 360 / m_RadiusSteps;

        for (int32_t theta=-90; theta<=90-dtheta; theta+=dtheta)
        {
            for (int32_t phi=0; phi<=360-dphi; phi+=dphi)
            {
                float32_t sinTheta = Sin( theta * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
                float32_t sinTheta2 = Sin( ( theta + dtheta ) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
                float32_t cosTheta = Cos( theta * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
                float32_t cosTheta2 = Cos( ( theta + dtheta ) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );

                float32_t sinPhi = Sin( phi * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
                float32_t sinPhi2 = Sin( ( phi + dphi ) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
                float32_t cosPhi = Cos( phi * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
                float32_t cosPhi2 = Cos( ( phi + dphi ) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );

                Lunar::SimpleVertex a(
                    cosTheta * cosPhi * m_Radius,
                    cosTheta * sinPhi * m_Radius,
                    sinTheta * m_Radius );
                m_Vertices.push_back( a );

                Lunar::SimpleVertex b(
                    cosTheta2 * cosPhi2 * m_Radius,
                    cosTheta2 * sinPhi2 * m_Radius,
                    sinTheta2 * m_Radius );
                m_Vertices.push_back( b );

                Lunar::SimpleVertex vertex(
                    cosTheta2 * cosPhi * m_Radius,
                    cosTheta2 * sinPhi * m_Radius,
                    sinTheta2 * m_Radius );
                m_Vertices.push_back( vertex );

                if (theta > -90 && theta < 90)
                {
                    m_Vertices.push_back( b );

                    m_Vertices.push_back( a );

                    vertex.position[ 0 ] = cosTheta * cosPhi2 * m_Radius;
                    vertex.position[ 1 ] = cosTheta * sinPhi2 * m_Radius;
                    vertex.position[ 2 ] = sinTheta * m_Radius;
                    m_Vertices.push_back( vertex );
                }
            }
        }
    }

    Base::Update();
}

void PrimitiveSphere::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
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
        m_Device->DrawPrimitive(D3DPT_TRIANGLELIST, (UINT)GetBaseIndex() + m_WireVertCount, m_PolyVertCount/3);
        args->m_TriangleCount += (m_PolyVertCount/3);
    }
    else
    {
        m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex(), m_WireVertCount/2);
        args->m_LineCount += (m_WireVertCount/2);
    }

    if (transparent ? *transparent : m_IsTransparent)
    {
        m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }
}

bool PrimitiveSphere::Pick( PickVisitor* pick, const bool* solid ) const
{
    if (solid ? *solid : m_IsSolid)
    {
        return pick->PickSphere (Vector3::Zero, m_Radius);
    }
    else
    {
        if (pick->GetPickType() == PickTypes::Line)
        {
            if (pick->PickSphere(Vector3::Zero, m_Radius))
            {
                return (pick->GetHits().back()->GetIntersectionDistance() < m_Radius);
            }
        }
        else
        {
            for (size_t i=0; i<m_Vertices.size(); i+=2)
            {
                if (pick->PickSegment(m_Vertices[i].m_Position, m_Vertices[i+1].m_Position))
                {
                    return true;
                }
            }
        }
    }

    return false;
}