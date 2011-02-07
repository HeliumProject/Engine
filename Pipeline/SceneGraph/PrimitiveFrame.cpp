/*#include "Precompile.h"*/
#include "PrimitiveFrame.h"

#include "Graphics/BufferedDrawer.h"
#include "Pipeline/SceneGraph/Pick.h"
#include "Color.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitiveFrame::PrimitiveFrame()
{
    SetElementCount( 9 );
    SetElementType( VertexElementTypes::ScreenVertex );

    m_BorderColor.SetArgb( 0xffc8c8c8 );
    m_InnerColor = SceneGraph::Color::BlendColor( m_BorderColor, Lunar::Color( 0 ), 0.5f);
}

void PrimitiveFrame::Update()
{
    float32_t startX = static_cast< float32_t >( m_Start.x );
    float32_t startY = static_cast< float32_t >( m_Start.y );
    float32_t endX = static_cast< float32_t >( m_End.x );
    float32_t endY = static_cast< float32_t >( m_End.y );

    m_Vertices.clear();

    Lunar::ScreenVertex vertex;
    vertex.texCoords[ 0 ].packed = 0;
    vertex.texCoords[ 1 ].packed = 0;

    vertex.color[ 0 ] = m_InnerColor.GetR();
    vertex.color[ 1 ] = m_InnerColor.GetG();
    vertex.color[ 2 ] = m_InnerColor.GetB();
    vertex.color[ 3 ] = m_InnerColor.GetA();

    vertex.position[ 0 ] = startX;
    vertex.position[ 1 ] = startY;
    m_Vertices.push_back( vertex );

    vertex.position[ 1 ] = endY;
    m_Vertices.push_back( vertex );

    vertex.position[ 0 ] = endX;
    m_Vertices.push_back( vertex );

    vertex.position[ 1 ] = startY;
    m_Vertices.push_back( vertex );

    vertex.color[ 0 ] = m_BorderColor.GetR();
    vertex.color[ 1 ] = m_BorderColor.GetG();
    vertex.color[ 2 ] = m_BorderColor.GetB();
    vertex.color[ 3 ] = m_BorderColor.GetA();

    vertex.position[ 0 ] = startX;
    vertex.position[ 1 ] = startY;
    m_Vertices.push_back( vertex );

    vertex.position[ 1 ] = endY;
    m_Vertices.push_back( vertex );

    vertex.position[ 0 ] = endX;
    m_Vertices.push_back( vertex );

    vertex.position[ 1 ] = startY;
    m_Vertices.push_back( vertex );

    vertex.position[ 0 ] = startX;
    m_Vertices.push_back( vertex );

    Base::Update();
}

void PrimitiveFrame::Draw(
    Lunar::BufferedDrawer* drawInterface,
    DrawArgs* args,
    Lunar::Color materialColor,
    const Simd::Matrix44& transform,
    const bool* solid,
    const bool* transparent ) const
{
    D3DCULL cull;
    m_Device->GetRenderState(D3DRS_CULLMODE, (DWORD*)&cull);
    m_Device->SetRenderState(D3DRS_ZENABLE, FALSE);
    {
        m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

        m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        m_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
        m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

        m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex()+4, 1);
        m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex()+5, 1);
        m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex()+6, 1);
        m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex()+7, 1);
    }
    m_Device->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_Device->SetRenderState(D3DRS_CULLMODE, cull);

    args->m_LineCount += 4;
}

bool PrimitiveFrame::Pick( PickVisitor* pick, const bool* solid ) const
{
    return false;
}