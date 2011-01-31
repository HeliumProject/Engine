/*#include "Precompile.h"*/
#include "PrimitiveGrid.h"

#include "Pipeline/SceneGraph/Pick.h"

#include "Orientation.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitiveGrid::PrimitiveGrid()
{
    SetElementType( VertexElementTypes::SimpleVertex );

    m_Width = 10;
    m_Length = 10;
    m_AxisColor.SetArgb( 0xFF000000 );
    m_MajorColor.SetArgb( 0xFF808080 );
    m_MinorColor.SetArgb( 0xFF808080 );
    m_MinorStep = 0.5f;
    m_MajorStep = 0.5f;
}

void PrimitiveGrid::SetAxisColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
    m_AxisColor.SetR( r );
    m_AxisColor.SetG( g );
    m_AxisColor.SetB( b );
    m_AxisColor.SetA( a );
}


void PrimitiveGrid::SetMajorColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
    m_MajorColor.SetR( r );
    m_MajorColor.SetG( g );
    m_MajorColor.SetB( b );
    m_MajorColor.SetA( a );
}

void PrimitiveGrid::SetMinorColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
    m_MinorColor.SetR( r );
    m_MinorColor.SetG( g );
    m_MinorColor.SetB( b );
    m_MinorColor.SetA( a );
}

void PrimitiveGrid::Update()
{
    m_Vertices.clear();

    if ( ( m_Width > 0 ) && ( m_Length > 0 ) && ( m_MajorStep > 0.0f ) && ( m_MinorStep > 0.0f ) )
    {
        int32_t numWidthLines = 2 * m_Width + 1;
        int32_t numLengthLines = 2 * m_Length + 1;

        m_Vertices.reserve( 2 * ( numWidthLines + numLengthLines ) );

        Lunar::Color color;
        Vector3 v1, v2a, v2b, v1v2a, v1v2b;

        float32_t epsilon = 0.00001f;
        float32_t halfWidth = (float32_t) m_Width * m_MinorStep;
        float32_t halfLength = (float32_t) m_Length * m_MinorStep;

        for ( float32_t x = 0.0f; x <= halfWidth; x += m_MinorStep )
        {
            if ( Abs( x ) < epsilon )
            {
                color = m_AxisColor;
            }
            else
            {
                float32_t majorDelta = Abs( x / m_MajorStep );
                if ( majorDelta - (int32_t) majorDelta < epsilon )
                {
                    color = m_MajorColor;
                }
                else
                {
                    color = m_MinorColor;
                }
            }

            v1 = SideVector * x;
            v2a = OutVector * halfLength;
            v2b = OutVector * -halfLength;
            v1v2a = v1 + v2a;
            v1v2b = v1 + v2b;
            m_Vertices.push_back( Lunar::SimpleVertex(
                v1v2a.x,
                v1v2a.y,
                v1v2a.z,
                color.GetR(),
                color.GetG(),
                color.GetB(),
                color.GetA() ) );
            m_Vertices.push_back( Lunar::SimpleVertex(
                v1v2b.x,
                v1v2b.y,
                v1v2b.z,
                color.GetR(),
                color.GetG(),
                color.GetB(),
                color.GetA() ) );

            if ( x > 0.0f )
            {
                v1 = SideVector * -x;
                v1v2a = v1 + v2a;
                v1v2b = v1 + v2b;
                m_Vertices.push_back( Lunar::SimpleVertex(
                    v1v2a.x,
                    v1v2a.y,
                    v1v2a.z,
                    color.GetR(),
                    color.GetG(),
                    color.GetB(),
                    color.GetA() ) );
                m_Vertices.push_back( Lunar::SimpleVertex(
                    v1v2b.x,
                    v1v2b.y,
                    v1v2b.z,
                    color.GetR(),
                    color.GetG(),
                    color.GetB(),
                    color.GetA() ) );
            }
        }

        for ( float32_t y = 0.0f; y <= halfLength; y += m_MinorStep )
        {
            if ( Abs( y ) < epsilon )
            {
                color = m_AxisColor;
            }
            else
            {
                float32_t majorDelta = Abs( y / m_MajorStep );
                if ( majorDelta - (int32_t) majorDelta < epsilon )
                {
                    color = m_MajorColor;
                }
                else
                {
                    color = m_MinorColor;
                }
            }

            v1 = OutVector * y;
            v2a = SideVector * halfWidth;
            v2b = SideVector * -halfWidth;
            v1v2a = v1 + v2a;
            v1v2b = v1 + v2b;
            m_Vertices.push_back( Lunar::SimpleVertex(
                v1v2a.x,
                v1v2a.y,
                v1v2a.z,
                color.GetR(),
                color.GetG(),
                color.GetB(),
                color.GetA() ) );
            m_Vertices.push_back( Lunar::SimpleVertex(
                v1v2b.x,
                v1v2b.y,
                v1v2b.z,
                color.GetR(),
                color.GetG(),
                color.GetB(),
                color.GetA() ) );

            if ( y > 0.0f )
            {
                v1 = OutVector * -y;
                v1v2a = v1 + v2a;
                v1v2b = v1 + v2b;
                m_Vertices.push_back( Lunar::SimpleVertex(
                    v1v2a.x,
                    v1v2a.y,
                    v1v2a.z,
                    color.GetR(),
                    color.GetG(),
                    color.GetB(),
                    color.GetA() ) );
                m_Vertices.push_back( Lunar::SimpleVertex(
                    v1v2b.x,
                    v1v2b.y,
                    v1v2b.z,
                    color.GetR(),
                    color.GetG(),
                    color.GetB(),
                    color.GetA() ) );
            }
        }
    }

    SetElementCount( (uint32_t) m_Vertices.size() );

    Base::Update();
}

void PrimitiveGrid::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
{
    if ( !SetState() )
    {
        return;
    }

    m_Device->SetRenderState( D3DRS_LIGHTING, FALSE );

    uint32_t lineCount = ( (uint32_t) m_Vertices.size() ) / 2;
    m_Device->DrawPrimitive( D3DPT_LINELIST, (UINT)GetBaseIndex(), lineCount );
    args->m_LineCount += lineCount;

    m_Device->SetRenderState( D3DRS_LIGHTING, TRUE );
}

bool PrimitiveGrid::Pick( PickVisitor* pick, const bool* solid ) const
{
    return false;
}