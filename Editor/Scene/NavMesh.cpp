
#include "Precompile.h"
#include "NavMesh.h"

#include "Pipeline/Content/Nodes/Camera.h"
#include "Pipeline/Content/Nodes/Mesh.h"

#include "Pick.h"
#include "PrimitiveLocator.h"
#include "PrimitiveCone.h"
#include "Color.h"
#include "Scene.h"
#include "SceneManager.h"
#include "NavMeshCreateTool.h"
#include "Statistics.h"
#include "HierarchyNodeType.h"
#include "ReverseChildrenCommand.h"


#include "Application/UI/ArtProvider.h"
#include "PropertiesGenerator.h"
#include "Application/Undo/PropertyCommand.h"
#include "Foundation/Log.h"
#include "Orientation.h"

#include "Foundation/Math/Curve.h"
#include "Foundation/Math/AngleAxis.h"

#include <algorithm>

using namespace Editor;

LUNA_DEFINE_TYPE( Editor::NavMesh );

D3DMATERIAL9 Editor::NavMesh::s_Material;
D3DMATERIAL9 Editor::NavMesh::s_HullMaterial;


D3DMATERIAL9 Editor::NavMesh::s_HiResVertDisplayMaterial;
D3DMATERIAL9 Editor::NavMesh::s_LowResVertDisplayMaterial;
D3DMATERIAL9 Editor::NavMesh::s_HiResEdgeDisplayMaterial;
D3DMATERIAL9 Editor::NavMesh::s_LowResEdgeDisplayMaterial;
D3DMATERIAL9 Editor::NavMesh::s_HiResTriDisplayMaterial;
D3DMATERIAL9 Editor::NavMesh::s_LowResTriDisplayMaterial;
D3DMATERIAL9 Editor::NavMesh::s_HiResTriSubDuedDisplayMaterial;
D3DMATERIAL9 Editor::NavMesh::s_LowResTriSubDuedDisplayMaterial;
D3DMATERIAL9 Editor::NavMesh::s_MouseOverTriMaterial;
D3DMATERIAL9 Editor::NavMesh::s_SelectedTriMaterial;


class SelectionDataObject : public Reflect::Object
{
public:

  OS_SelectableDumbPtr m_Selection;

  SelectionDataObject( const OS_SelectableDumbPtr& selection )
    : m_Selection( selection)
  {
  }

};


void NavMesh::InitializeType()
{
  Reflect::RegisterClass< Editor::NavMesh >( TXT( "Editor::NavMesh" ) );

  ZeroMemory(&s_Material, sizeof(s_Material));
  s_Material.Ambient = Editor::Color::FORESTGREEN;

  ZeroMemory(&s_HullMaterial, sizeof(s_HullMaterial));
  s_HullMaterial.Ambient = Editor::Color::GRAY;

  ZeroMemory(&s_HiResVertDisplayMaterial, sizeof(s_HiResVertDisplayMaterial));
  s_HiResVertDisplayMaterial.Ambient = Color::ColorToColorValue(128, 0, 255, 255);
  s_HiResVertDisplayMaterial.Diffuse = Color::ColorToColorValue(128, 0, 255, 255);
  ZeroMemory(&s_LowResVertDisplayMaterial, sizeof(s_LowResVertDisplayMaterial));
  s_LowResVertDisplayMaterial.Ambient = Color::ColorToColorValue(128, 0, 0, 255);
  s_LowResVertDisplayMaterial.Diffuse = Color::ColorToColorValue(128, 0, 0, 255);
  ZeroMemory(&s_HiResEdgeDisplayMaterial, sizeof(s_HiResEdgeDisplayMaterial));
  s_HiResEdgeDisplayMaterial.Ambient = Color::ColorToColorValue(128, 0, 255, 0);
  s_HiResEdgeDisplayMaterial.Diffuse = Color::ColorToColorValue(128, 0, 255, 0);
  ZeroMemory(&s_LowResEdgeDisplayMaterial, sizeof(s_LowResEdgeDisplayMaterial));
  s_LowResEdgeDisplayMaterial.Ambient = Color::ColorToColorValue(128, 0, 0, 255);
  s_LowResEdgeDisplayMaterial.Diffuse = Color::ColorToColorValue(128, 0, 0, 255);
  ZeroMemory(&s_HiResTriDisplayMaterial, sizeof(s_HiResTriDisplayMaterial));
  s_HiResTriDisplayMaterial.Ambient = Color::ColorToColorValue(128, 0, 128, 0);
  s_HiResTriDisplayMaterial.Diffuse = Color::ColorToColorValue(128, 0, 128, 0);
  ZeroMemory(&s_LowResTriDisplayMaterial, sizeof(s_LowResTriDisplayMaterial));
  s_LowResTriDisplayMaterial.Ambient = Color::ColorToColorValue(128, 0, 0, 128);
  s_LowResTriDisplayMaterial.Diffuse = Color::ColorToColorValue(128, 0, 0, 128);
  ZeroMemory(&s_HiResTriSubDuedDisplayMaterial, sizeof(s_HiResTriSubDuedDisplayMaterial));
  s_HiResTriSubDuedDisplayMaterial.Ambient = Color::ColorToColorValue(98, 0, 48, 0);
  s_HiResTriSubDuedDisplayMaterial.Diffuse = Color::ColorToColorValue(98, 0, 48, 0);
  ZeroMemory(&s_LowResTriSubDuedDisplayMaterial, sizeof(s_LowResTriSubDuedDisplayMaterial));
  s_LowResTriSubDuedDisplayMaterial.Ambient = Color::ColorToColorValue(98, 0, 0, 48);
  s_LowResTriSubDuedDisplayMaterial.Diffuse = Color::ColorToColorValue(98, 0, 0, 48);
  ZeroMemory(&s_MouseOverTriMaterial, sizeof(s_MouseOverTriMaterial));
  s_MouseOverTriMaterial.Ambient = Color::ColorToColorValue(128, 0, 255, 255);
  s_MouseOverTriMaterial.Diffuse = Color::ColorToColorValue(128, 0, 255, 255);
  ZeroMemory(&s_SelectedTriMaterial, sizeof(s_SelectedTriMaterial));
  s_SelectedTriMaterial.Ambient = Color::ColorToColorValue(98, 255, 255, 0);
  s_SelectedTriMaterial.Diffuse = Color::ColorToColorValue(98, 255, 255, 0);
  PropertiesGenerator::InitializePanel( TXT( "NavMesh" ), CreatePanelSignature::Delegate( &NavMesh::CreatePanel ) );
}

void NavMesh::CleanupType()
{
  Reflect::UnregisterClass< Editor::NavMesh >();
}

NavMesh::NavMesh( Editor::Scene* scene, Content::Mesh* mesh )
: Editor::Mesh ( scene, mesh )
{
  m_Locator = new Editor::PrimitiveLocator( m_Scene->GetViewport()->GetResources() );
  m_Locator->Update();
  if (mesh->m_TriangleVertexIndices.size() == 0)
  {
    mesh->m_GivenName = TXT( "HiResNavMesh" );
    mesh->m_UseGivenName = true;
  }
   
  m_mouse_over_vert = m_mouse_over_edge = m_mouse_over_tri = 0xFFFFFFFF;
  m_selected_verts.clear();
  m_selected_tris.clear();
  m_selected_edge = 0xFFFFFFFF;
  m_cached_selected_tris.clear();
  m_SelectionMode = NavMeshCreateTool::MOUSE_HOVER_SELECT_VERT;
  m_DrawLocator = true;
}

NavMesh::~NavMesh()
{
  delete m_Locator;
}

i32 NavMesh::GetImageIndex() const
{
  return Helium::GlobalFileIconsTable().GetIconID( TXT( "mesh" ) );
}

tstring NavMesh::GetApplicationTypeName() const
{
  return TXT( "NavMesh" );
}

void NavMesh::Initialize()
{
  __super::Initialize();
}

u32 NavMesh::GetNumberVertices() const
{
  const Content::Mesh* mesh = GetPackage< Content::Mesh >();
  return (u32) mesh->m_Positions.size();
}

void NavMesh::Create()
{
  __super::Create();

  m_Vertices->Create();
  m_Locator->Create();
  m_Vertices->SetElementCount( m_VertexCount );
  m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Vertices->Update();
}

void NavMesh::Delete()
{
  __super::Delete();

  m_Vertices->SetElementCount( 0 );
  m_Vertices->Delete();

  m_Locator->Delete();
}

void NavMesh::Populate( PopulateArgs* args )
{
  Content::Mesh* mesh = GetPackage<Content::Mesh>();

  switch( args->m_Type )
  {
  case ResourceTypes::Index:
    {
      if ( args->m_Buffer != NULL )
      {
        if ( mesh->m_WireframeVertexIndices.size())
        {
          memcpy( args->m_Buffer + args->m_Offset, &( mesh->m_WireframeVertexIndices.front() ), mesh->m_WireframeVertexIndices.size() * sizeof( u32 ) );
          args->m_Offset += ( (u32)mesh->m_WireframeVertexIndices.size() * sizeof( u32 ) );
        }
        if (mesh->m_TriangleVertexIndices.size())
        {
          memcpy( args->m_Buffer + args->m_Offset, &( mesh->m_TriangleVertexIndices.front() ), mesh->m_TriangleVertexIndices.size() * sizeof( u32 ) );
          args->m_Offset += ( (u32)mesh->m_TriangleVertexIndices.size() * sizeof( u32 ) );
        }
      }
      break;
    }

  case ResourceTypes::Vertex:
    {
      if ( args->m_Buffer != NULL )
      {
        m_ObjectBounds.Reset();

        u32 vertexCount = (u32)GetNumberVertices();
        if (vertexCount > 0)
        {
          StandardVertex* vertex = NULL;

          for ( u32 i=0; i<vertexCount; ++i )
          {
            // get address for the current vertex in the resource buffer
            vertex = reinterpret_cast<StandardVertex*>(args->m_Buffer + args->m_Offset) + i;

            // Position, test for local bounds computation
            vertex->m_Position = m_ObjectBounds.Test( mesh->m_Positions[i] );

            // Normal, used for lighting
            //vertex->m_Normal = mesh->m_Normals[i];

            if (m_HasColor)
            {
              // Vertex Color
              HELIUM_ASSERT(0);
              vertex->m_Diffuse = D3DCOLOR_COLORVALUE( mesh->m_Colors[i].x, mesh->m_Colors[i].y, mesh->m_Colors[i].z, mesh->m_Colors[i].w );
            }
            else
            {
              vertex->m_Diffuse = D3DCOLOR_COLORVALUE( 0.5f, 0.5f, 0.5f, 0.5f );
            }

            if (m_HasTexture)
            {
              HELIUM_ASSERT(0);
              // Color Map UV
              vertex->m_BaseUV.x = mesh->m_BaseUVs[i].x;
              vertex->m_BaseUV.y = 1.0f - mesh->m_BaseUVs[i].y;
            }
          }

          args->m_Offset += ( vertexCount * sizeof( StandardVertex ) );

          HELIUM_ASSERT(args->m_Buffer + args->m_Offset == reinterpret_cast<u8*>(++vertex));
        }
      }
      break;
    }
  }
}

void NavMesh::Evaluate( GraphDirection direction )
{
  __super::Evaluate(direction);
}

void NavMesh::SetupNavObject( IDirect3DDevice9* device, const SceneNode* object )
{
  SetupNormalObject( device, object );
}

void NavMesh::SetUpNavMeshRenderEntry(RenderVisitor* render, RenderEntry* entry)
{
  entry->m_ObjectSetup = &NavMesh::SetupNavObject;
  entry->m_ObjectReset = &Mesh::ResetFlippedObject;
 // entry->m_Flags |= RenderFlags::DistanceSort;//forcing it to draw later
  entry->m_Location = render->State().m_Matrix;
  entry->m_Center = m_ObjectBounds.Center();
  entry->m_DrawSetup = &Mesh::SetupNormalWire;
}

void NavMesh::Render( RenderVisitor* render )
{
  this->SetSelected( true );  // makes sure the mesh gets drawn

  //__super::Render( render );
  RenderEntry* entry;

    entry = render->Allocate(this);
    SetUpNavMeshRenderEntry(render, entry);
    entry->m_Flags |= RenderFlags::DistanceSort;
    entry->m_Draw = &NavMesh::DrawSelectedVerts;

    if ( m_SelectionMode == NavMeshCreateTool::MOUSE_HOVER_SELECT_VERT )
    {
      entry = render->Allocate(this);
      SetUpNavMeshRenderEntry(render, entry);
      entry->m_Draw = &NavMesh::DrawMouseOverVert;
      entry->m_Flags |= RenderFlags::DistanceSort;

      //draw the mesh verts only in 
      entry = render->Allocate(this);
      SetUpNavMeshRenderEntry(render, entry);
      entry->m_Draw = &NavMesh::DrawMeshVerts;
    }
    
    entry = render->Allocate(this);
    SetUpNavMeshRenderEntry(render, entry);
    entry->m_Draw = &NavMesh::DrawSelectedEdge;
    entry->m_Flags |= RenderFlags::DistanceSort;
    entry = render->Allocate(this);
    SetUpNavMeshRenderEntry(render, entry);
    entry->m_Draw = &NavMesh::DrawMouseOverEdge;
    entry->m_Flags |= RenderFlags::DistanceSort;
    entry = render->Allocate(this);
    SetUpNavMeshRenderEntry(render, entry);
    entry->m_Draw = &NavMesh::DrawSelectedTri;
    entry->m_Flags |= RenderFlags::DistanceSort;
    entry = render->Allocate(this);
    SetUpNavMeshRenderEntry(render, entry);
    entry->m_Draw = &NavMesh::DrawMouseOverTri;
    entry->m_Flags |= RenderFlags::DistanceSort;

    if ( m_DrawLocator )
    {
      entry = render->Allocate(this);
      SetUpNavMeshRenderEntry(render, entry);
      entry->m_Draw = &NavMesh::DrawLocator;
    }

  //draw the mesh wire frame wire (always)
  entry = render->Allocate(this);
  SetUpNavMeshRenderEntry(render, entry);
  entry->m_Draw = &NavMesh::DrawMeshEdges;

  //draw the mesh tris
  if (render->GetViewport()->GetCamera()->GetShadingMode() !=  ShadingModes::Wireframe)
  {
    entry = render->Allocate(this);
    SetUpNavMeshRenderEntry(render, entry);
    entry->m_Draw = &NavMesh::DrawMeshTris;
  }

  //entry = render->Allocate(this);
  //SetUpNavMeshRenderEntry(render, entry);
  //entry->m_Draw = &NavMesh::DrawToBeDeletedVerts;
  //entry = render->Allocate(this);
  //SetUpNavMeshRenderEntry(render, entry);
  //entry->m_Draw = &NavMesh::DrawToBeDeletedTris;
}

void NavMesh::DrawMeshVerts( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::NavMesh* navMesh = Reflect::ConstAssertCast< Editor::NavMesh > ( node );
  const Content::Mesh* data = navMesh->GetPackage<Content::Mesh>();
  const VertexResource* vertices = navMesh->m_Vertices;
  u32 meshPoints    = (u32) data->GetVertexCount();
  if ( !meshPoints || !vertices->SetState() )
  {
    return;
  }
  //device->SetMaterial( &navMesh->s_Material );
  if (data->m_MeshOriginType == Content::Mesh::NavHiRes)
  {
    device->SetMaterial( &NavMesh::s_HiResVertDisplayMaterial );
  }
  else
  {
    device->SetMaterial( &NavMesh::s_LowResVertDisplayMaterial );
  }
  static float pointSize = 6.0f;
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );  
  device->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
  device->SetRenderState( D3DRS_POINTSIZE, *( (DWORD*) &pointSize ) );
  device->DrawPrimitive( D3DPT_POINTLIST, (u32)vertices->GetBaseIndex(), meshPoints );
  device->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
  device->SetMaterial( &Editor::Viewport::s_ReactiveMaterial );
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
}


void NavMesh::DrawMeshEdges( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::NavMesh* navMesh = Reflect::ConstAssertCast< Editor::NavMesh > ( node );
  const Content::Mesh* data = navMesh->GetPackage<Content::Mesh>();
  const VertexResource* vertices = navMesh->m_Vertices;
  const IndexResource* indices = navMesh->m_Indices;
  u32 meshPoints    = (u32) data->GetVertexCount();
  if ( !meshPoints || !vertices->SetState() || !indices->SetState())
  {
    return;
  }
  //device->SetMaterial( &navMesh->s_Material );
    if (data->m_MeshOriginType == Content::Mesh::NavHiRes)
    {
      device->SetMaterial( &NavMesh::s_HiResEdgeDisplayMaterial );
    }
    else
    {
      device->SetMaterial( &NavMesh::s_LowResEdgeDisplayMaterial );
    }
  
  device->DrawIndexedPrimitive( D3DPT_LINELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), indices->GetBaseIndex(), navMesh->m_LineCount );
  args->m_LineCount += navMesh->m_LineCount;
}
void NavMesh::DrawMeshTris( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::NavMesh* navMesh = Reflect::ConstAssertCast< Editor::NavMesh > ( node );
  const Content::Mesh* data = navMesh->GetPackage<Content::Mesh>();
  const VertexResource* vertices = navMesh->m_Vertices;
  const IndexResource* indices = navMesh->m_Indices;
  u32 meshPoints    = (u32) data->GetVertexCount();
  if ( !meshPoints || !vertices->SetState() || !indices->SetState())
  {
    return;
  }
  //device->SetMaterial( &navMesh->s_Material );
 
  //if ( navMesh->GetScene()->GetManager()->GetCurrentScene()->GetTool() || navMesh->GetScene()->GetManager()->GetCurrentScene()->GetTool()->GetType() != Reflect::GetType<Editor::NavMeshCreateTool>())
    if (data->m_MeshOriginType == Content::Mesh::NavHiRes)
    {
      device->SetMaterial( &NavMesh::s_HiResTriDisplayMaterial );
    }
    else
    {
      device->SetMaterial( &NavMesh::s_LowResTriDisplayMaterial );
    }
  
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
  device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
  device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
  device->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
  device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), (UINT)( indices->GetBaseIndex() + data->m_WireframeVertexIndices.size() ), (UINT)( navMesh->m_TriangleCount ) );
  args->m_TriangleCount += navMesh->m_TriangleCount;
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
  device->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
  device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}


void NavMesh::DrawMouseOverVert( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::NavMesh* navMesh = Reflect::ConstAssertCast< Editor::NavMesh > ( node );
  const Content::Mesh* data = navMesh->GetPackage<Content::Mesh>();
  const VertexResource* vertices = navMesh->m_Vertices;
  u32 meshPoints    = (u32) data->GetVertexCount();
  if ( !meshPoints || !vertices->SetState() )
  {
    return;
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );  
  device->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
  if (navMesh->m_mouse_over_vert != 0xFFFFFFFF)
  {
    static float high_light_pointSize = 10.0f;
    device->SetRenderState( D3DRS_POINTSIZE, *( (DWORD*) &high_light_pointSize ) );
    device->SetMaterial( &Editor::Viewport::s_ReactiveMaterial );
    device->DrawPrimitive( D3DPT_POINTLIST, (u32)vertices->GetBaseIndex() + navMesh->m_mouse_over_vert, 1 );
  }
  device->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
}

void NavMesh::DrawLocator( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::NavMesh* navMesh = Reflect::ConstAssertCast< Editor::NavMesh > ( node );
  const Math::Matrix4& globalTransform = navMesh->GetGlobalTransform();
  Math::Matrix4 m;
  m = Math::Matrix4( navMesh->m_LocatorPos ) * globalTransform;
  device->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&m );
  navMesh->m_Locator->Draw( args );
  device->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&globalTransform );
}

void NavMesh::DrawSelectedVerts( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::NavMesh* navMesh = Reflect::ConstAssertCast< Editor::NavMesh > ( node );
  const Content::Mesh* data = navMesh->GetPackage<Content::Mesh>();
  const VertexResource* vertices = navMesh->m_Vertices;
  u32 meshPoints    = (u32) data->GetVertexCount();
  if ( !meshPoints || !vertices->SetState() )
  {
    return;
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );  
  device->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
  device->SetRenderState(D3DRS_ZENABLE, FALSE);
  static float select_top_two_pointSize = 8.0f;
  device->SetRenderState( D3DRS_POINTSIZE, *( (DWORD*) &select_top_two_pointSize ) );
  if (navMesh->m_selected_verts.size())
  {
    device->SetMaterial( &Editor::Viewport::s_RedMaterial );
    device->DrawPrimitive( D3DPT_POINTLIST, (u32)vertices->GetBaseIndex() + navMesh->m_selected_verts[0], 1 );
    if (navMesh->m_selected_verts.size()>1)
    {
      device->DrawPrimitive( D3DPT_POINTLIST, (u32)vertices->GetBaseIndex() + navMesh->m_selected_verts[1], 1 );
    }
  }

  static float select_pointSize = 6.0f;
  device->SetRenderState( D3DRS_POINTSIZE, *( (DWORD*) &select_pointSize ) );
  device->SetMaterial( &Editor::Viewport::s_SelectedComponentMaterial );
  for (std::vector< u32 >::const_iterator it = navMesh->m_selected_verts.begin(); it != navMesh->m_selected_verts.end(); ++it)
  {
    device->DrawPrimitive( D3DPT_POINTLIST, (u32)vertices->GetBaseIndex() + *it, 1 );
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
  device->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );


  //TEMP RENDERING TO DISPLAY MAQRQUEE VERTS
  static float marquee_pointSize = 7.0f;
  if (navMesh->m_marquee_selected_verts.size())
  {
    device->SetMaterial( &Editor::Viewport::s_SelectedComponentMaterial );
    device->SetRenderState( D3DRS_POINTSIZE, *( (DWORD*) &marquee_pointSize ) );
    for (std::vector< u32 >::const_iterator it = navMesh->m_marquee_selected_verts.begin(); it != navMesh->m_marquee_selected_verts.end(); ++it)
    {
      device->DrawPrimitive( D3DPT_POINTLIST, (u32)vertices->GetBaseIndex() + *it, 1 );
    }
  }
  device->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void NavMesh::DrawToBeDeletedVerts( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::NavMesh* navMesh = Reflect::ConstAssertCast< Editor::NavMesh > ( node );
  const Content::Mesh* data = navMesh->GetPackage<Content::Mesh>();
  const VertexResource* vertices = navMesh->m_Vertices;
  u32 meshPoints    = (u32) data->GetVertexCount();
  if ( !meshPoints || !vertices->SetState() )
  {
    return;
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );  
  device->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );

  static float delete_pointSize = 8.0f;
  device->SetRenderState( D3DRS_POINTSIZE, *( (DWORD*) &delete_pointSize ) );
  device->SetMaterial( &Editor::Viewport::s_RedMaterial );
  for (std::vector< u32 >::const_iterator it = navMesh->m_to_be_deleted_verts.begin(); it != navMesh->m_to_be_deleted_verts.end(); ++it)
  {
    device->DrawPrimitive( D3DPT_POINTLIST, (u32)vertices->GetBaseIndex() + *it, 1 );
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
  device->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
}

void NavMesh::DrawMouseOverEdge( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::NavMesh* navMesh = Reflect::ConstAssertCast< Editor::NavMesh > ( node );
  const Content::Mesh* data = navMesh->GetPackage<Content::Mesh>();
  const VertexResource* vertices = navMesh->m_Vertices;
  //Editor::Viewport* view = node->GetScene()->GetViewport();
  //Editor::Camera* camera = view->GetCamera();
  u32 meshPoints    = (u32) data->GetVertexCount();
  navMesh->SetMaterial( navMesh->s_Material );
  if ( !meshPoints || !vertices->SetState() )
  {
    return;
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );  
  if ( !navMesh->m_Indices->SetState() )
  {
    return;
  }
  if (navMesh->m_mouse_over_edge != 0xFFFFFFFF)
  {
    device->SetMaterial( &Editor::Viewport::s_ReactiveMaterial );
    device->DrawIndexedPrimitive(D3DPT_LINELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), navMesh->m_Indices->GetBaseIndex() + navMesh->m_mouse_over_edge*2, 1 );
    args->m_LineCount += 1;
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
}

void NavMesh::DrawSelectedEdge( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::NavMesh* navMesh = Reflect::ConstAssertCast< Editor::NavMesh > ( node );
  const Content::Mesh* data = navMesh->GetPackage<Content::Mesh>();
  const VertexResource* vertices = navMesh->m_Vertices;
  //Editor::Viewport* view = node->GetScene()->GetViewport();
  //Editor::Camera* camera = view->GetCamera();
  u32 meshPoints    = (u32) data->GetVertexCount();
  if ( !meshPoints || !vertices->SetState() )
  {
    return;
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );  
  if ( !navMesh->m_Indices->SetState() )
  {
    return;
  }
  device->SetRenderState(D3DRS_ZENABLE, FALSE);
  if (navMesh->m_selected_edge != 0xFFFFFFFF)
  {
    device->SetMaterial( &Editor::Viewport::s_SelectedComponentMaterial );
    device->DrawIndexedPrimitive(D3DPT_LINELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), navMesh->m_Indices->GetBaseIndex() + navMesh->m_selected_edge*2, 1 );
    args->m_LineCount += 1;
  }
  //TEMP RENDERING TO DISPLAY MAQRQUEE TRIS
  if (navMesh->m_marquee_selected_edges.size())
  {
    device->SetMaterial( &Editor::Viewport::s_SelectedComponentMaterial );
    for (std::vector< u32 >::const_iterator it = navMesh->m_marquee_selected_edges.begin(); it != navMesh->m_marquee_selected_edges.end(); ++it)
    {
      device->DrawIndexedPrimitive(D3DPT_LINELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), navMesh->m_Indices->GetBaseIndex() + *(it)*2, 1 );
    }
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
  device->SetRenderState(D3DRS_ZENABLE, TRUE);
}


void NavMesh::DrawMouseOverTri( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::NavMesh* navMesh = Reflect::ConstAssertCast< Editor::NavMesh > ( node );
  const Content::Mesh* data = navMesh->GetPackage<Content::Mesh>();
  const VertexResource* vertices = navMesh->m_Vertices;
  //Editor::Viewport* view = node->GetScene()->GetViewport();
  //Editor::Camera* camera = view->GetCamera();
  u32 meshPoints    = (u32) data->GetVertexCount();
  if ( !meshPoints || !vertices->SetState() )
  {
    return;
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );  
  if ( !navMesh->m_Indices->SetState() )
  {
    return;
  }
  device->SetRenderState(D3DRS_ZENABLE, FALSE);
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
  device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
  device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
  device->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
  

  if (navMesh->m_mouse_over_tri != 0xFFFFFFFF)
  {
    device->SetMaterial(&s_MouseOverTriMaterial );
    device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), navMesh->m_Indices->GetBaseIndex() + (u32)data->m_WireframeVertexIndices.size() + navMesh->m_mouse_over_tri*3, 1 );
    args->m_TriangleCount += 1;
  }
  device->SetRenderState(D3DRS_ZENABLE, TRUE);
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
  device->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
  device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  

}
void NavMesh::DrawSelectedTri( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::NavMesh* navMesh = Reflect::ConstAssertCast< Editor::NavMesh > ( node );
  const Content::Mesh* data = navMesh->GetPackage<Content::Mesh>();
  const VertexResource* vertices = navMesh->m_Vertices;
  //Editor::Viewport* view = node->GetScene()->GetViewport();
  //Editor::Camera* camera = view->GetCamera();
  u32 meshPoints    = (u32) data->GetVertexCount();
  if ( !meshPoints || !vertices->SetState() )
  {
    return;
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );  
  if ( !navMesh->m_Indices->SetState() )
  {
    return;
  }
   device->SetRenderState(D3DRS_ZENABLE, FALSE);
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
  device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  device->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
  device->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
  device->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);
  if ( !navMesh->m_selected_tris.empty() )
  {
    for ( u32 i = 0; i < navMesh->m_selected_tris.size(); i++ )
    {
      device->SetMaterial( &s_SelectedTriMaterial );
      device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), navMesh->m_Indices->GetBaseIndex() + (u32)data->m_WireframeVertexIndices.size() + navMesh->m_selected_tris.at(i)*3, 1 );
      args->m_TriangleCount += 1;
    }
  }

  //TEMP RENDERING TO DISPLAY MAQRQUEE TRIS
  if (navMesh->m_marquee_selected_tris.size())
  {
    device->SetMaterial( &Editor::Viewport::s_SelectedComponentMaterial );
    for (std::vector< u32 >::const_iterator it = navMesh->m_marquee_selected_tris.begin(); it != navMesh->m_marquee_selected_tris.end(); ++it)
    {
      device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), navMesh->m_Indices->GetBaseIndex() + (u32)data->m_WireframeVertexIndices.size() + (*it)*3, 1 );
    }
  }

   device->SetRenderState(D3DRS_ZENABLE, TRUE);
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
  device->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
  device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void NavMesh::DrawToBeDeletedTris( IDirect3DDevice9* device, DrawArgs* args, const SceneNode*object )
{
  const Editor::HierarchyNode* node = Reflect::ConstAssertCast<Editor::HierarchyNode>( object );
  const Editor::NavMesh* navMesh = Reflect::ConstAssertCast< Editor::NavMesh > ( node );
  const Content::Mesh* data = navMesh->GetPackage<Content::Mesh>();
  const VertexResource* vertices = navMesh->m_Vertices;
  //Editor::Viewport* view = node->GetScene()->GetViewport();
  //Editor::Camera* camera = view->GetCamera();
  u32 meshPoints    = (u32) data->GetVertexCount();
  if ( !meshPoints || !vertices->SetState() )
  {
    return;
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );  
  if ( !navMesh->m_Indices->SetState() )
  {
    return;
  }
  for (std::vector< u32 >::const_iterator it = navMesh->m_to_be_deleted_tris.begin(); it != navMesh->m_to_be_deleted_tris.end(); ++it)
  {
    device->SetMaterial( &Editor::Viewport::s_RedMaterial );
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), navMesh->m_Indices->GetBaseIndex() + (u32)data->m_WireframeVertexIndices.size() + (*it)*3, 1 );
    args->m_TriangleCount += 1;
  }
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
  device->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
  device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

bool NavMesh::Pick( PickVisitor* pick )
{  
  return __super::Pick( pick );
}

bool NavMesh::ValidatePanel( const tstring& name )
{
  if ( name == TXT( "NavMesh" ) )
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void NavMesh::CreatePanel( CreatePanelArgs& args )
{
  /*args.m_Generator->PushPanel( "NavMesh", true);
  {
    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( "Type" );
      args.m_Generator->AddChoice<Editor::Curve, int>( args.m_Selection, Reflect::Registry::GetInstance()->GetEnumeration( "CurveType" ), &Curve::GetCurveType, &Curve::SetCurveType );
    }
    args.m_Generator->Pop();

    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( "Control Point Label" );
      args.m_Generator->AddChoice<Editor::Curve, int>( args.m_Selection, Reflect::Registry::GetInstance()->GetEnumeration( "ControlPointLabel" ), &Curve::GetControlPointLabel, &Curve::SetControlPointLabel );
    }
    args.m_Generator->Pop();

    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( "Resolution" );
      Inspect::Slider* slider = args.m_Generator->AddSlider<Editor::Curve, u32>( args.m_Selection, &Curve::GetResolution, &Curve::SetResolution );
      slider->SetRangeMin( 1.0f, false );
      slider->SetRangeMax( 20.0f, false );
    }
    args.m_Generator->Pop();

    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( "Closed" );
      args.m_Generator->AddCheckBox<Editor::Curve, bool>( args.m_Selection, &Curve::GetClosed, &Curve::SetClosed );
    }
    args.m_Generator->Pop();

    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( "Reverse Control Points" );
      Inspect::Action* button = args.m_Generator->AddAction( Inspect::ActionSignature::Delegate( &Curve::OnReverseControlPoints ) );
      button->SetIcon( "reverse" );
      button->SetClientData( new SelectionDataObject( args.m_Selection ) );
    }
    args.m_Generator->Pop();

    args.m_Generator->PushContainer();
    {
      args.m_Generator->AddLabel( "Curve Length" );

      typedef f32 ( Curve::*Getter )() const;
      typedef void ( Curve::*Setter )( const f32& );
      Inspect::Value* textBox = args.m_Generator->AddValue< Editor::Curve, f32, Getter, Setter >( args.m_Selection, &Curve::CalculateCurveLength );
      textBox->SetReadOnly( true );
    }
    args.m_Generator->Pop();
  }
  args.m_Generator->Pop();*/
}

void NavMesh::AddVert( Math::Vector3 position )
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  m_mouse_over_vert = (u32)navMesh->m_Positions.size();
  navMesh->m_Positions.push_back(position);
  m_VertexCount = (u32)navMesh->m_Positions.size();
  m_Vertices->SetElementCount( (u32)navMesh->m_Positions.size() );
  m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Vertices->Update();
  ToggleMouseOverVertSelection();
  if (m_selected_verts.size() > 2)
  {
    m_selected_edge = 0xFFFFFFFF;
    AddTri();
  }
  // when we're adding verts just keep 3 verts of the current tri selected
  while ( m_selected_verts.size() > 3)
  {
    m_selected_verts.erase( m_selected_verts.end()-1 );
  }
}

void NavMesh::AddTri()
{
  if (m_selected_verts.size() < 3)
  {
    return;
  }
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  assert(m_selected_verts[0] < navMesh->m_Positions.size());
  assert(m_selected_verts[1] < navMesh->m_Positions.size());
  assert(m_selected_verts[2] < navMesh->m_Positions.size());
  m_mouse_over_tri = (u32)navMesh->m_TriangleVertexIndices.size()/3;
  m_selected_tris.clear();
  m_selected_tris.push_back( m_mouse_over_tri );
  navMesh->AddTri(m_selected_verts[0], m_selected_verts[1], m_selected_verts[2]);
  ChangeTriEdgeVertSelection();
  m_LineCount = (u32)navMesh->m_WireframeVertexIndices.size()/2;
  m_TriangleCount = (u32)navMesh->m_TriangleVertexIndices.size()/3;
  m_Indices->SetElementCount( (u32)navMesh->m_WireframeVertexIndices.size()  + (u32)navMesh->m_TriangleVertexIndices.size() );
  m_Indices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Indices->Update();
}

void NavMesh::UpdateVert( u32 index, Math::Vector3 position )
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  navMesh->m_Positions[index] = position;
  Dirty();
  m_Vertices->Update();
}

void NavMesh::UpdateMouseOverVert(const Math::Vector3& mouse_ss_start, const f32 threshold_rad, const Math::Vector3& mouse_ss_dir, const f32 ss_len)
{
  m_mouse_over_tri = m_mouse_over_edge = m_mouse_over_vert = 0xFFFFFFFF;
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  if (navMesh)
  {
    m_mouse_over_vert = navMesh->GetClosestVert(mouse_ss_start, threshold_rad, mouse_ss_dir, ss_len);
  }
}

void NavMesh::UpdateMouseOverVert(const Math::Matrix4& view_proj_mat, const f32 porj_space_threshold_sqr, Math::Vector2 proj_pt)
{
  m_mouse_over_tri = m_mouse_over_edge = m_mouse_over_vert = 0xFFFFFFFF;
   Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  if (navMesh)
  {
    m_mouse_over_vert = navMesh->GetClosestVert(view_proj_mat, porj_space_threshold_sqr, proj_pt);
  }

}

void  NavMesh::UpdateMouseOverEdge(const Math::Vector3& mouse_ss_start, const f32 threshold_rad, const Math::Vector3& mouse_ss_dir, const f32 ss_len)
{
    m_mouse_over_tri = m_mouse_over_edge = m_mouse_over_vert = 0xFFFFFFFF;
  m_mouse_over_tri = m_mouse_over_edge = m_mouse_over_vert = 0xFFFFFFFF;
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  if (navMesh)
  {
    m_mouse_over_edge = navMesh->GetClosestEdge(mouse_ss_start, threshold_rad, mouse_ss_dir, ss_len);
  }
}

void  NavMesh::UpdateMouseOverTri(const Math::Vector3& mouse_ss_start, const f32 threshold_rad, const Math::Vector3& mouse_ss_dir, const f32 ss_len)
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  m_mouse_over_tri = navMesh->GetClosestTri(mouse_ss_start, threshold_rad, mouse_ss_dir, ss_len);
}

// returns false if the vert was toggled to not-selected, true if it is now selected
bool NavMesh::ToggleMouseOverVertSelection()
{
  if (m_mouse_over_vert != 0xFFFFFFFF)
  {
    std::vector< u32 >::iterator it = std::find(m_selected_verts.begin(), m_selected_verts.end(), m_mouse_over_vert);
    if (it != m_selected_verts.end())
    {
      m_selected_verts.erase(it);
      return false;
    }
    else
    {
      m_selected_verts.insert(m_selected_verts.begin(), m_mouse_over_vert);

      if (m_selected_verts.size() > 1)
      {
        Content::Mesh* navMesh = GetPackage<Content::Mesh>();
        m_selected_edge = navMesh->GetEdgeIdForVerts(m_selected_verts[0], m_selected_verts[1]);
      }
      return true;
    }
  }
  return false;
}

void NavMesh::AddMouseOverVertToDeleteQueue()
{
  if (m_mouse_over_vert != 0xFFFFFFFF)
  {
    std::vector< u32 >::iterator it = std::find(m_to_be_deleted_verts.begin(), m_to_be_deleted_verts.end(), m_mouse_over_vert);
    if (it != m_to_be_deleted_verts.end())
    {
       m_to_be_deleted_verts.erase(it);
      return;
    }
    m_to_be_deleted_verts.insert(m_to_be_deleted_verts.begin(), m_mouse_over_vert);
  }
}

void NavMesh::AddMouseOverTriToDeleteQueue()
{
  if (m_mouse_over_tri != 0xFFFFFFFF)
  {
    std::vector< u32 >::iterator it = std::find(m_to_be_deleted_tris.begin(), m_to_be_deleted_tris.end(), m_mouse_over_tri);
    if (it != m_to_be_deleted_tris.end())
    {
      m_to_be_deleted_tris.erase(it);
      return;
    }
    m_to_be_deleted_tris.insert(m_to_be_deleted_tris.begin(), m_mouse_over_tri);
  }
}

bool NavMesh::ToggleMouseOverEdgeSelection()
{
  if (m_mouse_over_edge != 0xFFFFFFFF)
  {
    Content::Mesh* navMesh = GetPackage<Content::Mesh>();
    u32 edge_verts[2];
    edge_verts[0] = navMesh->m_WireframeVertexIndices[m_mouse_over_edge*2];
    edge_verts[1] = navMesh->m_WireframeVertexIndices[m_mouse_over_edge*2 + 1];
    std::vector< u32 >::iterator it = std::find(m_selected_verts.begin(), m_selected_verts.end(), edge_verts[0]);
    if (it != m_selected_verts.end())
    {
      m_selected_verts.erase(it);
    }
    it = std::find(m_selected_verts.begin(), m_selected_verts.end(), edge_verts[1]);
    if (it != m_selected_verts.end())
    {
      m_selected_verts.erase(it);
    }

    if ( m_selected_edge == m_mouse_over_edge )
    {
      // we are deselecting the edge
      ClearEditingState();
      return false;
    }

    m_selected_verts.insert(m_selected_verts.begin(), edge_verts[0]);
    m_selected_verts.insert(m_selected_verts.begin(), edge_verts[1]);
    m_selected_edge = m_mouse_over_edge;
    return true;
  }
  return false;
}

bool NavMesh::ToggleMouseOverTriSelection()
{
  if (m_mouse_over_tri != 0xFFFFFFFF)
  {
    std::vector< u32 >::iterator it = std::find(m_selected_tris.begin(), m_selected_tris.end(), m_mouse_over_tri);
    if (it != m_selected_tris.end())
    {
      m_selected_verts.clear();
      m_selected_tris.erase(it);
      ChangeTriEdgeVertSelection();
      return false;
    }
    
    Content::Mesh* navMesh = GetPackage<Content::Mesh>();
    m_selected_tris.push_back( m_mouse_over_tri );
    ChangeTriEdgeVertSelection();
    return true;
  }
  return false;
}

void NavMesh::ChangeTriEdgeVertSelection()
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  if ( m_selected_tris.empty() )
  {
    ClearEditingState();
  }
  else
  {
    for ( u32 i = 0; i < m_selected_tris.size(); i++ )
    {
      u32 tri_verts[3];
      tri_verts[0] = navMesh->m_TriangleVertexIndices[m_selected_tris.at(i)*3];
      tri_verts[1] = navMesh->m_TriangleVertexIndices[m_selected_tris.at(i)*3+1];
      tri_verts[2] = navMesh->m_TriangleVertexIndices[m_selected_tris.at(i)*3+2];
      bool flush_selected_verts = false;
      std::vector< u32 >::iterator it = std::find(m_cached_selected_tris.begin(), m_cached_selected_tris.end(), m_selected_tris.at(i) );
      if (it != m_cached_selected_tris.end() && m_selected_verts.size() > 2)
      {
        for (u32 k=0; k<3; ++k)
        {
          if (m_selected_verts[k] != tri_verts[0] &&  m_selected_verts[k] != tri_verts[1] && m_selected_verts[k] != tri_verts[2])
          {
            flush_selected_verts = true;
            break;
          }
        }
        if (!flush_selected_verts)
        {
          u32 tri_edge_id = 0xFFFFFFFF;
          for (u32 i=0; i<3; ++i)
          {
            if ((m_selected_verts[0] == tri_verts[i] && m_selected_verts[1] == tri_verts[(i+1)%3]) ||
              (m_selected_verts[0] == tri_verts[(i+1)%3] && m_selected_verts[1] == tri_verts[i]) )
            {
              tri_edge_id = i;
              break;
            }
          }
          assert(tri_edge_id != 0xFFFFFFFF);
          tri_edge_id = (tri_edge_id+1)%3;
          m_selected_verts[2] = tri_verts[(tri_edge_id+2)%3];
          m_selected_verts[1] = tri_verts[(tri_edge_id+1)%3];
          m_selected_verts[0] = tri_verts[(tri_edge_id)%3];
        }
      }
      else
      {
        flush_selected_verts = true;
      }
      if (flush_selected_verts)
      {
        std::vector< u32 >::iterator it = std::find(m_selected_verts.begin(), m_selected_verts.end(), tri_verts[0]);
        if (it != m_selected_verts.end())
        {
          m_selected_verts.erase(it);
        }
        it = std::find(m_selected_verts.begin(), m_selected_verts.end(), tri_verts[1]);
        if (it != m_selected_verts.end())
        {
          m_selected_verts.erase(it);
        }
        it = std::find(m_selected_verts.begin(), m_selected_verts.end(), tri_verts[2]);
        if (it != m_selected_verts.end())
        {
          m_selected_verts.erase(it);
        }
        m_selected_verts.insert(m_selected_verts.begin(), tri_verts[2]);
        m_selected_verts.insert(m_selected_verts.begin(), tri_verts[1]);
        m_selected_verts.insert(m_selected_verts.begin(), tri_verts[0]);
        m_cached_selected_tris = m_selected_tris;
      }
      //m_selected_edge = navMesh->GetEdgeIdForVerts(m_selected_verts[0], m_selected_verts[1]); // Why are we selecting an edge here? It means we draw a selected tri with one selected edge
    }
  }
}

void NavMesh::DeleteVertsInQueue()
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  //call delete verts on content mesh
  navMesh->DeleteVerts(m_selected_verts);
  ClearEditingState();
  m_VertexCount = (u32)navMesh->m_Positions.size();
  m_Vertices->SetElementCount( (u32)navMesh->m_Positions.size() );
  m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Vertices->Update();
  m_LineCount = (u32)navMesh->m_WireframeVertexIndices.size()/2;
  m_TriangleCount = (u32)navMesh->m_TriangleVertexIndices.size()/3;
  m_Indices->SetElementCount( (u32)navMesh->m_WireframeVertexIndices.size()  + (u32)navMesh->m_TriangleVertexIndices.size() );
  m_Indices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Indices->Update();
}

void NavMesh::DeleteTrisInQueue()
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  //call delete on tris on content mesh
  navMesh->DeleteTris(m_to_be_deleted_tris);
  ClearEditingState();
  m_VertexCount = (u32)navMesh->m_Positions.size();
  m_Vertices->SetElementCount( (u32)navMesh->m_Positions.size() );
  m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Vertices->Update();
  m_LineCount = (u32)navMesh->m_WireframeVertexIndices.size()/2;
  m_TriangleCount = (u32)navMesh->m_TriangleVertexIndices.size()/3;
  m_Indices->SetElementCount( (u32)navMesh->m_WireframeVertexIndices.size()  + (u32)navMesh->m_TriangleVertexIndices.size() );
  m_Indices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Indices->Update();
}

void NavMesh::DeleteEdgesInQueue()
{
  // right now we aren't really using the vector for selected edges but i'm going to use it here so it's easier if/when we want to use it everywhere
  m_marquee_selected_edges.clear();
  m_marquee_selected_edges.push_back( m_selected_edge );

  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  navMesh->DeleteEdges( m_marquee_selected_edges );
  ClearEditingState();
  m_VertexCount = (u32)navMesh->m_Positions.size();
  m_Vertices->SetElementCount( (u32)navMesh->m_Positions.size() );
  m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Vertices->Update();
  m_LineCount = (u32)navMesh->m_WireframeVertexIndices.size()/2;
  m_TriangleCount = (u32)navMesh->m_TriangleVertexIndices.size()/3;
  m_Indices->SetElementCount( (u32)navMesh->m_WireframeVertexIndices.size()  + (u32)navMesh->m_TriangleVertexIndices.size() );
  m_Indices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Indices->Update();
}

void NavMesh::MergeVertToClosest(u32 vert_to_be_merged)
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  navMesh->MergeVertToClosest(vert_to_be_merged);
  m_VertexCount = (u32)navMesh->m_Positions.size();
  m_Vertices->SetElementCount( (u32)navMesh->m_Positions.size() );
  m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Vertices->Update();
  m_LineCount = (u32)navMesh->m_WireframeVertexIndices.size()/2;
  m_TriangleCount = (u32)navMesh->m_TriangleVertexIndices.size()/3;
  m_Indices->SetElementCount( (u32)navMesh->m_WireframeVertexIndices.size()  + (u32)navMesh->m_TriangleVertexIndices.size() );
  m_Indices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Indices->Update();
}

void NavMesh::PunchCubeHole(Math::Matrix4& mat, Math::Matrix4& inv_mat, f32 vert_merge_threshold)
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  navMesh->PunchCubeHole(mat, inv_mat, vert_merge_threshold);
  m_VertexCount = (u32)navMesh->m_Positions.size();
  m_Vertices->SetElementCount( (u32)navMesh->m_Positions.size() );
  m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Vertices->Update();
  m_LineCount = (u32)navMesh->m_WireframeVertexIndices.size()/2;
  m_TriangleCount = (u32)navMesh->m_TriangleVertexIndices.size()/3;
  m_Indices->SetElementCount( (u32)navMesh->m_WireframeVertexIndices.size()  + (u32)navMesh->m_TriangleVertexIndices.size() );
  m_Indices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Indices->Update();
}


void NavMesh::MoveSelectedVertsByOffset( const Math::Vector3& offset )
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();

  for( size_t i = 0; i < m_selected_verts.size(); ++i )
  {
    navMesh->m_Positions.at(m_selected_verts.at(i)) += offset;
  }
  m_Vertices->Update();
}

void NavMesh::MoveSelectedEdgeByOffset( const Math::Vector3& offset )
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();

  if ( m_selected_verts.size() >= 2 )
  {
    for( size_t i = 0; i < m_selected_verts.size(); ++i )
    {
      navMesh->m_Positions.at(m_selected_verts.at(i)) += offset;
    }
  }
  m_Vertices->Update();
}

void NavMesh::MoveSelectedTriByOffset( const Math::Vector3& offset )
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();

  if ( m_selected_verts.size() >= 3 )
  {
    for( size_t i = 0; i < m_selected_verts.size(); ++i )
    {
      navMesh->m_Positions.at(m_selected_verts.at(i)) += offset;
    }
  }
  m_Vertices->Update();
}

Math::Vector3 NavMesh::GetMouseOverVertPosition()
{
  if ( m_mouse_over_vert != 0xFFFFFFFF )
  {
    std::vector< u32 >::iterator it = std::find(m_selected_verts.begin(), m_selected_verts.end(), m_mouse_over_vert);
    if (it != m_selected_verts.end())
    {
      Content::Mesh* navMesh = GetPackage<Content::Mesh>();
      return navMesh->m_Positions.at(m_mouse_over_vert);
    }
  }
  return Math::Vector3::Zero;
}

const Math::Vector3& NavMesh::GetVertPosition( u32 index )
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  if ( index >= navMesh->m_Positions.size() )
  {
    return Math::Vector3::Zero;
  }
  else
  {
    return navMesh->m_Positions.at(index);
  }
}

void NavMesh::SelectNearestEdge( const Math::Vector3& position )
{
  if ( m_selected_verts.size() != 3 )
  {
    return;
  }

  u32 tri_verts[3];

  u32 cached_selected_tri = 0xFFFFFFFF;
  if ( !m_selected_tris.empty() )
  {
    cached_selected_tri = m_selected_tris.back();
  }

  tri_verts[0] = m_selected_verts.at(0);
  tri_verts[1] = m_selected_verts.at(1);
  tri_verts[2] = m_selected_verts.at(2);

  ClearEditingState();

  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  Math::Vector3 p0 = navMesh->m_Positions.at( tri_verts[0] );
  Math::Vector3 p1 = navMesh->m_Positions.at( tri_verts[1] );
  Math::Vector3 p2 = navMesh->m_Positions.at( tri_verts[2] );

  // Get the edges of the triangle
  Math::Vector3 e0 = p1 - p0;
  Math::Vector3 e1 = p2 - p1;
  Math::Vector3 e2 = p0 - p2;

  // Get the normal of the triangle
  Math::Vector3 n = e0.Cross(e1);

  // compute the edge normals (going into the triangle)
  Math::Vector3 n0 = n.Cross(e0);
  Math::Vector3 n1 = n.Cross(e1);
  Math::Vector3 n2 = n.Cross(e2);

  n0.Normalize();
  n1.Normalize();
  n2.Normalize();

  // get the distance to each edge
  f32 d0 = n0.Dot(position - p0);
  f32 d1 = n1.Dot(position - p1);
  f32 d2 = n2.Dot(position - p2);
  
  if ( d0 <= d1 && d0 < d2 )    // e0 is the closest edge
  {
      m_selected_verts.push_back( tri_verts[0] );
      m_selected_verts.push_back( tri_verts[1] );
      m_selected_verts.push_back( tri_verts[2] );
  }
  else if ( d1 < d2 && d1 < d0 ) // e1 is the closest
  {
    m_selected_verts.push_back( tri_verts[1] );
    m_selected_verts.push_back( tri_verts[2] );
    m_selected_verts.push_back( tri_verts[0] );
  }
  else  // e2 is the closest
  {
    m_selected_verts.push_back( tri_verts[0] );
    m_selected_verts.push_back( tri_verts[2] );
    m_selected_verts.push_back( tri_verts[1] );
  }
  if ( cached_selected_tri != 0xFFFFFFFF )
  {
    m_selected_tris.push_back( cached_selected_tri );
  }
  
  m_selected_edge = navMesh->GetEdgeIdForVerts(m_selected_verts[0], m_selected_verts[1]);
}

void NavMesh::ClearSelectionState()
{
  m_selected_tris.clear();
  m_selected_verts.clear();
  m_selected_edge = 0xFFFFFFFF;
  m_cached_selected_tris.clear();
  m_marquee_selected_edges.clear();

}

void NavMesh::ClearEditingState()
{
  ClearSelectionState();
  m_mouse_over_vert = 0xFFFFFFFF;
  m_mouse_over_edge = 0xFFFFFFFF;
  m_mouse_over_tri = 0xFFFFFFFF;
  m_to_be_deleted_tris.clear();
  m_to_be_deleted_verts.clear();
}

void NavMesh::DeleteVert( const Math::Vector3& position )
{
  //ClearEditingState();
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();

  std::vector< u32 > temp_delete_queue;
  if ( !m_selected_verts.empty() )
  {
    temp_delete_queue = m_selected_verts;
    m_selected_verts.clear();
  }

  u32 pos = 0;
  Math::V_Vector3::iterator it;
  for ( it = navMesh->m_Positions.begin(); it != navMesh->m_Positions.end(); ++it, ++pos )
  {
    if ( *it == position )
    {
      m_selected_verts.push_back(pos);
      DeleteVertsInQueue();
      break;
    }
  }
  m_selected_verts = temp_delete_queue;
}

void NavMesh::SetSelectionMode( u32 mode ) 
{ 
  m_SelectionMode = mode; 
}

void NavMesh::GetMeshData( Math::V_Vector3& positions, std::vector< u32 >& triVerts, std::vector< u32 >& wireVerts )
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  positions = navMesh->m_Positions;
  triVerts = navMesh->m_TriangleVertexIndices;
  wireVerts = navMesh->m_WireframeVertexIndices;
}

void NavMesh::UpdateMeshData( const Math::V_Vector3& positions, const std::vector< u32 >& triVerts, const std::vector< u32 >& wireVerts )
{
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  navMesh->m_Positions = positions;
  navMesh->m_TriangleVertexIndices = triVerts;
  navMesh->m_WireframeVertexIndices = wireVerts;

  m_VertexCount = (u32)navMesh->m_Positions.size();
  m_Vertices->SetElementCount( (u32)navMesh->m_Positions.size() );
  m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Vertices->Update();
  m_LineCount = (u32)navMesh->m_WireframeVertexIndices.size()/2;
  m_TriangleCount = (u32)navMesh->m_TriangleVertexIndices.size()/3;
  m_Indices->SetElementCount( (u32)navMesh->m_WireframeVertexIndices.size()  + (u32)navMesh->m_TriangleVertexIndices.size() );
  m_Indices->SetPopulator( PopulateSignature::Delegate( this, &NavMesh::Populate ) );
  m_Indices->Update();
}

void NavMesh::GetMarqueeSelectedVerts(Math::Frustum& marquee_frustom, MarqueeSelectionMode selection_mode)
{
  if ( selection_mode == kClearPreviousSelection )
  {
    ClearEditingState();
  }

  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  Math::V_Vector3::const_iterator pos_iter = navMesh->m_Positions.begin();
  for (u32 i=0; i<navMesh->m_Positions.size(); ++i, ++pos_iter)
  {
    if (marquee_frustom.IntersectsPoint(*pos_iter))
    {
      std::vector< u32 >::iterator find_it = std::find(m_selected_verts.begin(), m_selected_verts.end(), i);
      if (selection_mode != KRemovedFromPeeviousSElection)
      {
        if (find_it == m_selected_verts.end())
        {
          m_selected_verts.push_back(i);
        }
      }
      else
      {
        if (find_it != m_selected_verts.end())
        {
          m_selected_verts.erase(find_it);
        }
      }
      //m_marquee_selected_verts.push_back(i);
    }
  }
  std::sort(m_selected_verts.begin(), m_selected_verts.end());
}

void NavMesh::GetMarqueeSelectedTris(Math::Frustum& marquee_frustom, MarqueeSelectionMode selection_mode)
{
  if ( selection_mode == kClearPreviousSelection )
  {
    ClearEditingState();
  }

  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  std::vector< u32 >::const_iterator iter =  navMesh->m_TriangleVertexIndices.begin();
  for (u32 tri_id=0; iter != navMesh->m_TriangleVertexIndices.end(); ++tri_id)
  {
    u32 vert_ids[3];
    vert_ids[0] = *iter;
    ++iter;
    vert_ids[1] = *iter;
    ++iter;
    vert_ids[2] = *iter;
    ++iter;
    Math::Vector3 v[3];
    v[0] = navMesh->m_Positions[vert_ids[0]];
    v[1] = navMesh->m_Positions[vert_ids[1]];
    v[2] = navMesh->m_Positions[vert_ids[2]];
    if (marquee_frustom.IntersectsTriangle(v[0], v[1], v[2]))
    {
      //m_selected_tris.push_back( tri_id );

      std::vector< u32 >::iterator find_it = std::find(m_selected_tris.begin(), m_selected_tris.end(), tri_id);
      if (selection_mode != KRemovedFromPeeviousSElection)
      {
        if (find_it == m_selected_tris.end())
        {
          m_selected_tris.push_back(tri_id);
        }
      }
      else
      {
        if (find_it != m_selected_tris.end())
        {
          m_selected_tris.erase(find_it);
        }
      }
      //m_marquee_selected_tris.push_back(tri_id);
    }
  }
  std::sort(m_selected_tris.begin(), m_selected_tris.end());
  ChangeTriEdgeVertSelection();
}

void NavMesh::GetMarqueeSelectedEdges(Math::Frustum& marquee_frustom, MarqueeSelectionMode selection_mode)
{
  if ( selection_mode == kClearPreviousSelection )
  {
    m_marquee_selected_edges.clear();
  }
  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  std::vector< u32 >::const_iterator iter =  navMesh->m_WireframeVertexIndices.begin();
  for (u32 edge_id=0; iter!=navMesh->m_WireframeVertexIndices.end(); ++edge_id)
  {
    u32 vert_ids[2];
    vert_ids[0] = *iter;
    ++iter;
    vert_ids[1] = *iter;
    ++iter;
    Math::Vector3 v[2];
    v[0] = navMesh->m_Positions[vert_ids[0]];
    v[1] = navMesh->m_Positions[vert_ids[1]];
    if (marquee_frustom.IntersectsSegment(v[0], v[1]))
    {
      //m_marquee_selected_edges.push_back(edge_id);
      std::vector< u32 >::iterator find_it = std::find(m_marquee_selected_edges.begin(), m_marquee_selected_edges.end(), edge_id);
      if (selection_mode != KRemovedFromPeeviousSElection)
      {
        if (find_it == m_marquee_selected_edges.end())
        {
          m_marquee_selected_edges.push_back(edge_id);
        }
      }
      else
      {
        if (find_it != m_marquee_selected_edges.end())
        {
          m_marquee_selected_edges.erase(find_it);
        }
      }
    }
  }
  std::sort(m_marquee_selected_edges.begin(), m_marquee_selected_edges.end());
}

void NavMesh::SelectObtuseAngledTris()
{
  ClearEditingState();

  Content::Mesh* navMesh = GetPackage<Content::Mesh>();
  std::vector< u32 >::const_iterator iter =  navMesh->m_TriangleVertexIndices.begin();
  for (u32 tri_id=0; iter != navMesh->m_TriangleVertexIndices.end(); ++tri_id)
  {
    u32 vert_ids[3];
    vert_ids[0] = *iter;
    ++iter;
    vert_ids[1] = *iter;
    ++iter;
    vert_ids[2] = *iter;
    ++iter;
    Math::Vector3 v[3];
    v[0] = navMesh->m_Positions[vert_ids[0]];
    v[1] = navMesh->m_Positions[vert_ids[1]];
    v[2] = navMesh->m_Positions[vert_ids[2]];
    //since i cant rely on normals  (They dont exist and no way to say if they re up)
    Math::Vector3 e0, e1;
    e0 = v[1] - v[0];
    e1 = v[2] - v[0];
    if (e0.Dot(e1)< 0.0f)
    {
      m_selected_tris.push_back( tri_id );
      continue;
    }
    e0 = v[2] - v[1];
    e1 = v[0] - v[1];
    if (e0.Dot(e1)< 0.0f)
    {
      m_selected_tris.push_back( tri_id );
      continue;
    }
    e0 = v[0] - v[2];
    e1 = v[1] - v[2];
    if (e0.Dot(e1)< 0.0f)
    {
      m_selected_tris.push_back( tri_id );
      continue;
    }
    
  }
  ChangeTriEdgeVertSelection();
}

