#include "PipelinePch.h"
#include "PrimitiveAxes.h"

#include "Graphics/BufferedDrawer.h"

#include "Pipeline/SceneGraph/Pick.h"

#include "Orientation.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitiveAxes::PrimitiveAxes()
: m_ColorX( 0xffff0000 )
, m_ColorY( 0xff00ff00 )
, m_ColorZ( 0xff0000ff )
{
    SetElementCount( 6 );
    SetElementType( VertexElementTypes::SimpleVertex );

    m_Length = 50.0f;

    m_Bounds.minimum = Vector3 (0.0f, 0.0f, 0.0f);
    m_Bounds.maximum = Vector3 (m_Length, m_Length, m_Length);
}

void PrimitiveAxes::SetColor( AxesFlags axes, Lunar::Color c )
{
    if ((axes & MultipleAxes::X) == MultipleAxes::X)
        m_ColorX = c;

    if ((axes & MultipleAxes::Y) == MultipleAxes::Y)
        m_ColorY = c;

    if ((axes & MultipleAxes::Z) == MultipleAxes::Z)
        m_ColorZ = c;
}

void PrimitiveAxes::SetColor( Lunar::Color c )
{
    m_ColorX = c;
    m_ColorY = c;
    m_ColorZ = c;
}

void PrimitiveAxes::SetRGB()
{
    m_ColorX.SetArgb( 0xffff0000 );
    m_ColorY.SetArgb( 0xff00ff00 );
    m_ColorZ.SetArgb( 0xff0000ff );
}

void PrimitiveAxes::Update()
{
    m_Bounds.minimum = Vector3 (0.0f, 0.0f, 0.0f);
    m_Bounds.maximum = Vector3 (m_Length, m_Length, m_Length);

    m_Vertices.clear();
    m_Vertices.push_back(
        Lunar::SimpleVertex( 0.0f, 0.0f, 0.0f, m_ColorX.GetR(), m_ColorX.GetG(), m_ColorX.GetB(), m_ColorX.GetA() ) );
    m_Vertices.push_back(
        Lunar::SimpleVertex( m_Length, 0.0f, 0.0f, m_ColorX.GetR(), m_ColorX.GetG(), m_ColorX.GetB(), m_ColorX.GetA() ) );
    m_Vertices.push_back(
        Lunar::SimpleVertex( 0.0f, 0.0f, 0.0f, m_ColorY.GetR(), m_ColorY.GetG(), m_ColorY.GetB(), m_ColorY.GetA() ) );
    m_Vertices.push_back(
        Lunar::SimpleVertex( 0.0f, m_Length, 0.0f, m_ColorY.GetR(), m_ColorY.GetG(), m_ColorY.GetB(), m_ColorY.GetA() ) );
    m_Vertices.push_back(
        Lunar::SimpleVertex( 0.0f, 0.0f, 0.0f, m_ColorZ.GetR(), m_ColorZ.GetG(), m_ColorZ.GetB(), m_ColorZ.GetA() ) );
    m_Vertices.push_back(
        Lunar::SimpleVertex( 0.0f, 0.0f, m_Length, m_ColorZ.GetR(), m_ColorZ.GetG(), m_ColorZ.GetB(), m_ColorZ.GetA() ) );

    Base::Update();
}

void PrimitiveAxes::Draw(
    Lunar::BufferedDrawer* drawInterface,
    DrawArgs* args,
    Lunar::Color materialColor,
    const Simd::Matrix44& transform,
    const bool* solid,
    const bool* transparent ) const
{
    drawInterface->DrawUntextured(
        Lunar::RENDERER_PRIMITIVE_TYPE_LINE_LIST,
        transform,
        m_Buffer,
        NULL,
        GetBaseIndex(),
        6,
        0,
        3,
        materialColor,
        Lunar::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
    args->m_LineCount += 3;
}

void PrimitiveAxes::DrawAxes(
    Lunar::BufferedDrawer* drawInterface,
    DrawArgs* args,
    AxesFlags axes,
    Lunar::Color materialColor,
    const Simd::Matrix44& transform ) const
{
    if (axes & MultipleAxes::X)
    {
        drawInterface->DrawUntextured(
            Lunar::RENDERER_PRIMITIVE_TYPE_LINE_LIST,
            transform,
            m_Buffer,
            NULL,
            GetBaseIndex(),
            2,
            0,
            1,
            materialColor,
            Lunar::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
        args->m_LineCount += 1;
    }

    if (axes & MultipleAxes::Y)
    {
        drawInterface->DrawUntextured(
            Lunar::RENDERER_PRIMITIVE_TYPE_LINE_LIST,
            transform,
            m_Buffer,
            NULL,
            GetBaseIndex() + 2,
            2,
            0,
            1,
            materialColor,
            Lunar::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
        args->m_LineCount += 1;
    }

    if (axes & MultipleAxes::Z)
    {
        drawInterface->DrawUntextured(
            Lunar::RENDERER_PRIMITIVE_TYPE_LINE_LIST,
            transform,
            m_Buffer,
            NULL,
            GetBaseIndex() + 4,
            2,
            0,
            1,
            materialColor,
            Lunar::RenderResourceManager::RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED );
        args->m_LineCount += 1;
    }
}

void PrimitiveAxes::DrawViewport(
    Lunar::BufferedDrawer* drawInterface,
    DrawArgs* args,
    const SceneGraph::Camera* camera,
    Lunar::Color materialColor,
    const Simd::Matrix44& transform ) const
{
    Matrix4 projection, inverseProjection;
    camera->GetOrthographicProjection(projection);
    inverseProjection = projection.Inverted();

#ifdef VIEWPORT_REFACTOR
    // render in view space
    m_Device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix4::Identity);
    m_Device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&Matrix4::Identity);
    m_Device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&projection);

    {
        // get a point in the lower left hand corner in camera coordinates
        Vector3 pos;
        pos.x = (((2.0f * 50) / camera->GetWidth()) - 1);
        pos.y = -(((2.0f * (camera->GetHeight() - 50)) / camera->GetHeight()) - 1);
        pos.z = 1.0f;

        // transform
        inverseProjection.TransformVertex(pos);

        float32_t s = camera->GetOffset();
        Matrix4 world = Matrix4 (Scale (s, s, s)) * camera->GetOrientation() * Matrix4 (pos) * Matrix4 (Vector3 (OutVector * 100));
        m_Device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&world);

        // draw the axes
        Draw( args );
    }

    // restore matrix state
    m_Device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&camera->GetViewport());
    m_Device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&camera->GetProjection());
#endif
}

bool PrimitiveAxes::Pick( PickVisitor* pick, const bool* solid ) const
{
    for (size_t i=0; i<m_Vertices.size(); i+=2)
    {
        const Lunar::SimpleVertex& vertex0 = m_Vertices[ i ];
        const Lunar::SimpleVertex& vertex1 = m_Vertices[ i + 1 ];
        Vector3 position0( vertex0.position[ 0 ], vertex0.position[ 1 ], vertex0.position[ 2 ] );
        Vector3 position1( vertex1.position[ 0 ], vertex1.position[ 1 ], vertex1.position[ 2 ] );
        if ( pick->PickSegment( position0, position1 ) )
        {
            return true;
        }
    }
    return false;
}

AxesFlags PrimitiveAxes::PickAxis(const Matrix4& transform, Line pick, float32_t err)
{
    Vector3 offset;
    float32_t minX = m_Length, minY = m_Length, minZ = m_Length;

    Vector3 axisOrigin (Vector3::Zero);
    Vector3 axisEnd (m_Length, 0.f, 0.f);

    transform.TransformVertex (axisOrigin);
    transform.TransformVertex (axisEnd);

    if (pick.IntersectsSegment (axisOrigin, axisEnd, err, NULL, &offset))
    {
        float32_t dist = offset.Length();
        if (dist > 0.0f && dist < minX)
        {
            minX = dist;
        }
    }

    axisEnd = Vector3(0.f, m_Length, 0.f);
    transform.TransformVertex (axisEnd);

    if (pick.IntersectsSegment (axisOrigin, axisEnd, err, NULL, &offset))
    {
        float32_t dist = offset.Length();
        if (dist > 0.0f && dist < minY)
        {
            minY = dist;
        }
    }

    axisEnd = Vector3(0.f, 0.f, m_Length);
    transform.TransformVertex (axisEnd);

    if (pick.IntersectsSegment (axisOrigin, axisEnd, err, NULL, &offset))
    {
        float32_t dist = offset.Length();
        if (dist > 0.0f && dist < minZ)
        {
            minZ = dist;
        }
    }

    if ((minX == minY) && (minY == minZ))
    {
        return MultipleAxes::None;
    }

    if (minX <= minY && minX <= minZ)
    {
        return MultipleAxes::X;
    }

    if (minY <= minX && minY <= minZ)
    {
        return MultipleAxes::Y;
    }

    if (minZ <= minX && minZ <= minY)
        return MultipleAxes::Z;

    return MultipleAxes::None;
}