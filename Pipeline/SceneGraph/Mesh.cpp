/*#include "Precompile.h"*/
#include "Pipeline/SceneGraph/Mesh.h"

#include "Pipeline/SceneGraph/Pick.h"
#include "Pipeline/SceneGraph/Color.h"
#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/Shader.h"
#include "Pipeline/SceneGraph/Transform.h"
#include "Pipeline/SceneGraph/HierarchyNodeType.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_OBJECT( Mesh );

#pragma TODO("Data-hide public reflected fields")

void Mesh::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &Mesh::m_Positions,                  TXT( "m_Positions" ) );
    comp.AddField( &Mesh::m_Normals,                    TXT( "m_Normals" ) );
    comp.AddField( &Mesh::m_Colors,                     TXT( "m_Colors" ) );
    comp.AddField( &Mesh::m_BaseUVs,                    TXT( "m_BaseUVs" ) );

    comp.AddField( &Mesh::m_WireframeVertexIndices,     TXT( "m_WireframeVertexIndices" ) );
    comp.AddField( &Mesh::m_TriangleVertexIndices,      TXT( "m_TriangleVertexIndices" ) );
    comp.AddField( &Mesh::m_PolygonIndices,             TXT( "m_PolygonIndices" ) );
    comp.AddField( &Mesh::m_ShaderIndices,              TXT( "m_ShaderIndices" ) );

    comp.AddField( &Mesh::m_ShaderIDs,                  TXT( "m_ShaderIDs" ) );
    comp.AddField( &Mesh::m_ShaderTriangleCounts,       TXT( "m_ShaderTriangleCounts" ) );
}

void Mesh::InitializeType()
{
    Reflect::RegisterClassType< Mesh >( TXT( "SceneGraph::Mesh" ) );

    s_WireMaterial = Color::BLACK;
    s_FillMaterial = Color::DARKGRAY;
}

void Mesh::CleanupType()
{
    Reflect::UnregisterClassType< Mesh >();
}

Mesh::Mesh()
: m_HasAlpha( false )
, m_HasColor( false )
, m_HasTexture( false )
, m_LineCount( 0x0 )
, m_VertexCount( 0x0 )
, m_TriangleCount( 0x0 )
{

}

Mesh::~Mesh()
{

}

int32_t Mesh::GetImageIndex() const
{
    return -1; // Helium::GlobalFileIconsTable().GetIconID( TXT( "mesh" ) );
}

tstring Mesh::GetApplicationTypeName() const
{
    return TXT( "Mesh" );
}

void Mesh::Initialize()
{
    Base::Initialize();

    //
    // Dereference Shaders
    //

    V_TUID::const_iterator itr = m_ShaderIDs.begin();
    V_TUID::const_iterator end = m_ShaderIDs.end();
    for ( ; itr != end; ++itr )
    {
        Shader* shader = Reflect::SafeCast< Shader >( m_Owner->FindNode( *itr ) );

        if ( shader )
        {
            CreateDependency( shader );
            shader->Dirty();
        }

        m_Shaders.push_back( shader );
    }

    //
    // Deduce data
    //

    for ( size_t i = 0; i < m_Shaders.size(); i++ )
    {
        if ( m_Shaders[i] )
        {
            m_HasAlpha |= m_Shaders[i]->GetAlpha();
            m_HasTexture |= m_Shaders[i]->GetBaseTexture() != NULL;
        }
    }

    if ( m_Colors.size() > 0 )
    {
        if ( m_Colors.size() == 1 )
        {
            m_HasColor = m_Colors[0] != Vector4( 0, 0, 0, 1.0 );
        }
        else
        {
            m_HasColor = true;
        }
    }

    m_LineCount = (uint32_t)m_WireframeVertexIndices.size() / 2;
    m_VertexCount = (uint32_t)m_Positions.size();
    m_TriangleCount = (uint32_t)m_TriangleVertexIndices.size() / 3;

    uint32_t count = 0;
    for ( size_t i=0; i<m_ShaderIDs.size(); ++i )
    {
        m_ShaderStartIndices.push_back( count );
        count += m_ShaderTriangleCounts[i];
    }

    m_Indices = new IndexResource;
    m_Indices->SetElementType( IndexElementTypes::Unsigned32 );
    m_Indices->SetElementCount( (uint32_t)(m_WireframeVertexIndices.size() + m_TriangleVertexIndices.size()) );
    m_Indices->SetPopulator( PopulateSignature::Delegate( this, &Mesh::Populate ) );
    m_Indices->Create();

    m_Vertices = new VertexResource;
    m_Vertices->SetElementType( VertexElementTypes::SimpleVertex );
    m_Vertices->SetElementCount( (uint32_t)m_Positions.size() );
    m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &Mesh::Populate ) );
    m_Vertices->Create();
}

void Mesh::Create()
{
    Base::Create();

    if (m_IsInitialized)
    {
        m_Indices->Create();
        m_Vertices->Create();
    }
}

void Mesh::Delete()
{
    Base::Delete();

    if (m_IsInitialized)
    {
        m_Indices->Delete();
        m_Vertices->Delete();
    }
}

void Mesh::Populate(PopulateArgs* args)
{
#ifdef VIEWPORT_REFACTOR
    switch ( args->m_Type )
    {
    case ResourceTypes::Index:
        {
            if ( args->m_Buffer != NULL )
            {
                memcpy( args->m_Buffer + args->m_Offset, &( m_WireframeVertexIndices.front() ), m_WireframeVertexIndices.size() * sizeof( uint32_t ) );
                args->m_Offset += ( (uint32_t)m_WireframeVertexIndices.size() * sizeof( uint32_t ) );

                memcpy( args->m_Buffer + args->m_Offset, &( m_TriangleVertexIndices.front() ), m_TriangleVertexIndices.size() * sizeof( uint32_t ) );
                args->m_Offset += ( (uint32_t)m_TriangleVertexIndices.size() * sizeof( uint32_t ) );
            }
            break;
        }

    case ResourceTypes::Vertex:
        {
            if ( args->m_Buffer != NULL )
            {
                m_ObjectBounds.Reset();

                if (m_VertexCount > 0)
                {
                    StandardVertex* vertex = NULL;

                    for ( uint32_t i=0; i<m_VertexCount; ++i )
                    {
                        // get address for the current vertex in the resource buffer
                        vertex = reinterpret_cast<StandardVertex*>(args->m_Buffer + args->m_Offset) + i;

                        // Position, test for local bounds computation
                        vertex->m_Position = m_ObjectBounds.Test( m_Positions[i] );

                        // Normal, used for lighting
                        if (m_Normals.size())
                        {
                            vertex->m_Normal = m_Normals[i];
                        }

                        if (m_HasColor)
                        {
                            // Vertex Color
                            vertex->m_Diffuse = D3DCOLOR_COLORVALUE( m_Colors[i].x, m_Colors[i].y, m_Colors[i].z, m_Colors[i].w );
                        }

                        if (m_HasTexture)
                        {
                            // Color Map UV
                            vertex->m_BaseUV.x = m_BaseUVs[i].x;
                            vertex->m_BaseUV.y = 1.0f - m_BaseUVs[i].y;
                        }
                    }

                    args->m_Offset += ( m_VertexCount * sizeof( StandardVertex ) );

                    HELIUM_ASSERT(args->m_Buffer + args->m_Offset == reinterpret_cast<uint8_t*>(++vertex));
                }
            }
            break;
        }
    }
#endif
}

void Mesh::Evaluate(GraphDirection direction)
{
    switch (direction)
    {
    case GraphDirections::Downstream:
        {
            m_ObjectBounds.Reset();

            for ( uint32_t i=0; i<m_VertexCount; ++i )
            {
                m_ObjectBounds.Test( m_Positions[i] );
            }

            if (m_IsInitialized)
            {
                m_Indices->Update();
                m_Vertices->Update();
            }

            break;
        }
    }

    Base::Evaluate(direction);
}

void Mesh::Render( RenderVisitor* render )
{
#ifdef VIEWPORT_REFACTOR
    RenderEntry* entry = render->Allocate(this);

    if (render->GetViewport()->GetCamera()->IsBackFaceCulling() && render->State().m_Matrix.Determinant() < 0)
    {
        entry->m_ObjectSetup = &Mesh::SetupFlippedObject;
        entry->m_ObjectReset = &Mesh::ResetFlippedObject;
    }
    else
    {
        entry->m_ObjectSetup = &Mesh::SetupNormalObject;
    }

    entry->m_Location = render->State().m_Matrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Flags |= m_HasAlpha ? RenderFlags::DistanceSort : 0;

    bool selectable = render->State().m_Selectable;
    bool highlighted = ( ( IsHighlighted() && m_Owner->IsFocused() ) || ( render->State().m_Highlighted ) ) && render->GetViewport()->IsHighlighting();
    bool selected = ( IsSelected() && m_Owner->IsFocused() ) || ( render->State().m_Selected );
    bool live = ( IsLive() && m_Owner->IsFocused() ) || ( render->State().m_Live );
    bool wire = render->GetViewport()->GetCamera()->GetWireframeOnShaded();

    switch ( render->GetViewport()->GetCamera()->GetShadingMode() )
    {
    case ShadingMode::Wireframe:
        {
            if ( selectable )
            {
                entry->m_Draw = &Mesh::DrawNormalWire;

                if (highlighted)
                {
                    entry->m_DrawSetup = &Mesh::SetupHighlightedWire;
                }
                else if (selected)
                {
                    entry->m_DrawSetup = &Mesh::SetupSelectedWire;
                }
                else if (live)
                {
                    entry->m_DrawSetup = &Mesh::SetupLiveWire;
                }
                else
                {
                    entry->m_DrawSetup = &Mesh::SetupNormalWire;
                }
            }
            else
            {
                entry->m_DrawSetup = &Mesh::SetupUnselectableWire;
                entry->m_Draw = &Mesh::DrawUnselectableWire;
            }

            break;
        }

    default:
        {
            // this will draw biased geometry
            if (m_HasAlpha)
            {
                entry->m_DrawSetup = &Mesh::SetupAlpha;
                entry->m_DrawReset = &Mesh::ResetAlpha;
            }
            else
            {
                entry->m_DrawSetup = &Mesh::SetupNormal;
                entry->m_DrawReset = &Mesh::ResetNormal;
            }

            entry->m_Draw = &Mesh::DrawNormal;

            if ( render->GetViewport()->GetCamera()->GetWireframeOnMesh() && ( highlighted || selected || live || wire ) )
            {
                // this will draw unbiased wireframe
                entry = render->Allocate(this);

                if (render->GetViewport()->GetCamera()->IsBackFaceCulling() && render->State().m_Matrix.Determinant() < 0)
                {
                    entry->m_ObjectSetup = &Mesh::SetupFlippedObject;
                    entry->m_ObjectReset = &Mesh::ResetFlippedObject;
                }
                else
                {
                    entry->m_ObjectSetup = &Mesh::SetupNormalObject;
                }

                entry->m_Draw = &Mesh::DrawNormalWire;
                entry->m_Location = render->State().m_Matrix;
                entry->m_Center = m_ObjectBounds.Center();

                if (selectable)
                {
                    if (highlighted)
                    {
                        entry->m_DrawSetup = &Mesh::SetupHighlightedWire;
                    }
                    else if (selected)
                    {
                        entry->m_DrawSetup = &Mesh::SetupSelectedWire;
                    }
                    else if (live)
                    {
                        entry->m_DrawSetup = &Mesh::SetupLiveWire;
                    }
                    else if (wire)
                    {
                        entry->m_DrawSetup = &Mesh::SetupNormalWire;
                    }
                }
                else
                {
                    entry->m_DrawSetup = &Mesh::SetupUnselectableWire;
                    entry->m_Draw = &Mesh::DrawUnselectableWire;
                }
            }

            break;
        }
    }
#endif

    Base::Render( render );
}

#ifdef VIEWPORT_REFACTOR

void Mesh::SetupNormalObject( IDirect3DDevice9* device, const SceneNode* object )
{
    const Mesh* mesh = Reflect::AssertCast< Mesh > ( object );

    const Resource* indices = mesh->m_Indices;
    const Resource* vertices = mesh->m_Vertices;

    if (indices && vertices)
    {
        indices->SetState();
        vertices->SetState();
    }
    else
    {
        HELIUM_BREAK();
    }
}

void Mesh::SetupFlippedObject( IDirect3DDevice9* device, const SceneNode* object )
{
    SetupNormalObject( device, object );
    device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
}

void Mesh::ResetFlippedObject( IDirect3DDevice9* device, const SceneNode* object )
{
    device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
}

void Mesh::SetupNormalWire( IDirect3DDevice9* device )
{
    device->SetMaterial( &s_WireMaterial );
}

void Mesh::DrawNormalWire( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const HierarchyNode* node = Reflect::AssertCast<HierarchyNode>( object );
    const Mesh* mesh = Reflect::AssertCast<Mesh>( node );

    const IndexResource* indices = mesh->m_Indices;
    const VertexResource* vertices = mesh->m_Vertices;

    if (indices && vertices)
    {
        device->DrawIndexedPrimitive( D3DPT_LINELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), indices->GetBaseIndex(), mesh->m_LineCount );
        args->m_LineCount += mesh->m_LineCount;
    }
    else
    {
        HELIUM_BREAK();
    }
}

void Mesh::DrawUnselectableWire( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    DrawNormalWire( device, args, object );
}

void Mesh::SetupUnselectableWire( IDirect3DDevice9* device )
{
    device->SetMaterial( &Viewport::s_UnselectableMaterial );
}

void Mesh::SetupSelectedWire( IDirect3DDevice9* device )
{
    device->SetMaterial( &Viewport::s_SelectedMaterial );
}

void Mesh::SetupHighlightedWire( IDirect3DDevice9* device )
{
    device->SetMaterial( &Viewport::s_HighlightedMaterial );
}

void Mesh::SetupLiveWire( IDirect3DDevice9* device )
{
    device->SetMaterial( &Viewport::s_LiveMaterial );
}

void Mesh::SetupAlpha( IDirect3DDevice9* device )
{
    device->SetMaterial( &s_FillMaterial );
    device->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    device->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, 0x40000000 );
}

void Mesh::ResetAlpha( IDirect3DDevice9* device )
{
    device->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
    device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    device->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, 0x0 );
}

void Mesh::SetupNormal( IDirect3DDevice9* device )
{
    device->SetMaterial( &s_FillMaterial );
    device->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, 0x40000000 );
}

void Mesh::ResetNormal( IDirect3DDevice9* device )
{
    device->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, 0x0 );
}

void Mesh::DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    SCENE_GRAPH_RENDER_SCOPE_TIMER( ("") );

    const HierarchyNode* node = Reflect::AssertCast<HierarchyNode>( object );
    const Mesh* mesh = Reflect::AssertCast<Mesh>( node );

    const IndexResource* indices = mesh->m_Indices;
    const VertexResource* vertices = mesh->m_Vertices;

    if (indices && vertices)
    {
        Viewport* view = node->GetOwner()->GetViewport();
        Camera* camera = view->GetCamera();

        switch ( camera->GetShadingMode() )
        {
        case ShadingMode::Material:
            {
                device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), (UINT)( indices->GetBaseIndex() + mesh->m_WireframeVertexIndices.size() ), (UINT)( mesh->m_TriangleCount ) );

                args->m_TriangleCount += mesh->m_TriangleCount;

                break;
            }

        case ShadingMode::Texture:
            {
                size_t shaderCount;
                size_t shaderTriCountsCount;
                size_t shaderStartIndicesCount;

                shaderCount = mesh->m_Shaders.size();

                shaderTriCountsCount = mesh->m_ShaderTriangleCounts.size();
                if (shaderCount != shaderTriCountsCount)
                {
                    shaderTriCountsCount = shaderCount = MIN(shaderTriCountsCount, shaderCount);
                }

                shaderStartIndicesCount = mesh->m_ShaderStartIndices.size();
                if (shaderCount != shaderStartIndicesCount)
                {
                    shaderStartIndicesCount = shaderCount = MIN(shaderStartIndicesCount, shaderCount);
                }

                for ( uint32_t shaderIndex = 0; shaderIndex < shaderTriCountsCount; shaderIndex++ )
                {
                    uint32_t triangleCount = mesh->m_ShaderTriangleCounts[shaderIndex];

                    if ( triangleCount != 0 )
                    {
                        if ( shaderIndex < shaderCount && mesh->m_Shaders[shaderIndex])
                        {
                            device->SetTexture( 0, mesh->m_Shaders[shaderIndex]->GetBaseTexture() );
                        }
                        else
                        {
                            device->SetTexture( 0, NULL );
                        }

                        uint32_t startIndex = (uint32_t)indices->GetBaseIndex() + (uint32_t)mesh->m_WireframeVertexIndices.size() + (mesh->m_ShaderStartIndices[shaderIndex] * 3);

                        device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), (UINT)startIndex, triangleCount );

                        args->m_TriangleCount += triangleCount;

                        device->SetTexture( 0, NULL );
                    }
                }

                break;
            }
        }
    }
    else
    {
        HELIUM_BREAK();
    }
}

#endif

bool Mesh::Pick( PickVisitor* pick )
{
    const Transform* t = GetTransform();

    // save the size for checking if we got local hits later
    size_t high = pick->GetHitCount();

    // set the pick's matrices to process intersections in this space
    pick->SetCurrentObject (this, pick->State().m_Matrix);

    if (pick->GetCamera()->GetShadingMode() == ShadingMode::Wireframe)
    {
        // test each segment (vertex data is in local space, intersection function will transform)
        for (size_t i=0; i<m_WireframeVertexIndices.size(); i+=2)
        {
            pick->PickSegment(m_Positions[ m_WireframeVertexIndices[i] ],
                m_Positions[ m_WireframeVertexIndices[i+1] ]);
        }
    }
    else
    {
        // test each triangle (vertex data is in local space, intersection function will transform)
        for (size_t i=0; i<m_TriangleVertexIndices.size(); i+=3)
        {
            pick->PickTriangle(m_Positions[ m_TriangleVertexIndices[i] ],
                m_Positions[ m_TriangleVertexIndices[i+1] ],
                m_Positions[ m_TriangleVertexIndices[i+2] ]);
        }
    }

    return pick->GetHits().size() > high;
}


void Mesh::ComputeTNBs()
{
    uint32_t numTris = GetTriangleCount();
    uint32_t numVerts = GetVertexCount();

    //m_Normals.resize( numVerts );
    m_Tangents.resize( numVerts );
    m_Binormals.resize( numVerts );

    for( uint32_t i = 0; i < numTris; ++i )
        ComputeTNB( i );
}

bool Mesh::ComputeTNB( uint32_t triIndex )
{

    static const float kLengthSqrTolerance  =  1.0e-30f;
    const float EPSILON = 0.000001f;

    triIndex *= 3;
    uint32_t idx0 = m_TriangleVertexIndices[ triIndex ];
    uint32_t idx1 = m_TriangleVertexIndices[ triIndex + 1 ];
    uint32_t idx2 = m_TriangleVertexIndices[ triIndex + 2 ];

    Vector3 v0 = m_Positions[ idx0 ];
    Vector3 v1 = m_Positions[ idx1 ];
    Vector3 v2 = m_Positions[ idx2 ];

    Vector2 uv0 = m_BaseUVs[ idx0 ];
    Vector2 uv1 = m_BaseUVs[ idx1 ];
    Vector2 uv2 = m_BaseUVs[ idx2 ];

    Vector3 triNormal;
    Vector3 triTangent;
    Vector3 triBinormal;

    triNormal = ( v1 - v0 ).Cross( (v2 - v0 ) );

    if (triNormal.LengthSquared() <= kLengthSqrTolerance)
    {
        return false;
    }
    triNormal.Normalize();

    float interp;
    Vector3 interp_vec;
    Vector2 interpuv;
    Vector3 tempCoord;
    Vector2 tempuv;

    // COMPUTE TANGENT VECTOR
    //sort verts by their v in uv
    if (uv0.y < uv1.y)
    {
        tempCoord = v0;
        tempuv = uv0;
        v0 = v1;
        uv0 = uv1;
        v1 = tempCoord;
        uv1 = tempuv;
    }
    if (uv0.y < uv2.y)
    {
        tempCoord = v0;
        tempuv = uv0;
        v0 = v2;
        uv0 = uv2; 
        v2 = tempCoord;
        uv2 = tempuv;
    }
    if (uv1.y < uv2.y)
    {
        tempCoord = v1;
        tempuv = uv1;
        v1 = v2;
        uv1 = uv2;
        v2 = tempCoord;
        uv2 = tempuv;
    }

    //compute parametric offset along edge02 to the middle coordinate
    if (abs(uv2.y - uv0.y) < EPSILON)
    {
        interp = 1.0f;
    }
    else
    {
        interp = (uv1.y - uv0.y)/(uv2.y - uv0.y);
    }

    //use iterpolation parameter to compute the vertex position along edge02 that has same v as vert1
    interp_vec = (v0*(1.0f - interp)) + (v2*interp);
    interpuv.y = uv1.y;
    interpuv.x = uv0.x*(1.0f - interp) + uv2.x*interp;

    //tangent vector is the ray from middle vert to the interploated vector
    triTangent  = (interp_vec - v1);

    //make sure tangent points in the right direction
    if (interpuv.x < uv1.x)
    {
        triTangent *= -1.0f;
    }

    if (triTangent.LengthSquared() <= kLengthSqrTolerance)
    {
        return false;
    }

    triTangent.Normalize();

    //make sure tangent is perpendicular to the normal (unecessary step. Check and remove later. -Reddy)
    float dot = triNormal.Dot( triTangent );
    triTangent = triTangent - (triNormal*dot);

    if (triTangent.LengthSquared() <= kLengthSqrTolerance)
    {
        return false;
    }
    triTangent.Normalize();

    //Compute Binormal vector
    //sort vectors by u 
    if (uv0.x < uv1.x)
    {
        tempCoord = v0;
        tempuv = uv0;
        v0 = v1;
        uv0 = uv1;
        v1 = tempCoord;
        uv1 = tempuv;
    }
    if (uv0.x < uv2.x)
    {
        tempCoord = v0;
        tempuv = uv0;
        v0 = v2;
        uv0 = uv2;
        v2 = tempCoord;
        uv2 = tempuv;
    }
    if (uv1.x < uv2.x)
    {
        tempCoord = v1;
        tempuv = uv1;
        v1 = v2;
        uv1 = uv2;
        v2 = tempCoord;
        uv2 = tempuv;
    }

    //compute parametric offset along edge02 to the middle coordinate

    if (abs(uv2.x - uv0.x) < EPSILON)
    {
        interp = 1.0f;
    }
    else
    {
        interp = (uv1.x - uv0.x)/(uv2.x - uv0.x);
    }

    //use iterpolation parameter to compute the vertex position along edge02 that has same u as vert1
    interp_vec = (v0*(1.0f - interp)) + (v2*interp);
    interpuv.x = uv1.x;
    interpuv.y = uv0.y*(1.0f - interp) + uv2.y*interp;

    //binormal vector is the ray from middle vert to the interploated vector
    triBinormal  = (interp_vec - v1);

    //make sure binormal points in the right direction
    if (interpuv.y < uv1.y)
    {
        triBinormal *= -1.0f;
    }

    //make sure binormal is perpendicular to the normal
    dot = triBinormal.Dot( triNormal );
    triBinormal = triBinormal - (triNormal*dot);

    if ( triBinormal.Equal( Vector3::Zero ) )
    {
        triBinormal = triTangent.Cross( triNormal );
    }
    else
    {
        triBinormal.Normalize();
    }

    if( triBinormal.LengthSquared() < kLengthSqrTolerance )
    {
        return false;
    }

    m_Tangents[idx0] = triTangent;
    m_Tangents[idx1] = triTangent;
    m_Tangents[idx2] = triTangent;

    m_Binormals[idx0] = triBinormal;
    m_Binormals[idx1] = triBinormal;
    m_Binormals[idx2] = triBinormal;

    return true;
}

void Mesh::GetAlignedBoundingBox( AlignedBox& box ) const
{
    V_Vector3::const_iterator itr = m_Positions.begin();
    V_Vector3::const_iterator end = m_Positions.end();
    for ( ; itr != end; ++itr )
    {
        const Vector3& point = *itr;
        box.Merge( point );
    }
}

void Mesh::GetBoundingSphere( BoundingVolumeGenerator::BSphere& bsphere ) const
{
    BoundingVolumeGenerator generator( (Vector3*)&m_Positions.front(), (int32_t)m_Positions.size() );
    bsphere = generator.GetPrincipleAxisBoundingSphere();
}

float32_t Mesh::SurfaceArea( Scale* scale ) const
{
    HELIUM_ASSERT( m_TriangleVertexIndices.size()%3 == 0 );

    float32_t area = 0.0f;

    if( !scale )
    {
        std::vector< uint32_t >::const_iterator itr = m_TriangleVertexIndices.begin();
        std::vector< uint32_t >::const_iterator end  = m_TriangleVertexIndices.end();
        for( ; itr != end; itr+=3 )
        {

            Vector3 edge1 = m_Positions[*(itr+1)] - m_Positions[*itr];
            Vector3 edge3 = m_Positions[*(itr+2)] - m_Positions[*itr];

            float32_t dot = edge1.Dot( edge3 );
            float32_t triArea = sqrt( ( edge1.LengthSquared()*edge3.LengthSquared() ) -  ( dot*dot ) ) * 0.5f;
            if( !_isnan( triArea ) )
                area += triArea;

        }
    }
    else
    {
        std::vector< uint32_t >::const_iterator itr = m_TriangleVertexIndices.begin();
        std::vector< uint32_t >::const_iterator end  = m_TriangleVertexIndices.end();
        for( ; itr != end; itr+=3 )
        {
            Vector3 edge1 = m_Positions[*(itr+1)] * *scale - m_Positions[*itr] * *scale;
            Vector3 edge3 = m_Positions[*(itr+2)] * *scale - m_Positions[*itr] * *scale;

            float32_t dot = edge1.Dot( edge3 );
            float32_t triArea = sqrt( ( edge1.LengthSquared()*edge3.LengthSquared() ) -  ( dot*dot ) ) * 0.5f;
            if( !_isnan( triArea ) )
                area += triArea;

        }
    }
    return area;
}

float32_t Mesh::SurfaceAreaComponents( Vector3& areaVec ) const
{
    /*
    static const Vector3 xAxis(1.0f, 0.0f, 0.0f );
    static const Vector3 yAxis(0.0f, 1.0f, 0.0f );
    static const Vector3 zAxis(0.0f, 0.0f, 1.0f );

    float32_t area = 0.0f;

    std::vector< uint32_t >::const_iterator itr = m_TriangleVertexIndices.begin();
    std::vector< uint32_t >::const_iterator end  = m_TriangleVertexIndices.end();
    for( ; itr != end; itr+=3 )
    {
    const Vector3& v0 = m_Positions[*itr];
    const Vector3& v1 = m_Positions[*(itr+1)];
    const Vector3& v2 = m_Positions[*(itr+2)];

    float32_t a = (v0 - v1).Length();
    float32_t b = (v1 - v2).Length();
    float32_t c = (v2 - v0).Length();

    float32_t p1 = (a+b+c) * 0.5f;
    float32_t p2 = p1 * (p1-a)*(p1-b)*(p1-c);
    float32_t triArea = sqrt(p2);

    if( !_isnan( triArea ) )
    {
    float32_t xMag = v0.Dot(xAxis) + v1.Dot(xAxis) + v2.Dot(xAxis);
    float32_t yMag = v0.Dot(yAxis) + v1.Dot(yAxis) + v2.Dot(yAxis);
    float32_t zMag = v0.Dot(zAxis) + v1.Dot(zAxis) + v2.Dot(zAxis);

    float32_t d = (xMag + yMag + zMag )/triArea;     

    Vector3 triAreaVec( xMag, yMag, zMag );
    triAreaVec /= d;

    area += triArea;
    areaVec += triAreaVec;
    }
    }
    return area;
    */
    static const Vector3 xAxis(1.0f, 0.0f, 0.0f );
    static const Vector3 yAxis(0.0f, 1.0f, 0.0f );
    static const Vector3 zAxis(0.0f, 0.0f, 1.0f );

    float32_t area = 0.0f;

    Scale scale( 1.5f, 0.5f, 0.5f );

    std::vector< uint32_t >::const_iterator itr = m_TriangleVertexIndices.begin();
    std::vector< uint32_t >::const_iterator end  = m_TriangleVertexIndices.end();
    for( ; itr != end; itr+=3 )
    {
        float32_t triArea = 0.0f;

        const Vector3& v0 = m_Positions[*itr];
        const Vector3& v1 = m_Positions[*(itr+1)];
        const Vector3& v2 = m_Positions[*(itr+2)];

        {

            float32_t a = (v0 - v1).Length();
            float32_t b = (v1 - v2).Length();
            float32_t c = (v2 - v0).Length();

            float32_t p1 = (a+b+c) * 0.5f;
            float32_t p2 = p1 * (p1-a)*(p1-b)*(p1-c);
            triArea = sqrt(p2);
        }

        float32_t scaledTriArea = 0.0f;
        const Vector3& ScaledV0 = m_Positions[*itr] * scale;
        const Vector3& ScaledV1 = m_Positions[*(itr+1)] * scale;
        const Vector3& ScaledV2 = m_Positions[*(itr+2)] * scale;

        {
            float32_t a = (ScaledV0 - ScaledV1).Length();
            float32_t b = (ScaledV1 - ScaledV2).Length();
            float32_t c = (ScaledV2 - ScaledV0).Length();

            float32_t p1 = (a+b+c) * 0.5f;
            float32_t p2 = p1 * (p1-a)*(p1-b)*(p1-c);
            scaledTriArea = sqrt(p2);
        }

        if( !_isnan( triArea ) )
        {
            float32_t xMag = abs(v0.Dot(xAxis)) + abs(v1.Dot(xAxis)) + abs(v2.Dot(xAxis));
            float32_t yMag = abs(v0.Dot(yAxis)) + abs(v1.Dot(yAxis)) + abs(v2.Dot(yAxis));
            float32_t zMag = abs(v0.Dot(zAxis)) + abs(v1.Dot(zAxis)) + abs(v2.Dot(zAxis));

            //float32_t d = (xMag + yMag + zMag )/triArea;     

            //Vector3 triAreaVec( xMag/triArea, yMag/triArea, zMag/triArea );
            // triAreaVec /= d;

            float32_t magSum = xMag + yMag + zMag;
            Vector3 triAreaVec( xMag/magSum, yMag/magSum, zMag/magSum );

            float32_t sqrtTriArea = sqrt(triArea);

            area += triArea;
            areaVec += triAreaVec;
        }
    }
    return area;
}

void Mesh::GetTriangle( uint32_t triIndex, Vector3& v0, Vector3& v1, Vector3& v2, Matrix4* transform )
{
    HELIUM_ASSERT( m_TriangleVertexIndices.size()%3 == 0 );
    HELIUM_ASSERT( triIndex < GetTriangleCount() );

    triIndex *= 3;


    v0 = m_Positions[m_TriangleVertexIndices[triIndex]];
    v1 = m_Positions[m_TriangleVertexIndices[triIndex+1]];
    v2 = m_Positions[m_TriangleVertexIndices[triIndex+2]];

    if( transform )
    {
        transform->TransformVertex( v0 );
        transform->TransformVertex( v1 );
        transform->TransformVertex( v2 );
    }
}

/////////////////////////////////////////////////////////////
// welds mesh verts for a given threshold
/////////////////////////////////////////////////////////////
void Mesh::WeldMeshVerts(const float32_t vertex_merge_threshold)
{
    V_Vector3  pos_array;
    MM_i32 pos_lookup;

    std::vector< int32_t > old_to_new_vert_mapping;
    int32_t iv = 0;

    pos_array.reserve(m_Positions.size());
    old_to_new_vert_mapping.reserve(m_Positions.size());

    for (V_Vector3::iterator ivert = m_Positions.begin(); ivert != m_Positions.end(); ++ivert, ++iv)
    {
        // compute a min and max key based on vertex position
        float fkey = (ivert->x + ivert->y + ivert->z) * 100.0f;
        int32_t min_key = (int32_t)( fkey - 0.1f );
        int32_t max_key = (int32_t)( fkey + 0.1f );

        // check if this vertex position is very similar to one already in the vertex position array
        int32_t match_idx = LookupPosInArray(*ivert, min_key, max_key, pos_array, pos_lookup, vertex_merge_threshold );

        // add there was no matching vertex position in the array...
        if (match_idx < 0)
        {
            // add an entry in the lookup multi-map for the new vertex position
            pos_lookup.insert( std::make_pair( (int32_t)fkey, (int32_t)pos_array.size() ) );
            old_to_new_vert_mapping.push_back((int32_t)pos_array.size() );
            // add this vertex position to the master list
            pos_array.push_back( *ivert);
        }
        else
        {
            // add this vert to the smoothable list for the matching position
            old_to_new_vert_mapping.push_back(match_idx );
        }
    }
    m_Positions = pos_array;

    //fix tri data
    std::vector< uint32_t >::iterator itr = m_TriangleVertexIndices.begin();
    std::vector< uint32_t >::iterator end  = m_TriangleVertexIndices.end();
    for( ; itr != end; ++itr )
    {
        *itr = old_to_new_vert_mapping[*itr];
    }
    itr = m_WireframeVertexIndices.begin();
    end  = m_WireframeVertexIndices.end();
    for( ; itr != end; ++itr )
    {
        *itr = old_to_new_vert_mapping[*itr];
    }
}

uint32_t  Mesh::GetEdgeIdForVerts(uint32_t vert_a, uint32_t vert_b)
{
    std::vector< uint32_t >::const_iterator iter =  m_WireframeVertexIndices.begin();
    uint32_t edge_id = 0;
    for (; iter!= m_WireframeVertexIndices.end(); ++edge_id)
    {
        uint32_t vert_ids[2];
        vert_ids[0] = *iter;
        ++iter;
        vert_ids[1] = *iter;
        ++iter;
        if  ((vert_ids[0] == vert_a || vert_ids[0] == vert_b) &&
            (vert_ids[1] == vert_a || vert_ids[1] == vert_b) )
        {
            return edge_id;
        }
    }
    return 0xFFFFFFFF;
}

void Mesh::AddTri(uint32_t vert_a, uint32_t vert_b, uint32_t vert_c)
{
    m_TriangleVertexIndices.push_back(vert_a);
    m_TriangleVertexIndices.push_back(vert_b);
    m_TriangleVertexIndices.push_back(vert_c);
    if (GetEdgeIdForVerts(vert_a, vert_b) == 0xFFFFFFFF)
    {
        m_WireframeVertexIndices.push_back(vert_a);
        m_WireframeVertexIndices.push_back(vert_b);
    }
    if (GetEdgeIdForVerts(vert_b, vert_c) == 0xFFFFFFFF)
    {
        m_WireframeVertexIndices.push_back(vert_b);
        m_WireframeVertexIndices.push_back(vert_c);
    }
    if (GetEdgeIdForVerts(vert_c, vert_a) == 0xFFFFFFFF)
    {
        m_WireframeVertexIndices.push_back(vert_c);
        m_WireframeVertexIndices.push_back(vert_a);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// lifted from our igintersection (for now no bsphere broad culling. if performance of this sucks then I will put one in)
//  NearestPointInTriangle()
//  - given a point in the plane of the triangle, finds the nearest point in the triangle.
//    returns 0 if exterior and 1 if interior
//  Parameters:
//   - dv1:            (v0->v1)
//   - dv2:            (v0->v2)
//   - n:              normal
//   - dpp:            (v0->pp) -- this value is modified and returned as the result
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t NearestPointInTri(Vector3 dv1, Vector3 dv2, Vector3 normal, Vector3& dpp)
{
    Vector3 snap_a, snap_b;
    // check pp against edge (v0->v1)
    Vector3 edge = dpp.Cross(dv1);
    if ( edge.Dot(normal) < 0.0f)
    {
        snap_a.Set(0.0f, 0.0f, 0.0f);
        snap_b = dv1;
    }
    else
    {
        // check pp against edge (v0->v2)
        edge = dv2.Cross(dpp);
        if ( edge.Dot(normal)< 0.0f)
        {
            snap_a = dv2;
            snap_b.Set(0.0f, 0.0f, 0.0f);
        }
        else
        {
            // check pp against edge (v1->v2)
            Vector3 ev2 = dv2 - dv1;
            Vector3 epp = dpp - dv1;

            edge = epp.Cross(ev2);
            if ( edge.Dot(normal) < 0.0f)
            {
                snap_a = dv1;
                snap_b = dv2;
            }
            else
            {
                // return 'interior'
                return 1;
            }
        }
    }
    // snap to nearest point on line segment if pp is outside triangle 
    {
        Vector3 lvb  = snap_b - snap_a;
        Vector3 lpp  = dpp - snap_a;
        float32_t  dotn = lpp.Dot(lvb);
        float32_t  dotd = lvb.Dot(lvb);
        float32_t  t    = (dotn / dotd);
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        dpp = lvb * t;
        dpp = dpp + snap_a;
        // return 'exterior'
        return 0;
    }
}

uint32_t Mesh::GetClosestTri(const Vector3& sphere_pos, const float32_t& sphere_rad)
{
    uint32_t res_tri_id = 0xFFFFFFFF;
    float32_t   best_dist_sq = sphere_rad * sphere_rad;
    for (uint32_t i=0; i< ((uint32_t)m_TriangleVertexIndices.size()/3); ++i)
    {
        Vector3 tri_verts[3];
        tri_verts[0] = m_Positions[m_TriangleVertexIndices[3*i]];
        tri_verts[1] = m_Positions[m_TriangleVertexIndices[3*i+1]];
        tri_verts[2] = m_Positions[m_TriangleVertexIndices[3*i+2]];
        Vector3 dv1 = tri_verts[1] - tri_verts[0];
        Vector3 dv2 = tri_verts[2] - tri_verts[0];
        Vector3 dp  = sphere_pos - tri_verts[0];
        Vector3 normal = dv2.Cross(dv1);
        float32_t dotn = dp.Dot(normal);
        /* //IF we need to not include the back faced tris then uncomment this block
        if (dotn < 0.0f)
        {
        continue;
        }
        */
        float32_t dotd = normal.LengthSquared();
        // abort if normal is too short (degenerate triangle)
        if (dotd < 0.0000001f)//need to see where tools Epsilon is) 
        {
            continue;
        }
        float32_t t = (dotn / dotd);
        Vector3 dpp = normal * (-t);
        dpp = dpp + dp;
        // abort if nearest-point on plane is farther than radius
        float32_t dsq = normal.LengthSquared() * t * t;
        if (best_dist_sq < dsq)
        {
            continue;
        }
        NearestPointInTri(dv1, dv2, normal, dpp);
        Vector3 ip_to_closest_pt = dpp - dp;
        dsq = ip_to_closest_pt.LengthSquared();
        if (best_dist_sq < dsq)
        {
            continue;
        }
        best_dist_sq= dsq;
        res_tri_id = i;
    }
    return res_tri_id;
}

void Mesh::NopTrisByTriList(const std::vector< uint32_t >& ip_tris)
{
    for (std::vector< uint32_t >::const_iterator it=ip_tris.begin(); it!=ip_tris.end(); ++it)
    {
        HELIUM_ASSERT(*it <= (m_TriangleVertexIndices.size()/3));
        m_TriangleVertexIndices[(*it)*3] = 0xFFFFFFFF;
        m_TriangleVertexIndices[(*it)*3+1] = 0xFFFFFFFF;
        m_TriangleVertexIndices[(*it)*3+2] = 0xFFFFFFFF;
    }
}

void Mesh::NopTrisByVertList(const std::vector< uint32_t >& ip_verts)
{
    uint32_t num_tris = (uint32_t)(m_TriangleVertexIndices.size()/3);
    for (uint32_t i=0; i<num_tris; ++i)
    {
        uint32_t tr_vert_ids[3];
        tr_vert_ids[0] = m_TriangleVertexIndices[i*3];
        tr_vert_ids[1] = m_TriangleVertexIndices[i*3+1];
        tr_vert_ids[2] = m_TriangleVertexIndices[i*3+2];
        std::vector< uint32_t >::const_iterator it = std::find(ip_verts.begin(), ip_verts.end(), tr_vert_ids[0]);
        if (it != ip_verts.end())
        {
            m_TriangleVertexIndices[(i)*3] = 0xFFFFFFFF;
            m_TriangleVertexIndices[(i)*3+1] = 0xFFFFFFFF;
            m_TriangleVertexIndices[(i)*3+2] = 0xFFFFFFFF;
            continue;
        }
        it = std::find(ip_verts.begin(), ip_verts.end(), tr_vert_ids[1]);
        if (it != ip_verts.end())
        {
            m_TriangleVertexIndices[(i)*3] = 0xFFFFFFFF;
            m_TriangleVertexIndices[(i)*3+1] = 0xFFFFFFFF;
            m_TriangleVertexIndices[(i)*3+2] = 0xFFFFFFFF;
            continue;
        }
        it = std::find(ip_verts.begin(), ip_verts.end(), tr_vert_ids[2]);
        if (it != ip_verts.end())
        {
            m_TriangleVertexIndices[(i)*3] = 0xFFFFFFFF;
            m_TriangleVertexIndices[(i)*3+1] = 0xFFFFFFFF;
            m_TriangleVertexIndices[(i)*3+2] = 0xFFFFFFFF;
            continue;
        }
    }
}

void Mesh::NopTrisByEdgeList( const std::vector< uint32_t >& ip_edges )
{
    std::vector< uint32_t > tris;
    uint32_t num_tris = (uint32_t)(m_TriangleVertexIndices.size()/3);

    for (std::vector< uint32_t >::const_iterator it=ip_edges.begin(); it!=ip_edges.end(); it++)
    {
        uint32_t vert_1 = m_WireframeVertexIndices[(*it)*2];
        uint32_t vert_2 = m_WireframeVertexIndices[((*it)*2)+1];

        for (uint32_t i=0; i<num_tris; ++i)
        {
            uint32_t tr_vert_ids[3];
            tr_vert_ids[0] = m_TriangleVertexIndices[i*3];
            tr_vert_ids[1] = m_TriangleVertexIndices[i*3+1];
            tr_vert_ids[2] = m_TriangleVertexIndices[i*3+2];

            if ( (vert_1 == tr_vert_ids[0] || vert_1 == tr_vert_ids[1] || vert_1 == tr_vert_ids[2])
                && (vert_2 == tr_vert_ids[0] || vert_2 == tr_vert_ids[1] || vert_2 == tr_vert_ids[2]) )
            {
                m_TriangleVertexIndices[(i)*3] = 0xFFFFFFFF;
                m_TriangleVertexIndices[(i)*3+1] = 0xFFFFFFFF;
                m_TriangleVertexIndices[(i)*3+2] = 0xFFFFFFFF;
            }
        }
    }
}

void Mesh::PruneVertsNotInTris()
{
    std::vector< uint8_t > vert_is_in_tris;
    vert_is_in_tris.resize(m_Positions.size(), 0);
    for (std::vector< uint32_t >::const_iterator it=m_TriangleVertexIndices.begin(); it!=m_TriangleVertexIndices.end(); ++it)
    {
        if ((*it) != 0xFFFFFFFF)
        {
            vert_is_in_tris[*it] = 1;
        }
    }
    std::vector< uint32_t > vert_remap;
    vert_remap.resize(m_Positions.size(), 0xFFFFFFFF);
    uint32_t next_vert_index = 0;
    uint32_t vert_id = 0;
    for (std::vector< uint8_t >::const_iterator it=vert_is_in_tris.begin(); it!=vert_is_in_tris.end(); ++it, ++vert_id)
    {
        if ((*it))
        {
            vert_remap[vert_id] = next_vert_index;
            m_Positions[next_vert_index] = m_Positions[vert_id];
            ++next_vert_index;
        }
    }
    m_Positions.resize(next_vert_index);
    std::vector< uint32_t > new_tri_vert_ids;
    std::vector< uint32_t > new_edge_vert_ids;
    new_tri_vert_ids.reserve(m_TriangleVertexIndices.size());
    new_edge_vert_ids.reserve(m_TriangleVertexIndices.size());
    uint32_t old_tri_cnt = (uint32_t)(m_TriangleVertexIndices.size()/3);
    for (uint32_t i=0; i<old_tri_cnt; ++i)
    {
        uint32_t tr_vert_ids[3];
        tr_vert_ids[0] = m_TriangleVertexIndices[i*3];
        tr_vert_ids[1] = m_TriangleVertexIndices[i*3+1];
        tr_vert_ids[2] = m_TriangleVertexIndices[i*3+2];
        if (tr_vert_ids[0] == 0xFFFFFFFF)
        {
            continue;
        }
        HELIUM_ASSERT(tr_vert_ids[1] != 0xFFFFFFFF && tr_vert_ids[2] != 0xFFFFFFFF);
        new_tri_vert_ids.push_back(vert_remap[tr_vert_ids[0]]);
        new_tri_vert_ids.push_back(vert_remap[tr_vert_ids[1]]);
        new_tri_vert_ids.push_back(vert_remap[tr_vert_ids[2]]);

        //duplicates..but who cares whole concept of having wireframe edge ids isnt good as we could render tris wireframe
        new_edge_vert_ids.push_back(vert_remap[tr_vert_ids[0]]);
        new_edge_vert_ids.push_back(vert_remap[tr_vert_ids[1]]);
        new_edge_vert_ids.push_back(vert_remap[tr_vert_ids[1]]);
        new_edge_vert_ids.push_back(vert_remap[tr_vert_ids[2]]);
        new_edge_vert_ids.push_back(vert_remap[tr_vert_ids[2]]);
        new_edge_vert_ids.push_back(vert_remap[tr_vert_ids[0]]);
    }
    m_TriangleVertexIndices = new_tri_vert_ids;
    m_WireframeVertexIndices = new_edge_vert_ids;
}

void Mesh::DeleteTris(const std::vector< uint32_t >& ip_tris)
{
    NopTrisByTriList(ip_tris);
    PruneVertsNotInTris();
}
void Mesh::DeleteVerts(const std::vector< uint32_t >& ip_verts)
{
    NopTrisByVertList(ip_verts);
    PruneVertsNotInTris();
}

void Mesh::DeleteEdges( const std::vector< uint32_t >& ip_edges )
{
    NopTrisByEdgeList( ip_edges );
    PruneVertsNotInTris();
}

uint32_t Mesh::GetClosestVert(const Vector3& sphere_start_pos, const float32_t& sphere_rad, const Vector3& swept_dir, const float32_t& len)
{
    uint32_t res_vert_index = 0xFFFFFFFF;
    float32_t min_dist = len + 2.0f*sphere_rad;
    float32_t sphere_rad_sqr = sphere_rad*sphere_rad;
    uint32_t i=0;
    for (V_Vector3::const_iterator iter = m_Positions.begin(); iter != m_Positions.end(); ++iter, ++i)
    {
        Vector3 vec_to_pt = *iter - sphere_start_pos;
        float32_t dot = vec_to_pt.Dot(swept_dir);
        dot = Clamp(dot, 0.0f, len);
        Vector3 closest_pt_on_axis = swept_dir*dot;
        Vector3 closest_pt_to_pos = closest_pt_on_axis - vec_to_pt;
        float32_t dist_sqr = closest_pt_to_pos.LengthSquared();
        if ( dist_sqr < sphere_rad_sqr)
        {
            if (dot < min_dist)
            {
                min_dist = dot;
                res_vert_index = i;
            }
        }
    }
    return res_vert_index;
}

uint32_t Mesh::GetClosestVert(const Matrix4& view_proj_mat, const float32_t porj_space_threshold_sqr, Vector2 proj_pt)
{
    uint32_t res_vert_index = 0xFFFFFFFF;
    float32_t min_z = 1.0f;
    uint32_t i=0;
    for (V_Vector3::const_iterator iter = m_Positions.begin(); iter != m_Positions.end(); ++iter, ++i)
    {
        Vector4 v ( iter->x, iter->y, iter->z, 1.f );
        view_proj_mat.Transform( v );
        v /= (v.w);

        if (v.z <0.0f || v.z > min_z || v.x < -1.0f  || v.x > 1.0f || v.y < -1.0f || v.y > 1.0f)
        {
            continue;
        }
        Vector2 v_pt;
        v_pt.x = v.x;
        v_pt.y = v.y;
        v_pt -= proj_pt;
        float32_t v_pt_len_sqr = v_pt.LengthSquared();
        if ( v_pt_len_sqr < porj_space_threshold_sqr)
        {
            min_z = v.z;
            res_vert_index = i;
        }
    }
    return res_vert_index;
}

void ClosestPtsOnLinesegVsLineseg(const Vector3& line_a_start, const Vector3& line_a_end,
                                  const Vector3& line_b_start, const Vector3& line_b_end,
                                  Vector3& closest_pt_on_line_a, Vector3& closest_pt_on_line_b, float32_t& linea_a_t, float32_t& line_b_t)
{

    Vector3 l0 = line_a_end - line_a_start;
    Vector3 l1 = line_b_end - line_b_start;
    Vector3 ds = line_b_start - line_a_start;

    Vector3 c  = l0.Cross(l1);
    Vector3 c0 = ds.Cross(l1);
    Vector3 c1 = ds.Cross(l0);

    float32_t t0 = c0.Dot(c) / c.Dot(c);
    float32_t t1 = c1.Dot(c) / c.Dot(c);

    t0 = t0<0 ? 0 : t0>1 ? 1 : t0;
    t1 = t1<0 ? 0 : t1>1 ? 1 : t1;

    float32_t d0 = t1*l0.Dot(l1) + ds.Dot(l0);
    float32_t d1 = t0*l0.Dot(l1) - ds.Dot(l1);

    linea_a_t = d0 / l0.Dot(l0);
    line_b_t = d1 / l1.Dot(l1);

    linea_a_t = linea_a_t<0 ? 0 : linea_a_t>1 ? 1 : linea_a_t;
    line_b_t = line_b_t<0 ? 0 : line_b_t>1 ? 1 : line_b_t;

    closest_pt_on_line_a = line_a_start + l0*linea_a_t;
    closest_pt_on_line_b = line_b_start + l1*line_b_t;
}
uint32_t Mesh::GetClosestEdge(const Vector3& sphere_start_pos, const float32_t& sphere_rad, const Vector3& swept_dir, const float32_t& len)
{
    uint32_t res_edge_index = 0xFFFFFFFF;
    float32_t min_dist = len;
    float32_t sphere_rad_sqr = sphere_rad*sphere_rad;
    Vector3 ss_end = sphere_start_pos + swept_dir*len;
    uint32_t i=0;

    std::vector< uint32_t >::const_iterator iter =  m_WireframeVertexIndices.begin();
    uint32_t edge_id = 0;
    for (; iter!= m_WireframeVertexIndices.end(); ++edge_id)
    {
        uint32_t vert_ids[2];
        vert_ids[0] = *iter;
        ++iter;
        vert_ids[1] = *iter;
        ++iter;
        Vector3& edge_verts_0 = m_Positions[vert_ids[0]];
        Vector3& edge_verts_1 = m_Positions[vert_ids[1]];
        Vector3 closest_pt_ss, closest_pt_edge;
        float32_t ss_t, edge_t;
        ClosestPtsOnLinesegVsLineseg(sphere_start_pos, ss_end, edge_verts_0, edge_verts_1, closest_pt_ss, closest_pt_edge, ss_t, edge_t);
        Vector3 diff = closest_pt_edge - closest_pt_ss;
        if (diff.LengthSquared() < sphere_rad_sqr)
        {
            float32_t d = ss_t * len;
            if (min_dist > d)
            {
                min_dist = d;
                res_edge_index = edge_id;
            }
        }
    }
    return res_edge_index;
}

uint32_t Mesh::GetClosestTri(const Vector3& sphere_start_pos, const float32_t& sphere_rad, const Vector3& swept_dir, const float32_t& len)
{
    uint32_t res_tri_index = 0xFFFFFFFF;
    float32_t min_dist = len;
    float32_t sphere_rad_sqr = sphere_rad*sphere_rad;
    Vector3 ss_end = sphere_start_pos + swept_dir*len;

    std::vector< uint32_t >::const_iterator iter =  m_TriangleVertexIndices.begin();
    uint32_t tri_id = 0;
    for (; iter!= m_TriangleVertexIndices.end(); ++tri_id)
    {
        uint32_t vert_ids[3];
        vert_ids[0] = *iter;
        ++iter;
        vert_ids[1] = *iter;
        ++iter;
        vert_ids[2] = *iter;
        ++iter;
        Vector3 v[3];
        v[0] = m_Positions[vert_ids[0]];
        v[1] = m_Positions[vert_ids[1]];
        v[2] = m_Positions[vert_ids[2]];
        Vector3 dv1 = v[1] - v[0];
        Vector3 dv2 = v[2] - v[0];
        Vector3 plane_normal = dv2.Cross(dv1);
        plane_normal = plane_normal.Normalize();
        float32_t plane_d = plane_normal.Dot(v[0]);
        float32_t temp_dot = swept_dir.Dot(plane_normal);
        if (fabs(temp_dot) < 0.0001f)
        {
            continue;
        }
        float32_t t = plane_d - sphere_start_pos.Dot(plane_normal);
        t = t/temp_dot;
        Vector3 pt_on_plane = sphere_start_pos + swept_dir*t;
        //see if the pt is inside the tri
        bool outside = false;
        for (uint32_t j=0; j<3; ++j)
        {
            Vector3 v_to_pt = pt_on_plane - v[j];
            Vector3 edge = v[(j+1)%3] - v[j];
            Vector3 cross_p = v_to_pt.Cross(edge);
            if (cross_p.Dot(plane_normal) < 0.0f)
            {
                outside = true;
                break;
            }
        }
        if (!outside && t < min_dist)
        {
            min_dist = t;
            res_tri_index = tri_id;
        }
    }
    return res_tri_index;
}

void Mesh::GetEdges( S_MeshEdge& edges ) const 
{
    std::vector< uint32_t >::const_iterator itr = m_TriangleVertexIndices.begin();
    std::vector< uint32_t >::const_iterator end = m_TriangleVertexIndices.end();
    for( ; itr != end; itr+=3 )
    {
        edges.insert( MeshEdge( *itr, *(itr+1) ) );
        edges.insert( MeshEdge( *(itr+1), *(itr+2) ) );
        edges.insert( MeshEdge( *(itr+2), *itr ) );
    }
}

float32_t Mesh::VertDensity() const
{
    S_MeshEdge edges;
    GetEdges( edges );

    float32_t edgeLength = 0.0f;

    S_MeshEdge::const_iterator itr = edges.begin();
    S_MeshEdge::const_iterator end = edges.end();
    for( ; itr != end; ++itr )
    {
        edgeLength += (m_Positions[ itr->m_VertIndices[0]] - m_Positions[ itr->m_VertIndices[1] ]).Length();
    }

    return ( edges.size() * 2.0f ) / edgeLength;
}

bool ValidFloat(float f_)
{
    return (!_isnan(f_) && f_ != FLT_MAX && f_ != -FLT_MAX);
}
#pragma warning (default:4056)
#pragma warning (default:4756)
bool ValidVec3(const Vector3& v_) //need to move this some where
{
    return ( ValidFloat(v_.x) && ValidFloat(v_.y) && ValidFloat(v_.z) );
}
#define ZER_AREA 1.0e-6
void Mesh::PruneInvalidTris()
{
    std::vector< uint32_t >::const_iterator iter =  m_TriangleVertexIndices.begin();
    uint32_t tri_id = 0;
    std::vector< uint32_t > invalid_tris;
    for (; iter!= m_TriangleVertexIndices.end(); ++tri_id)
    {
        uint32_t vert_ids[3];
        vert_ids[0] = *iter;
        ++iter;
        vert_ids[1] = *iter;
        ++iter;
        vert_ids[2] = *iter;
        ++iter;
        Vector3 v[3];
        v[0] = m_Positions[vert_ids[0]];
        v[1] = m_Positions[vert_ids[1]];
        v[2] = m_Positions[vert_ids[2]];
        Vector3 dv1 = v[1] - v[0];
        Vector3 dv2 = v[2] - v[0];
        Vector3 plane_normal = dv2.Cross(dv1);
        float32_t area =  plane_normal.Length()/2.0f;
        if (area < ZER_AREA || (!ValidVec3(v[0])) || (!ValidVec3(v[1])) || (!ValidVec3(v[2])))
        {
            invalid_tris.push_back(tri_id);
        }
    }
    DeleteTris(invalid_tris);
}

void Mesh::MergeVertToClosest(uint32_t ip_vert_id)
{
    uint32_t closest_vert_id = 0xFFFFFFFF;
    float32_t min_dist_sqr = 10000.0f;//ok this should be big enough ever else one is screwing some thing bad
    uint32_t i=0;
    Vector3 input_vert_pos = m_Positions[ip_vert_id];
    for (V_Vector3::const_iterator iter = m_Positions.begin(); iter != m_Positions.end(); ++iter, ++i)
    {
        if (i == ip_vert_id)
        {
            continue;
        }
        Vector3 diff = *iter - input_vert_pos;
        float32_t diff_len_sqr = diff.LengthSquared();
        if ( diff_len_sqr < min_dist_sqr)
        {
            min_dist_sqr = diff_len_sqr;
            closest_vert_id = i;
        }
    }
    if (closest_vert_id == 0xFFFFFFFF)
    {
        return;
    }
    //swap verts in the tris which have just the input vert and not the closest vert and delete the ones which have both
    std::vector< uint32_t > tris_to_be_deleted;//ones with both vert_id and closest_vert_id
    {
        uint32_t tri_id = 0;
        std::vector< uint32_t > invalid_tris;
        std::vector< uint32_t >::const_iterator iter =  m_TriangleVertexIndices.begin();
        for (; iter!= m_TriangleVertexIndices.end(); ++tri_id)
        {
            uint32_t vert_ids[3];
            vert_ids[0] = *iter;
            ++iter;
            vert_ids[1] = *iter;
            ++iter;
            vert_ids[2] = *iter;
            ++iter;
            uint32_t has_ip_vert_id = 0;
            bool has_closest_vert_id = false;
            for (uint32_t j=0; j<3; ++j)
            {
                if (vert_ids[j] == ip_vert_id)
                {
                    has_ip_vert_id = j+1;
                }
                if (vert_ids[j] == closest_vert_id)
                {
                    has_closest_vert_id = true;
                }
            }
            if (has_closest_vert_id && has_ip_vert_id)
            {
                tris_to_be_deleted.push_back(tri_id);
            }
            else if (has_ip_vert_id)
            {
                //swap the ip_vert_id with the closest_vert_id
                m_TriangleVertexIndices[3*tri_id + has_ip_vert_id-1] = closest_vert_id;
            }
        }
    }
    DeleteTris(tris_to_be_deleted);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Given an convex poly P and a triangle T, this function triangulates the set difference T-P.
// All points are expressed as vec4f's but only the x & z components are used.
//
// This is done in 4 stages:
// 1: create a set of unclipped edges from the poly verts
// 2: clip the edges to each triangle halfspace in turn
// 3: join each surviving edge to an appropriate triangle vertex
// 4: add up to 3 gap-filling triangles (one wherever an original triangle edge remains fully outside)
//
// Input:
// tri    - the start of an array of 3 vec4f's
// poly   - the start of an array of any number (>=3) of vec4f's
// nverts - the number of verts in the poly
//
// Output:
// out    - this array will be filled with triplets of vec4f's representing the output triangles
// return value - the number of triangles output
//
// Note that no bounds checking is done on the output; the array 'out' should therefore be large enough
// to hold the worst-case output which is 3*nverts+9 vec4f's.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float32_t CrossVectorXZ(const Vector3& v1, const Vector3& v2)
{
    return(v2.x * v1.z - v1.x * v2.z);
}

void LerpVector3(Vector3& r, const Vector3& v1, const Vector3& v2, float32_t t)
{
    r.x = v1.x + (v2.x - v1.x) * t;
    r.y = v1.y + (v2.y - v1.y) * t;
    r.z = v1.z + (v2.z - v1.z) * t;
}

int32_t SenseOfTriangle(Vector3 &v0, Vector3 &v1, Vector3 &v2)
{
    Vector3 dv0, dv1;
    dv0 = v1 - v0;
    dv1 = v2 - v1;
    float32_t c = CrossVectorXZ(dv0, dv1);
    return (c >= 0.0f) ? 1 : -1;
}

uint32_t TriMinusPoly(Vector3 *out, Vector3 *tri, Vector3 *poly, uint32_t nverts)
{
    Vector3 vP0, vP1, vP2, dvP, vT0, vT1, dvT, dv0, dv1, dv2, *p_in, *p_out, *p_end, vP0_prev, vP1_prev, vP2_prev;
    // determine anticlockwise / clockwise sense of tri and poly
    float32_t sense_of_source_tri  = (float32_t)SenseOfTriangle(poly[0], poly[1], poly[2]);
    int32_t sense_of_source_poly = SenseOfTriangle(tri[0],  tri[1],  tri[2]);


    // initialise the set of unclipped poly edges (clockwise)
    vP1 = poly[nverts-1];
    p_end = out;
    for (uint32_t i=0; i<nverts; i++)
    {
        vP0 = vP1;
        vP1 = poly[i];
        *p_end++ = vP0;
        *p_end++ = vP1;
        p_end++;
    }

    // loop over the 3 halfspaces of the tri, each time clipping the current set of edges
    vT1 = tri[2];
    for (uint32_t i=0; i<3; i++)
    {
        // get tri edge
        vT0 = vT1;
        vT1 = tri[i];
        //SubVector4(dvT, vT1, vT0);
        dvT = vT1 - vT0;

        // loop over current set of poly edges
        p_in = p_out = out;   // we can overwrite the edges in place
        while (p_in < p_end)
        {
            // get poly edge
            vP0 = *p_in++;
            vP1 = *p_in++;
            p_in++;
            //SubVector4(dvP, vP1, vP0);
            dvP = vP1 - vP0;

            // edge endpoints relative to vT0
            //SubVector4(dv0, vP0, vT0);
            dv0 = vP0 - vT0;
            //SubVector4(dv1, vP1, vT0);
            dv1 = vP1 - vT0;

            // cross prods for classification and intersection
            float32_t c0 = CrossVectorXZ(dvT, dv0)* sense_of_source_tri;
            float32_t c1 = CrossVectorXZ(dvT, dv1)* sense_of_source_tri;

            // don't output degenerate edges
            if (c0<=0 && c1<=0)
            {
                continue;
            }

            // conditionally output initial endpoint
            if (c0>=0) *p_out++ = vP0;

            // conditionally output intersection
            if ((c0<0) ^ (c1<0)) LerpVector3(*p_out++, vP0, vP1, c0/(c0-c1));

            // conditionally output final endpoint
            if (c1>=0) *p_out++ = vP1;

            // conditionally output pad
            if (c0>=0 || c1>=0) p_out++;
        }

        p_end = p_out;
        if (p_end == out) return 0;
    }

    // loop over clipped edges and add a 3rd vertex to complete each triangle
    p_out  = out;
    uint32_t tri_verts_out  = 0;
    uint32_t tri_verts_used = 0;
    while (p_out < p_end)
    {
        // get poly edge
        vP0 = *p_out++;
        vP1 = *p_out++;
        //SubVector4(dvP, vP1, vP0);
        dvP = vP1 - vP0;

        // output tri vert furthest outside poly edge
        //SubVector4(dv0, tri[0], vP0);
        dv0 = tri[0] - vP0;
        //SubVector4(dv1, tri[1], vP0);
        dv1 = tri[1] - vP0;
        //SubVector4(dv2, tri[2], vP0);
        dv2 = tri[2] - vP0;

        float32_t c[3];
        c[0] = CrossVectorXZ(dvP, dv0);
        c[1] = CrossVectorXZ(dvP, dv1);
        c[2] = CrossVectorXZ(dvP, dv2);


        uint32_t tri_vert = (c[0] < c[1]) ? (c[0] < c[2]) ? 0 : 2 : (c[1] < c[2]) ? 1 : 2;

        *p_out++ = tri[tri_vert];

        tri_verts_used |= (1 << tri_vert);
        tri_verts_out  |= ((c[0] < 0.0f) << 0);
        tri_verts_out  |= ((c[1] < 0.0f) << 1);
        tri_verts_out  |= ((c[2] < 0.0f) << 2);
    }

    // find loose vert (if any)
    uint32_t loose_vert_flags = tri_verts_out & ~tri_verts_used;
    int32_t loose_vert = -1;
    if (loose_vert_flags & 0x1)
    {
        loose_vert = 0;
    }
    else if (loose_vert_flags & 0x2)
    {
        loose_vert = 1;
    }
    else if (loose_vert_flags & 0x4)
    {
        loose_vert = 2;
    }

    // loop over tris and fill in any gaps
    p_in = out;
    p_out = p_end;
    vP0 = p_end[-3];
    vP1 = p_end[-2];
    vP2 = p_end[-1];
    while (p_in < p_end)
    {
        vP0_prev = vP0;
        vP1_prev = vP1;
        vP2_prev = vP2;

        vP0 = *p_in++;
        vP1 = *p_in++;
        vP2 = *p_in++;

        if (vP1_prev.x==vP0.x && vP1_prev.z==vP0.z)
        {
            if (vP2_prev.x!=vP2.x || vP2_prev.z!=vP2.z)
            {
                // ensure the gap-filling tri has the correct sense
                if (SenseOfTriangle(vP0, vP2, vP2_prev) != sense_of_source_poly)
                {
                    // add a gap-filling tri
                    *p_out++ = vP0;
                    *p_out++ = vP2;
                    *p_out++ = vP2_prev;
                }
                else if (loose_vert >= 0)
                {
                    // attach loose vertex to its neighbouring tris
                    *p_out++ = vP2_prev;
                    *p_out++ = vP0;
                    *p_out++ = tri[loose_vert];

                    *p_out++ = vP0;
                    *p_out++ = vP2;
                    *p_out++ = tri[loose_vert];
                }
            }
        }
        else if (loose_vert >= 0)
        {
            // treat isolated triangles attaching to loose vertex
            if ((SenseOfTriangle(tri[loose_vert], vP0_prev, vP1_prev) != sense_of_source_poly) &&
                (SenseOfTriangle(tri[loose_vert], vP2_prev, vP1_prev) != sense_of_source_poly))
            {
                *p_out++ = vP2_prev;
                *p_out++ = vP1_prev;
                *p_out++ = tri[loose_vert];
            }

            if ((SenseOfTriangle(tri[loose_vert], vP0, vP1) != sense_of_source_poly) &&
                (SenseOfTriangle(tri[loose_vert], vP0, vP2) != sense_of_source_poly))
            {
                *p_out++ = vP0;
                *p_out++ = vP2;
                *p_out++ = tri[loose_vert];
            }
        }
    }

    return (uint32_t)((p_out - out) / 3);
}

void Mesh::PunchCubeHole(Matrix4& mat, Matrix4& inv_mat, float32_t vert_merge_threshold)
{
    AlignedBox unit_box;
    unit_box.minimum.Set(-1.0f, -1.0f, -1.0f);
    unit_box.maximum.Set(1.0f, 1.0f, 1.0f);
    Frustum frustom(unit_box);
    V_Vector3 new_tri_verts;
    //swap verts in the tris which have just the input vert and not the closest vert and delete the ones which have both
    std::vector< uint32_t > tris_to_be_deleted;//ones with both vert_id and closest_vert_id
    {
        uint32_t tri_id = 0;
        std::vector< uint32_t >::const_iterator iter =  m_TriangleVertexIndices.begin();
        for (; iter!= m_TriangleVertexIndices.end(); ++tri_id)
        {
            uint32_t vert_ids[3];
            vert_ids[0] = *iter;
            ++iter;
            vert_ids[1] = *iter;
            ++iter;
            vert_ids[2] = *iter;
            ++iter;
            Vector3 v[3];
            v[0] = m_Positions[vert_ids[0]];
            v[1] = m_Positions[vert_ids[1]]; 
            v[2] = m_Positions[vert_ids[2]];
            inv_mat.TransformVertex(v[0]);
            inv_mat.TransformVertex(v[1]);
            inv_mat.TransformVertex(v[2]);
            Vector3 tri_normal;
            Vector3 tri_edge_0 = v[1] - v[0];
            Vector3 tri_edge_1 = v[2] - v[0];
            tri_normal = tri_edge_0.Cross(tri_edge_1);
            tri_normal.Normalize();

            if (tri_normal.y < 0.0f)
            {
                Vector3 temp = v[2];
                v[2] = v[1];
                v[1] = temp;
                tri_normal *= -1.0f;
            }
            float32_t plane_w = tri_normal.Dot(v[0]);
            //v[0].y = v[1].y = v[2].y = 0.0f;


            if (frustom.IntersectsTriangle(v[0], v[1], v[2]))//if the box intersects the tri then send it for clipping in the XZ plane
            {
                v[0].y = v[1].y = v[2].y = 0.0f;
                tris_to_be_deleted.push_back(tri_id);
                Vector3 out_put_verts[21];
                Vector3 convex_poly[4];
                convex_poly[0].x = 1.0f;
                convex_poly[0].z = 1.0f;
                convex_poly[0].y = 0.0f;
                convex_poly[1].x = 1.0f;
                convex_poly[1].z = -1.0f;
                convex_poly[1].y = 0.0f;
                convex_poly[2].x = -1.0f;
                convex_poly[2].z = -1.0f;
                convex_poly[2].y = 0.0f;
                convex_poly[3].x = -1.0f;
                convex_poly[3].z = 1.0f;
                convex_poly[3].y = 0.0f;
                uint32_t num_clipped_tris = TriMinusPoly(out_put_verts, v, convex_poly, 4);
                for (uint32_t n_v=0; n_v<3*num_clipped_tris; ++n_v)
                {
                    Vector3& new_v = out_put_verts[n_v];
                    new_v.y = (plane_w - new_v.Dot(tri_normal))/tri_normal.y;
                    mat.TransformVertex(new_v);
                    new_tri_verts.push_back(new_v);
                }
            }
        }
    }

    if (tris_to_be_deleted.size())
    {
        uint32_t num_current_verts = (uint32_t)m_Positions.size();
        m_Positions.insert(m_Positions.end(), new_tri_verts.begin(), new_tri_verts.end());

        for (uint32_t n_t_id=0; n_t_id<(uint32_t)new_tri_verts.size(); n_t_id+=3)
        {
            uint32_t start = num_current_verts+n_t_id;
            AddTri(start, start+1, start+2);
        }
        DeleteTris(tris_to_be_deleted);
        WeldMeshVerts(0.001f);
    }
}

uint32_t Mesh::AddShader( Shader* shader )
{
    uint32_t index = 0;
    for ( V_ShaderDumbPtr::const_iterator itr = m_Shaders.begin(), end = m_Shaders.end(); itr != end; ++itr, ++index )
    {
        if ( *itr == shader )
        {
            return index;
        }
    }

    if ( m_Owner )
    {
        m_Owner->AddObject( shader );
    }

    m_Shaders.push_back( shader );
    m_ShaderIDs.push_back( shader->GetID() );
    return (uint32_t)m_Shaders.size() - 1;
}