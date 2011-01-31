/*#include "Precompile.h"*/
#include "PrimitiveCapsule.h"

#include "Pipeline/SceneGraph/Pick.h"

#include "Orientation.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitiveCapsule::PrimitiveCapsule()
{
    SetElementType( VertexElementTypes::SimpleVertex );

    m_Radius = 1.0f;
    m_RadiusSteps = 36;

    m_Length = 2.0f;
    m_LengthSteps = 6;
}

int PrimitiveCapsule::GetWireVertCount() const
{
    if (m_RadiusSteps == 0 || m_LengthSteps == 0)
    {
        return m_WireVertCount = 0;
    }
    else
    {
        int count = 0;
        int dphi = 180 / m_LengthSteps;
        int dtheta = 360 / m_RadiusSteps;

        for (int phi=-90; phi<=90; phi+=dphi)
        {
            if (phi == 0)
                continue;

            for (int theta=0; theta<=360-dtheta; theta+=dtheta)
                count+=2;
        }

        count += m_RadiusSteps*2 * m_LengthSteps;

        return m_WireVertCount = count;
    }
}

int PrimitiveCapsule::GetPolyVertCount() const
{
    if (m_RadiusSteps == 0)
    {
        return m_PolyVertCount = 0;
    }
    else
    {
        int dtheta = 360 / m_RadiusSteps;
        int dphi = 360 / m_RadiusSteps;

        m_CapVertCount = 0;
        for (int theta=-90; theta<=90-dtheta; theta+=dtheta)
        {
            for (int phi=0; phi<=360-dphi; phi+=dphi)
            {
                m_CapVertCount+=3;

                if (theta > -90 && theta < 90)
                    m_CapVertCount +=3;
            }
        }

        m_ShaftVertCount = m_RadiusSteps*2 + 2;

        return m_PolyVertCount = (m_CapVertCount + m_ShaftVertCount);
    }
}

void PrimitiveCapsule::Update()
{
    m_Bounds.minimum = Vector3 (-m_Radius, -(m_Radius + m_Length/2.f), -m_Radius);
    m_Bounds.maximum = Vector3 (m_Radius, m_Radius + m_Length/2.f, m_Radius);

    SetElementCount( GetWireVertCount() + GetPolyVertCount() );
    m_Vertices.clear();


    //
    // Wire
    //

    int dphi = 180 / m_LengthSteps;
    int dtheta = 360 / m_RadiusSteps;

    for (int phi=-90; phi<=90; phi+=dphi)
    {
        if (phi == 0)
            continue;

        for (int theta=0; theta<=360-dtheta; theta+=dtheta)
        {
            float sinTheta = Sin( theta * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
            float sinTheta2 = Sin( ( theta + dtheta ) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
            float cosTheta = Cos( theta * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
            float cosTheta2 = Cos( ( theta + dtheta ) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );

            float sinPhi = Sin( phi * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
            float cosPhi = Cos( phi * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );

            Vector3 point = SetupVector(sinTheta * cosPhi * m_Radius,
                sinPhi * m_Radius + (MATH_SIGN(phi) * m_Length / 2.0f),
                cosTheta * cosPhi * m_Radius);
            m_Vertices.push_back( Lunar::SimpleVertex( point.x, point.y, point.z ) );

            point = SetupVector(sinTheta2 * cosPhi * m_Radius,
                sinPhi * m_Radius + (MATH_SIGN(phi) * m_Length / 2.0f),
                cosTheta2 * cosPhi * m_Radius);
            m_Vertices.push_back( Lunar::SimpleVertex( point.x, point.y, point.z ) );
        }
    }

    float stepAngle = (float32_t)HELIUM_TWOPI / (float32_t)(m_RadiusSteps);
    float stepLength = m_Length/(float32_t)(m_LengthSteps-1);

    for (int l=0; l<m_LengthSteps; l++)
    {
        for (int s=0; s<m_RadiusSteps; s++)
        {
            float theta = (float32_t)(s) * stepAngle;

            Vector3 point = SetupVector(Sin(theta) * m_Radius,
                -m_Length/2.0f + stepLength*(float32_t)(l),
                Cos(theta) * m_Radius);
            m_Vertices.push_back( Lunar::SimpleVertex( point.x, point.y, point.z ) );

            point = SetupVector(Sin(theta + stepAngle) * m_Radius,
                -m_Length/2.0f + stepLength*(float32_t)(l),
                Cos(theta + stepAngle) * m_Radius);
            m_Vertices.push_back( Lunar::SimpleVertex( point.x, point.y, point.z ) );
        }
    }


    //
    // Poly
    //

    if (m_RadiusSteps > 0)
    {
        dtheta = 360 / m_RadiusSteps;
        dphi = 360 / m_RadiusSteps;

        float offset = -m_Length/2.0f;

        for (int theta=-90; theta<=90-dtheta; theta+=dtheta)
        {
            if (abs(theta) < dtheta)
            {
                offset = m_Length/2.0f;
            }

            for (int phi=0; phi<=360-dphi; phi+=dphi)
            {
                float sinTheta = Sin(theta * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
                float sinTheta2 = Sin((theta+dtheta) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
                float cosTheta = Cos(theta * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
                float cosTheta2 = Cos((theta+dtheta) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );

                float sinPhi = Sin(phi * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
                float sinPhi2 = Sin((phi+dphi) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
                float cosPhi = Cos(phi * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );
                float cosPhi2 = Cos((phi+dphi) * static_cast< float32_t >( HELIUM_DEG_TO_RAD ) );

                Vector3 a = SetupVector(cosTheta * sinPhi * m_Radius,
                    (sinTheta * m_Radius) + offset,
                    cosTheta * cosPhi * m_Radius);

                m_Vertices.push_back( Lunar::SimpleVertex( a.x, a.y, a.z ) );

                Vector3 b = SetupVector(cosTheta2 * sinPhi2 * m_Radius,
                    (sinTheta2 * m_Radius) + offset,
                    cosTheta2 * cosPhi2 * m_Radius);

                m_Vertices.push_back( Lunar::SimpleVertex( b.x, b.y, b.z ) );

                Vector3 point = SetupVector(cosTheta2 * sinPhi * m_Radius,
                    (sinTheta2 * m_Radius) + offset,
                    cosTheta2 * cosPhi * m_Radius);

                m_Vertices.push_back( Lunar::SimpleVertex( point.x, point.y, point.z ) );

                if (theta > -90 && theta < 90)
                {
                    m_Vertices.push_back( Lunar::SimpleVertex( b.x, b.y, b.z ) );

                    m_Vertices.push_back( Lunar::SimpleVertex( a.x, a.y, a.z ) );

                    point = SetupVector(cosTheta * sinPhi2 * m_Radius,
                        (sinTheta * m_Radius) + offset,
                        cosTheta * cosPhi2 * m_Radius);

                    m_Vertices.push_back( Lunar::SimpleVertex( point.x, point.y, point.z ) );
                }
            }
        }

        // midsection
        for (int x=0; x<=m_RadiusSteps; x++)
        {
            float theta = (float32_t)(x) * stepAngle;

            Vector3 point = SetupVector(Sin(theta) * m_Radius,
                m_Length/2.0f,
                Cos(theta) * m_Radius);
            m_Vertices.push_back( Lunar::SimpleVertex( point.x, point.y, point.z ) );

            point = SetupVector(Sin(theta) * m_Radius,
                -m_Length/2.0f,
                Cos(theta) * m_Radius);
            m_Vertices.push_back( Lunar::SimpleVertex( point.x, point.y, point.z ) );
        }
    }

    Base::Update();
}

void PrimitiveCapsule::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
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
        m_Device->DrawPrimitive(D3DPT_TRIANGLELIST, (UINT)GetBaseIndex() + m_WireVertCount, m_CapVertCount/3);
        args->m_TriangleCount += (m_CapVertCount/3);
        m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, (UINT)GetBaseIndex() + m_WireVertCount + m_CapVertCount, m_RadiusSteps*2);
        args->m_TriangleCount += (m_RadiusSteps*2);
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

bool PrimitiveCapsule::Pick( PickVisitor* pick, const bool* solid ) const 
{
    if( pick->GetPickType() == PickTypes::Line )
    {
        if (pick->PickSegment(SetupVector(0.0f, -m_Length/2.0f, 0.0f), SetupVector(0.0f, m_Length/2.0f, 0.0f), m_Radius))
        {
            return true;
        }

        if (pick->PickSphere(SetupVector(0.0f, -m_Length/2.0f, 0.0f), m_Radius))
        {
            if (pick->GetHits().back()->GetIntersectionDistance() < m_Radius)
            {
                return true;
            }
        }

        if (pick->PickSphere(SetupVector(0.0f, -m_Length/2.0f, 0.0f), m_Radius))
        {
            if (pick->GetHits().back()->GetIntersectionDistance() < m_Radius)
            {
                return true;
            }
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

    return false;
}