/*#include "Precompile.h"*/
#include "PrimitiveCube.h"

#include "Pipeline/SceneGraph/Pick.h"

using namespace Helium;
using namespace Helium::SceneGraph;

PrimitiveCube::PrimitiveCube(ResourceTracker* tracker)
: PrimitiveTemplate(tracker)
{
    SetElementType( VertexElementTypes::SimpleVertex );
    SetElementCount( 60 );

    m_Bounds.minimum = Vector3 (-1.0f, -1.0f, -1.0f);
    m_Bounds.maximum = Vector3 (1.0f, 1.0f, 1.0f);
}

void PrimitiveCube::Update()
{
    m_Vertices.clear();

    V_Vector3 vertices, drawVertices;
    m_Bounds.GetVertices( vertices );
    m_Bounds.GetWireframe( vertices, drawVertices, false );
    m_Bounds.GetTriangulated( vertices, drawVertices, false );

    size_t drawVertexCount = drawVertices.size();
    m_Vertices.reserve( drawVertexCount );
    for ( size_t vertexIndex = 0; vertexIndex < drawVertexCount; ++vertexIndex )
    {
        m_Vertices.push_back( Lunar::SimpleVertex( drawVertices[ vertexIndex ] ) );
    }

    Base::Update();
}

void PrimitiveCube::Draw( DrawArgs* args, const bool* solid, const bool* transparent ) const
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
        m_Device->DrawPrimitive(D3DPT_TRIANGLELIST, (UINT)(GetBaseIndex() + 24), 12);
        args->m_TriangleCount += 12;
    }
    else
    {
        m_Device->DrawPrimitive(D3DPT_LINELIST, (UINT)GetBaseIndex(), 12);
        args->m_LineCount += 12;
    }

    if (transparent ? *transparent : m_IsTransparent)
    {
        m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }
}

bool PrimitiveCube::Pick( PickVisitor* pick, const bool* solid ) const
{
    if (solid ? *solid : m_IsSolid)
    {
        return pick->PickBox(m_Bounds);
    }
    else
    {
        for (size_t i=0; i<24; i+=2)
        {
            if (pick->PickSegment(m_Vertices[i].m_Position, m_Vertices[i+1].m_Position))
            {
                return true;
            }
        }

        return false;
    }

    return false;
}