/*#include "Precompile.h"*/
#include "PrimitiveGrid.h"

#include "Pipeline/SceneGraph/Pick.h"

#include "Orientation.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitiveGrid::PrimitiveGrid(ResourceTracker* tracker)
: PrimitiveTemplate(tracker)
{
    SetElementType( ElementTypes::PositionColored );

    m_Width = 10;
    m_Length = 10;
    m_AxisColor = D3DCOLOR_ARGB( 0xFF, 0x00, 0x00, 0x00 );
    m_MajorColor = D3DCOLOR_ARGB( 0xFF, 0x80, 0x80, 0x80 );
    m_MinorColor = D3DCOLOR_ARGB( 0xFF, 0x80, 0x80, 0x80 );
    m_MinorStep = 0.5f;
    m_MajorStep = 0.5f;
}

void PrimitiveGrid::SetAxisColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
    m_AxisColor = D3DCOLOR_ARGB( a, r, g, b );
}


void PrimitiveGrid::SetMajorColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
    m_MajorColor = D3DCOLOR_ARGB( a, r, g, b );
}

void PrimitiveGrid::SetMinorColor( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
    m_MinorColor = D3DCOLOR_ARGB( a, r, g, b );
}

void PrimitiveGrid::Update()
{
    m_Vertices.clear();

    if ( ( m_Width > 0 ) && ( m_Length > 0 ) && ( m_MajorStep > 0.0f ) && ( m_MinorStep > 0.0f ) )
    {
        int numWidthLines = 2 * m_Width + 1;
        int numLengthLines = 2 * m_Length + 1;

        m_Vertices.reserve( 2 * ( numWidthLines + numLengthLines ) );

        int color;
        Vector3 v1;
        Vector3 v2a;
        Vector3 v2b;

        float epsilon = 0.00001f;
        float halfWidth = (float) m_Width * m_MinorStep;
        float halfLength = (float) m_Length * m_MinorStep;

        for ( float x = 0.0f; x <= halfWidth; x += m_MinorStep )
        {
            if ( fabs( x ) < epsilon )
            {
                color = m_AxisColor;
            }
            else
            {
                float majorDelta = fabs( x / m_MajorStep );
                if ( majorDelta - (int) majorDelta < epsilon )
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
            m_Vertices.push_back( PositionColored( v1 + v2a, color ) );
            m_Vertices.push_back( PositionColored( v1 + v2b, color ) );

            if ( x > 0.0f )
            {
                v1 = SideVector * -x;
                m_Vertices.push_back( PositionColored( v1 + v2a, color ) );
                m_Vertices.push_back( PositionColored( v1 + v2b, color ) );
            }
        }

        for ( float y = 0.0f; y <= halfLength; y += m_MinorStep )
        {
            if ( fabs( y ) < epsilon )
            {
                color = m_AxisColor;
            }
            else
            {
                float majorDelta = fabs( y / m_MajorStep );
                if ( majorDelta - (int) majorDelta < epsilon )
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
            m_Vertices.push_back( PositionColored( v1 + v2a, color ) );
            m_Vertices.push_back( PositionColored( v1 + v2b, color ) );

            if ( y > 0.0f )
            {
                v1 = OutVector * -y;
                m_Vertices.push_back( PositionColored( v1 + v2a , color ) );
                m_Vertices.push_back( PositionColored( v1 + v2b, color ) );
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