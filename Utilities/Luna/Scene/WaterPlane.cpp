#include "Precompile.h"
#include "WaterPlane.h"

#include "Content/WaterPlane.h"

#include "UIToolKit/ImageManager.h"
#include "Pick.h"
#include "Color.h"

#include "File/Manager.h"
#include "FileSystem/FileSystem.h"

#include "Scene.h"
#include "Shader.h"
#include "Transform.h"
#include "HierarchyNodeType.h"

using namespace Math;
using namespace Luna;

D3DMATERIAL9 WaterPlane::s_WireMaterial;
D3DMATERIAL9 WaterPlane::s_FillMaterial;

LUNA_DEFINE_TYPE( Luna::WaterPlane );

void WaterPlane::InitializeType()
{
  Reflect::RegisterClass< Luna::WaterPlane >( "Luna::WaterPlane" );

  ZeroMemory(&s_WireMaterial, sizeof(s_WireMaterial));
  s_WireMaterial.Ambient = Luna::Color::BLACK;
  s_WireMaterial.Diffuse = Luna::Color::BLACK;
  s_WireMaterial.Specular = Luna::Color::BLACK;

  ZeroMemory(&s_FillMaterial, sizeof(s_FillMaterial));
  s_FillMaterial.Ambient = Luna::Color::DODGERBLUE;
  s_FillMaterial.Diffuse = Luna::Color::DODGERBLUE;
  s_FillMaterial.Specular = Luna::Color::DODGERBLUE;
}

void WaterPlane::CleanupType()
{
  Reflect::UnregisterClass< Luna::WaterPlane >();
}

WaterPlane::WaterPlane( Luna::Scene* scene, Content::WaterPlane* waterPlane )
: Luna::PivotTransform ( scene, waterPlane )
, m_Color( waterPlane->m_Color )
, m_ClipMap( NULL )
 {
  std::string clipMap = File::GlobalManager().GetPath( waterPlane->m_ClipMapTuid );
  if ( !clipMap.empty() && FileSystem::Exists( clipMap ) && Luna::IsSupportedTexture( clipMap ) )
  {
    u32 clipMapSize = 0;
    bool clipMapAlpha = false;
    m_ClipMap = Luna::LoadTexture( scene->GetView()->GetDevice(), clipMap, &clipMapSize, &clipMapAlpha );
  }

  // Setup indices
  m_IndexList.resize( 14 );

  // Setup line indices
  m_LineCount = 4;
  m_IndexList[ 0 ] = 0;
  m_IndexList[ 1 ] = 1;
  m_IndexList[ 2 ] = 1;
  m_IndexList[ 3 ] = 3;
  m_IndexList[ 4 ] = 3;
  m_IndexList[ 5 ] = 2;
  m_IndexList[ 6 ] = 2;
  m_IndexList[ 7 ] = 0;

  // Setup triangle indices
  m_TriangleCount = 2;
  m_IndexList[ 8 ] = 0;
  m_IndexList[ 9 ] = 1;
  m_IndexList[ 10 ] = 2;
  m_IndexList[ 11 ] = 1;
  m_IndexList[ 12 ] = 3;
  m_IndexList[ 13 ] = 2;

  // Setup Vertices & UVs
  m_VertexList.resize( 4 );
  m_UVList.resize( m_VertexList.size() );

  Math::Scale scale;
  Math::Matrix3 rotate;
  Math::Vector3 translate;
  waterPlane->m_GlobalTransform.Decompose( scale, rotate, translate );

  m_VertexList[ 0 ].x = translate.x - scale.x;
  m_VertexList[ 0 ].y = translate.y;
  m_VertexList[ 0 ].z = translate.z - scale.z;
  m_UVList[ 0 ].x = 0.0f;
  m_UVList[ 0 ].y = 0.0f;

  m_VertexList[ 1 ].x = translate.x - scale.x;
  m_VertexList[ 1 ].y = translate.y;
  m_VertexList[ 1 ].z = translate.z + scale.z;
  m_UVList[ 1 ].x = 0.0f;
  m_UVList[ 1 ].y = 1.0f;

  m_VertexList[ 2 ].x = translate.x + scale.x;
  m_VertexList[ 2 ].y = translate.y;
  m_VertexList[ 2 ].z = translate.z - scale.z;
  m_UVList[ 2 ].x = 1.0f;
  m_UVList[ 2 ].y = 0.0f;

  m_VertexList[ 3 ].x = translate.x + scale.x;
  m_VertexList[ 3 ].y = translate.y;
  m_VertexList[ 3 ].z = translate.z + scale.z;
  m_UVList[ 3 ].x = 1.0f;
  m_UVList[ 3 ].y = 1.0f;

  m_Indices = new IndexResource ( scene->GetView()->GetResources() );
  m_Indices->SetElementType( ElementTypes::Unsigned32 );
  m_Indices->SetElementCount( (u32) m_IndexList.size() );
  m_Indices->SetPopulator( PopulateSignature::Delegate( this, &WaterPlane::Populate ) );
  
  m_Vertices = new VertexResource ( scene->GetView()->GetResources() );
  m_Vertices->SetElementType( ElementTypes::StandardVertex );
  m_Vertices->SetElementCount( (u32) m_VertexList.size() );
  m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &WaterPlane::Populate ) );
}

WaterPlane::~WaterPlane()
{
  if ( m_ClipMap )
  {
    m_ClipMap->Release();
  }
}

void WaterPlane::Initialize()
{
  __super::Initialize();

  Create();
}

void WaterPlane::Create()
{
  __super::Create();

  if (m_IsInitialized)
  {
    m_Indices->Create();
    m_Vertices->Create();
  }
}

void WaterPlane::Delete()
{
  __super::Delete();

  if (m_IsInitialized)
  {
    m_Indices->Delete();
    m_Vertices->Delete();
  }
}

void WaterPlane::Populate(PopulateArgs* args)
{
  switch ( args->m_Type )
  {
  case ResourceTypes::Index:
    {
      if ( args->m_Buffer != NULL )
      {
        memcpy( args->m_Buffer + args->m_Offset, &( m_IndexList.front() ), m_IndexList.size() * sizeof( u32 ) );
        args->m_Offset += (u32) ( m_IndexList.size() * sizeof( u32 ) );
      }

      break;
    }
  
  case ResourceTypes::Vertex:
    {
      if ( args->m_Buffer != NULL )
      {
        m_ObjectBounds.Reset();

        for ( u32 i = 0; i < m_VertexList.size(); ++i )
        {
          StandardVertex* vertex = reinterpret_cast<StandardVertex*>( args->m_Buffer + args->m_Offset ) + i;
          vertex->m_Position = m_ObjectBounds.Test( m_VertexList[ i ] );
          vertex->m_BaseUV.x = m_UVList[ i ].x;
          vertex->m_BaseUV.y = m_UVList[ i ].y;
          vertex->m_Diffuse = D3DCOLOR_COLORVALUE( 128, 128, 128, 255 );
        }
        
        args->m_Offset += (u32) ( m_VertexList.size() * sizeof( StandardVertex ) );
      }

      break;
    }
  }
}

void WaterPlane::Evaluate(GraphDirection direction)
{
  switch (direction)
  {
  case GraphDirections::Downstream:
    {
      m_ObjectBounds.Reset();

      for ( u32 i = 0; i < (u32) m_VertexList.size() ; ++i )
      {
        m_ObjectBounds.Test( m_VertexList[ i ] );
      }

      if ( m_IsInitialized )
      {
        m_Indices->Update();
        m_Vertices->Update();
      }

      break;
    }
  }

  __super::Evaluate(direction);
}

void WaterPlane::Render( RenderVisitor* render )
{
  ZeroMemory(&s_FillMaterial, sizeof(s_FillMaterial));
  s_FillMaterial.Ambient = Color::ColorToColorValue( 255, m_Color.r, m_Color.g, m_Color.b );
  s_FillMaterial.Diffuse = Color::ColorToColorValue( 255, m_Color.r, m_Color.g, m_Color.b );
  s_FillMaterial.Specular = Color::ColorToColorValue( 255, m_Color.r, m_Color.g, m_Color.b );

  RenderEntry* entry = render->Allocate(this);

  entry->m_ObjectSetup = &WaterPlane::SetupNormalObject;
  entry->m_ObjectReset = &WaterPlane::ResetNormalObject;
  entry->m_Location = render->State().m_Matrix;
  entry->m_Center = m_ObjectBounds.Center();

  bool highlighted = ( IsHighlighted() && m_Scene->IsCurrent() ) || ( render->State().m_Highlighted );
  bool selected = ( IsSelected() && m_Scene->IsCurrent() ) || ( render->State().m_Selected );
  bool live = ( IsLive() && m_Scene->IsCurrent() ) || ( render->State().m_Live );
  bool wire = render->GetView()->GetCamera()->GetWireframeOnShaded();

  switch ( render->GetView()->GetCamera()->GetShadingMode() )
  {
  case ShadingModes::Wireframe:
    {
      if (highlighted)
      {
        entry->m_DrawSetup = &WaterPlane::SetupHighlightedWire;
      }
      else if (selected)
      {
        entry->m_DrawSetup = &WaterPlane::SetupSelectedWire;
      }
      else if (live)
      {
        entry->m_DrawSetup = &WaterPlane::SetupLiveWire;
      }
      else
      {
        entry->m_DrawSetup = &WaterPlane::SetupNormalWire;
      }

      entry->m_Draw = &WaterPlane::DrawNormalWire;
      
      break;
    }
  
  default:
    {
      entry->m_DrawSetup = &WaterPlane::SetupNormal;
      entry->m_Draw = &WaterPlane::DrawNormal;

      if ( render->GetView()->GetCamera()->GetWireframeOnMesh() && ( highlighted || selected || live || wire ) )
      {
        entry = render->Allocate(this);
        entry->m_Draw = &WaterPlane::DrawNormalWire;
        entry->m_Location = render->State().m_Matrix;
        entry->m_Center = m_ObjectBounds.Center();

        if (highlighted)
        {
          entry->m_DrawSetup = &WaterPlane::SetupHighlightedWire;
        }
        else if (selected)
        {
          entry->m_DrawSetup = &WaterPlane::SetupSelectedWire;
        }
        else if (live)
        {
          entry->m_DrawSetup = &WaterPlane::SetupLiveWire;
        }
        else if (wire)
        {
          entry->m_DrawSetup = &WaterPlane::SetupNormalWire;
        }
      }
      
      break;
    }
  }

  __super::Render( render );
}

void WaterPlane::SetupNormalObject( IDirect3DDevice9* device, const SceneNode* object )
{
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

  const Luna::WaterPlane* waterPlane = Reflect::ConstAssertCast< Luna::WaterPlane > ( object );

  const Resource* indices = waterPlane->m_Indices;
  const Resource* vertices = waterPlane->m_Vertices;

  if (indices && vertices)
  {
    indices->SetState();
    vertices->SetState();
  }
  else
  {
    NOC_BREAK();
  }
}

void WaterPlane::ResetNormalObject( IDirect3DDevice9* device, const SceneNode* object )
{
  device->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
}

void WaterPlane::SetupNormalWire( IDirect3DDevice9* device )
{
  device->SetMaterial( &s_WireMaterial );
}

void WaterPlane::SetupSelectedWire( IDirect3DDevice9* device )
{
  device->SetMaterial( &Luna::View::s_SelectedMaterial );
}

void WaterPlane::SetupHighlightedWire( IDirect3DDevice9* device )
{
  device->SetMaterial( &Luna::View::s_HighlightedMaterial );
}

void WaterPlane::SetupLiveWire( IDirect3DDevice9* device )
{
  device->SetMaterial( &Luna::View::s_LiveMaterial );
}

void WaterPlane::DrawNormalWire( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::HierarchyNode* node = Reflect::ConstAssertCast<Luna::HierarchyNode>( object );
  const Luna::WaterPlane* waterPlane = Reflect::ConstAssertCast<Luna::WaterPlane>( node );
  
  const IndexResource* indices = waterPlane->m_Indices;
  const VertexResource* vertices = waterPlane->m_Vertices;

  if (indices && vertices)
  {
    device->DrawIndexedPrimitive( D3DPT_LINELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), indices->GetBaseIndex(), waterPlane->m_LineCount );
    args->m_LineCount += waterPlane->m_LineCount;
  }
  else
  {
    NOC_BREAK();
  }
}

void WaterPlane::SetupNormal( IDirect3DDevice9* device )
{
  device->SetMaterial( &s_FillMaterial );
}

void WaterPlane::DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  LUNA_SCENE_DRAW_SCOPE_TIMER( ("") );

  const Luna::HierarchyNode* node = Reflect::ConstAssertCast<Luna::HierarchyNode>( object );
  const Luna::WaterPlane* waterPlane = Reflect::ConstAssertCast<Luna::WaterPlane>( node );

  const IndexResource* indices = waterPlane->m_Indices;
  const VertexResource* vertices = waterPlane->m_Vertices;

  if ( indices && vertices )
  {
    if ( waterPlane->m_ClipMap && ( node->GetScene()->GetView()->GetCamera()->GetShadingMode() == ShadingModes::Texture ) )
    {
      device->SetTexture( 0, waterPlane->m_ClipMap );
    }
          
    device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), (UINT)( indices->GetBaseIndex() + waterPlane->m_LineCount * 2 ), (UINT)( waterPlane->m_TriangleCount ) );
    args->m_TriangleCount += waterPlane->m_TriangleCount;
    device->SetTexture( 0, NULL );
  }
}

bool WaterPlane::Pick( PickVisitor* pick )
{
  const Content::WaterPlane* waterPlane = GetPackage< Content::WaterPlane >();
  const Luna::Transform* t = GetTransform();

  // save the size for checking if we got local hits later
  size_t high = pick->GetHitCount();

  // set the pick's matrices to process intersections in this space
  pick->SetCurrentObject (this, pick->State().m_Matrix);

  if (pick->GetCamera()->GetShadingMode() == ShadingModes::Wireframe)
  {
    // test each segment (vertex data is in local space, intersection function will transform)
    for ( u32 i = 0; i < m_LineCount; ++i )
    {
      pick->PickSegment( m_VertexList[ m_IndexList[ 2 * i ] ], m_VertexList[ m_IndexList[ 2 * i + 1] ] );
    }
  }
  else
  {
    // test each triangle (vertex data is in local space, intersection function will transform)
    u32 triangleStart = m_LineCount * 2;
    for ( u32 i=0; i < m_TriangleCount; ++i)
    {
      pick->PickTriangle( m_VertexList[ m_IndexList[ triangleStart + 3 * i ] ], m_VertexList[ m_IndexList[ triangleStart + 3 * i+ 1 ] ], m_VertexList[ m_IndexList[ triangleStart + 3 * i+ 2 ] ] );
    }
  }
  
  return pick->GetHits().size() > high;
}