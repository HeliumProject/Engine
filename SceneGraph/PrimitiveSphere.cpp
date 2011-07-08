#include "SceneGraphPch.h"
#include "PrimitiveSphere.h"

#include "Graphics/BufferedDrawer.h"
#include "SceneGraph/Pick.h"
#include "Orientation.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitiveSphere::PrimitiveSphere()
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
            m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );

            position = SetupVector(sinTheta2 * cosPhi * m_Radius,
                sinPhi * m_Radius,
                cosTheta2 * cosPhi * m_Radius);
            m_Vertices.push_back( Helium::SimpleVertex( position.x, position.y, position.z ) );
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

                Helium::SimpleVertex a(
                    cosTheta * cosPhi * m_Radius,
                    cosTheta * sinPhi * m_Radius,
                    sinTheta * m_Radius );
                m_Vertices.push_back( a );

                Helium::SimpleVertex b(
                    cosTheta2 * cosPhi2 * m_Radius,
                    cosTheta2 * sinPhi2 * m_Radius,
                    sinTheta2 * m_Radius );
                m_Vertices.push_back( b );

                Helium::SimpleVertex vertex(
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

void PrimitiveSphere::Draw(
    Helium::BufferedDrawer* drawInterface,
    DrawArgs* args,
    Helium::Color materialColor,
    const Simd::Matrix44& transform,
    const bool* solid,
    const bool* transparent ) const
{
    HELIUM_ASSERT( drawInterface );

    if (transparent ? *transparent : m_IsTransparent)
    {
        if( materialColor.GetA() == 0 )
        {
            materialColor.SetA( 0x80 );
        }
    }

    if (solid ? *solid : m_IsSolid)
    {
        drawInterface->DrawUntextured(
            Helium::RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST,
            transform,
            m_Buffer,
            NULL,
            GetBaseIndex() + m_WireVertCount,
            m_PolyVertCount,
            0,
            m_PolyVertCount / 3,
            materialColor );
        args->m_TriangleCount += (m_PolyVertCount/3);
    }
    else
    {
        drawInterface->DrawUntextured(
            Helium::RENDERER_PRIMITIVE_TYPE_LINE_LIST,
            transform,
            m_Buffer,
            NULL,
            GetBaseIndex(),
            m_WireVertCount,
            0,
            m_WireVertCount / 2,
            materialColor,
            Helium::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
        args->m_LineCount += (m_WireVertCount/2);
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
                const Helium::SimpleVertex& vertex0 = m_Vertices[ i ];
                const Helium::SimpleVertex& vertex1 = m_Vertices[ i + 1 ];
                Vector3 position0( vertex0.position[ 0 ], vertex0.position[ 1 ], vertex0.position[ 2 ] );
                Vector3 position1( vertex1.position[ 0 ], vertex1.position[ 1 ], vertex1.position[ 2 ] );
                if ( pick->PickSegment( position0, position1 ) )
                {
                    return true;
                }
            }
        }
    }

    return false;
}