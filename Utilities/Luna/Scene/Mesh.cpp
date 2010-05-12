#include "Precompile.h"
#include "Mesh.h"

#include "Content/Mesh.h"

#include "UIToolKit/ImageManager.h"
#include "Pick.h"
#include "Color.h"

#include "Scene.h"
#include "Shader.h"
#include "Transform.h"
#include "HierarchyNodeType.h"

using namespace Math;
using namespace Luna;

LUNA_DEFINE_TYPE( Luna::Mesh );

D3DMATERIAL9 Mesh::s_WireMaterial;
D3DMATERIAL9 Mesh::s_FillMaterial;

void Mesh::InitializeType()
{
  Reflect::RegisterClass< Luna::Mesh >( "Luna::Mesh" );

  ZeroMemory(&s_WireMaterial, sizeof(s_WireMaterial));
  s_WireMaterial.Ambient = Luna::Color::BLACK;
  s_WireMaterial.Diffuse = Luna::Color::BLACK;
  s_WireMaterial.Specular = Luna::Color::BLACK;

  ZeroMemory(&s_FillMaterial, sizeof(s_FillMaterial));
  s_FillMaterial.Ambient = Luna::Color::DARKGRAY;
  s_FillMaterial.Diffuse = Luna::Color::DARKGRAY;
  s_FillMaterial.Specular = Luna::Color::DARKGRAY;
}

void Mesh::CleanupType()
{
  Reflect::UnregisterClass< Luna::Mesh >();
}

Mesh::Mesh( Luna::Scene* scene, Content::Mesh* mesh )
: Luna::PivotTransform ( scene, mesh )
, m_HasAlpha (false)
, m_HasColor (false)
, m_HasTexture (false)
{
  m_LineCount = (u32)mesh->m_WireframeVertexIndices.size() / 2;
  m_VertexCount = (u32)mesh->m_Positions.size();
  m_TriangleCount = (u32)mesh->m_TriangleVertexIndices.size() / 3;

  u32 count = 0;
  for ( size_t i=0; i<mesh->m_ShaderIDs.size(); ++i )
  {
    m_ShaderStartIndices.push_back( count );
    count += mesh->m_ShaderTriangleCounts[i];
  }

  m_Indices = new IndexResource ( scene->GetView()->GetResources() );
  m_Indices->SetElementType( ElementTypes::Unsigned32 );
  m_Indices->SetElementCount( (u32)(mesh->m_WireframeVertexIndices.size() + mesh->m_TriangleVertexIndices.size()) );
  m_Indices->SetPopulator( PopulateSignature::Delegate( this, &Mesh::Populate ) );
  
  m_Vertices = new VertexResource ( scene->GetView()->GetResources() );
  m_Vertices->SetElementType( ElementTypes::StandardVertex );
  m_Vertices->SetElementCount( (u32)mesh->m_Positions.size() );


  m_Vertices->SetPopulator( PopulateSignature::Delegate( this, &Mesh::Populate ) );
}

Mesh::~Mesh()
{

}

i32 Mesh::GetImageIndex() const
{
  return UIToolKit::GlobalImageManager().GetImageIndex( "mesh_16.png" );
}

std::string Mesh::GetApplicationTypeName() const
{
  return "Mesh";
}

void Mesh::Initialize()
{
  __super::Initialize();

  Content::Mesh* mesh = GetPackage< Content::Mesh >();


  //
  // Dereference Shaders
  //

  UniqueID::V_TUID::const_iterator itr = mesh->m_ShaderIDs.begin();
  UniqueID::V_TUID::const_iterator end = mesh->m_ShaderIDs.end();
  for ( ; itr != end; ++itr )
  {
    Shader* shader = Reflect::ObjectCast< Shader >( m_Scene->FindNode( *itr ) );

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

  if ( mesh->m_Colors.size() > 0 )
  {
    if ( mesh->m_Colors.size() == 1 )
    {
      m_HasColor = mesh->m_Colors[0] != Math::Vector4( 0, 0, 0, 1.0 );
    }
    else
    {
      m_HasColor = true;
    }
  }

  Create();
}

void Mesh::Create()
{
  __super::Create();

  if (m_IsInitialized)
  {
    m_Indices->Create();
    m_Vertices->Create();
  }
}

void Mesh::Delete()
{
  __super::Delete();

  if (m_IsInitialized)
  {
    m_Indices->Delete();
    m_Vertices->Delete();
  }
}

void Mesh::Populate(PopulateArgs* args)
{
  Content::Mesh* mesh = GetPackage< Content::Mesh >();

  switch ( args->m_Type )
  {
  case ResourceTypes::Index:
    {
      if ( args->m_Buffer != NULL )
      {
        memcpy( args->m_Buffer + args->m_Offset, &( mesh->m_WireframeVertexIndices.front() ), mesh->m_WireframeVertexIndices.size() * sizeof( u32 ) );
        args->m_Offset += ( (u32)mesh->m_WireframeVertexIndices.size() * sizeof( u32 ) );

        memcpy( args->m_Buffer + args->m_Offset, &( mesh->m_TriangleVertexIndices.front() ), mesh->m_TriangleVertexIndices.size() * sizeof( u32 ) );
        args->m_Offset += ( (u32)mesh->m_TriangleVertexIndices.size() * sizeof( u32 ) );
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

          for ( u32 i=0; i<m_VertexCount; ++i )
          {
            // get address for the current vertex in the resource buffer
            vertex = reinterpret_cast<StandardVertex*>(args->m_Buffer + args->m_Offset) + i;

            // Position, test for local bounds computation
            vertex->m_Position = m_ObjectBounds.Test( mesh->m_Positions[i] );

            // Normal, used for lighting
            if (mesh->m_Normals.size())
            {
            vertex->m_Normal = mesh->m_Normals[i];
            }

            if (m_HasColor)
            {
              // Vertex Color
              vertex->m_Diffuse = D3DCOLOR_COLORVALUE( mesh->m_Colors[i].x, mesh->m_Colors[i].y, mesh->m_Colors[i].z, mesh->m_Colors[i].w );
            }

            if (m_HasTexture)
            {
              // Color Map UV
              vertex->m_BaseUV.x = mesh->m_BaseUVs[i].x;
              vertex->m_BaseUV.y = 1.0f - mesh->m_BaseUVs[i].y;
            }
          }

          args->m_Offset += ( m_VertexCount * sizeof( StandardVertex ) );

          NOC_ASSERT(args->m_Buffer + args->m_Offset == reinterpret_cast<u8*>(++vertex));
        }
      }
      break;
    }
  }
}

void Mesh::Evaluate(GraphDirection direction)
{
  switch (direction)
  {
  case GraphDirections::Downstream:
    {
      Content::Mesh* mesh = GetPackage< Content::Mesh >();

      m_ObjectBounds.Reset();

      for ( u32 i=0; i<m_VertexCount; ++i )
      {
        m_ObjectBounds.Test( mesh->m_Positions[i] );
      }

      if (m_IsInitialized)
      {
        m_Indices->Update();
        m_Vertices->Update();
      }

      break;
    }
  }

  __super::Evaluate(direction);
}

void Mesh::Render( RenderVisitor* render )
{
  RenderEntry* entry = render->Allocate(this);

  if (render->GetView()->GetCamera()->IsBackFaceCulling() && render->State().m_Matrix.Determinant() < 0)
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
  bool highlighted = ( ( IsHighlighted() && m_Scene->IsCurrent() ) || ( render->State().m_Highlighted ) ) && render->GetView()->IsHighlighting();
  bool selected = ( IsSelected() && m_Scene->IsCurrent() ) || ( render->State().m_Selected );
  bool live = ( IsLive() && m_Scene->IsCurrent() ) || ( render->State().m_Live );
  bool wire = render->GetView()->GetCamera()->GetWireframeOnShaded();

  switch ( render->GetView()->GetCamera()->GetShadingMode() )
  {
  case ShadingModes::Wireframe:
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

      if ( render->GetView()->GetCamera()->GetWireframeOnMesh() && ( highlighted || selected || live || wire ) )
      {
        // this will draw unbiased wireframe
        entry = render->Allocate(this);

        if (render->GetView()->GetCamera()->IsBackFaceCulling() && render->State().m_Matrix.Determinant() < 0)
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

  __super::Render( render );
}

void Mesh::SetupNormalObject( IDirect3DDevice9* device, const SceneNode* object )
{
  const Luna::Mesh* mesh = Reflect::ConstAssertCast< Luna::Mesh > ( object );

  const Resource* indices = mesh->m_Indices;
  const Resource* vertices = mesh->m_Vertices;

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
  const Luna::HierarchyNode* node = Reflect::ConstAssertCast<Luna::HierarchyNode>( object );
  const Luna::Mesh* mesh = Reflect::ConstAssertCast<Luna::Mesh>( node );
  
  const IndexResource* indices = mesh->m_Indices;
  const VertexResource* vertices = mesh->m_Vertices;

  if (indices && vertices)
  {
    device->DrawIndexedPrimitive( D3DPT_LINELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), indices->GetBaseIndex(), mesh->m_LineCount );
    args->m_LineCount += mesh->m_LineCount;
  }
  else
  {
    NOC_BREAK();
  }
}

void Mesh::DrawUnselectableWire( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  DrawNormalWire( device, args, object );
}

void Mesh::SetupUnselectableWire( IDirect3DDevice9* device )
{
  device->SetMaterial( &Luna::View::s_UnselectableMaterial );
}

void Mesh::SetupSelectedWire( IDirect3DDevice9* device )
{
  device->SetMaterial( &Luna::View::s_SelectedMaterial );
}

void Mesh::SetupHighlightedWire( IDirect3DDevice9* device )
{
  device->SetMaterial( &Luna::View::s_HighlightedMaterial );
}

void Mesh::SetupLiveWire( IDirect3DDevice9* device )
{
  device->SetMaterial( &Luna::View::s_LiveMaterial );
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
  LUNA_SCENE_DRAW_SCOPE_TIMER( ("") );

  const Luna::HierarchyNode* node = Reflect::ConstAssertCast<Luna::HierarchyNode>( object );
  const Luna::Mesh* mesh = Reflect::ConstAssertCast<Luna::Mesh>( node );
  const Content::Mesh* data = mesh->GetPackage< Content::Mesh >();

  const IndexResource* indices = mesh->m_Indices;
  const VertexResource* vertices = mesh->m_Vertices;

  if (indices && vertices)
  {
    Luna::View* view = node->GetScene()->GetView();
    Luna::Camera* camera = view->GetCamera();

    switch ( camera->GetShadingMode() )
    {
    case ShadingModes::Material:
      {
        device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, vertices->GetBaseIndex(), 0, vertices->GetElementCount(), (UINT)( indices->GetBaseIndex() + data->m_WireframeVertexIndices.size() ), (UINT)( mesh->m_TriangleCount ) );

        args->m_TriangleCount += mesh->m_TriangleCount;

        break;
      }

    case ShadingModes::Texture:
      {
        static size_t shaderCount;
        static size_t shaderTriCountsCount;
        static size_t shaderStartIndicesCount;

#pragma TODO("Fix the bug in content export that requires these checks")
        shaderCount = mesh->m_Shaders.size();

        shaderTriCountsCount = data->m_ShaderTriangleCounts.size();
        if (shaderCount != shaderTriCountsCount)
        {
          shaderTriCountsCount = shaderCount = MIN(shaderTriCountsCount, shaderCount);
        }

        shaderStartIndicesCount = mesh->m_ShaderStartIndices.size();
        if (shaderCount != shaderStartIndicesCount)
        {
          shaderStartIndicesCount = shaderCount = MIN(shaderStartIndicesCount, shaderCount);
        }

        for ( u32 shaderIndex = 0; shaderIndex < shaderTriCountsCount; shaderIndex++ )
        {
          u32 triangleCount = data->m_ShaderTriangleCounts[shaderIndex];

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

            u32 startIndex = (u32)indices->GetBaseIndex() + (u32)data->m_WireframeVertexIndices.size() + (mesh->m_ShaderStartIndices[shaderIndex] * 3);

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
    NOC_BREAK();
  }
}

bool Mesh::Pick( PickVisitor* pick )
{
  const Content::Mesh* mesh = GetPackage< Content::Mesh >();
  const Luna::Transform* t = GetTransform();

  // save the size for checking if we got local hits later
  size_t high = pick->GetHitCount();

  // set the pick's matrices to process intersections in this space
  pick->SetCurrentObject (this, pick->State().m_Matrix);

  if (pick->GetCamera()->GetShadingMode() == ShadingModes::Wireframe)
  {
    // test each segment (vertex data is in local space, intersection function will transform)
    for (size_t i=0; i<mesh->m_WireframeVertexIndices.size(); i+=2)
    {
      pick->PickSegment(mesh->m_Positions[ mesh->m_WireframeVertexIndices[i] ],
                        mesh->m_Positions[ mesh->m_WireframeVertexIndices[i+1] ]);
    }
  }
  else
  {
    // test each triangle (vertex data is in local space, intersection function will transform)
    for (size_t i=0; i<mesh->m_TriangleVertexIndices.size(); i+=3)
    {
      pick->PickTriangle(mesh->m_Positions[ mesh->m_TriangleVertexIndices[i] ],
                         mesh->m_Positions[ mesh->m_TriangleVertexIndices[i+1] ],
                         mesh->m_Positions[ mesh->m_TriangleVertexIndices[i+2] ]);
    }
  }

  return pick->GetHits().size() > high;
}