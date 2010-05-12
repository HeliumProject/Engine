#include "Precompile.h"
#include "LightingVolume.h"

#include "RemoteScene.h"
#include "Scene.h"
#include "SceneEditor.h"
#include "SceneManager.h"

#include "LightingVolumeType.h"

#include "Core/Enumerator.h"
#include "UIToolKit/ImageManager.h"

#include "PrimitiveCube.h"
#include "PrimitivePointer.h"

#include "LightingVolumePanel.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/CubeMapAttribute.h"
#include "Asset/SceneManifest.h"

#include <algorithm>
#include "File/Exceptions.h"
#include "File/Manager.h"

#include "Content/SunShadowMergeAttribute.h"
#include "Content/GlossControlAttribute.h"
#include "Content/GroundLightAttribute.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "ShadowDirection.h"
#include "AmbientLight.h"
#include "SunLight.h"
#include "Layer.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::LightingVolume);

void LightingVolume::InitializeType()
{
  Reflect::RegisterClass< Luna::LightingVolume >( "Luna::LightingVolume" );

  Enumerator::InitializePanel( "LightingVolume", CreatePanelSignature::Delegate( &LightingVolume::CreatePanel ) );
}

void LightingVolume::CleanupType()
{
  Reflect::UnregisterClass< Luna::LightingVolume >();
}

LightingVolume::LightingVolume(Luna::Scene* scene)
: Luna::Instance ( scene, new Content::LightingVolume() )
{
}

LightingVolume::LightingVolume(Luna::Scene* scene, Content::LightingVolume* volume )
: Luna::Instance ( scene, volume )
{
}

void LightingVolume::PopulateManifest( Asset::SceneManifest* manifest ) const
{
  Attribute::AttributeViewer< Asset::CubeMapAttribute > cubeMap ( const_cast< Content::LightingVolume* >( GetPackage<Content::LightingVolume>() ) );

  if ( cubeMap.Valid() )
  {
    manifest->m_Assets.insert( cubeMap->GetFileID() );
  }
}

i32 LightingVolume::GetImageIndex() const
{
  return UIToolKit::GlobalImageManager().GetImageIndex( "cube_16.png" );
}

std::string LightingVolume::GetApplicationTypeName() const
{
  return "LightingVolume";
}

SceneNodeTypePtr LightingVolume::CreateNodeType( Luna::Scene* scene ) const
{
  // Overridden to create an volume-specific type
  Luna::LightingVolumeType* nodeType = new Luna::LightingVolumeType( scene, GetType() );

  // Set the image index (usually this is handled by the base class, but we aren't calling the base)
  nodeType->SetImageIndex( GetImageIndex() );

  return nodeType;
}

bool LightingVolume::ChildParentChanging( const ParentChangingArgs& args )
{
  return false;
}

void LightingVolume::Evaluate(GraphDirection direction)
{
  switch (direction)
  {
  case GraphDirections::Downstream:
    {
      // start the box from scratch
      m_ObjectBounds.Reset();

      // merge type pointer into our bounding box
      if (m_NodeType)
      {
        Luna::InstanceType* type = Reflect::AssertCast<Luna::InstanceType>(m_NodeType);
        m_ObjectBounds.Merge( type->GetPointer()->GetBounds() );
      }
      break;
    }
  }

  __super::Evaluate(direction);
}

void LightingVolume::Render( RenderVisitor* render )
{
  // pointer is drawn normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix.Normalized();
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &LightingVolume::DrawPointer;
  }

  // shape is drawn non-normalized
  {
    RenderEntry* entry = render->Allocate(this);
    entry->m_Location = render->State().m_Matrix;
    entry->m_Center = m_ObjectBounds.Center();
    entry->m_Draw = &LightingVolume::DrawShape;
  }

  // don't call __super here, it will draw big ass axes
  Luna::HierarchyNode::Render( render );
}

void LightingVolume::DrawPointer( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::LightingVolume* lightingVolume = Reflect::ConstAssertCast<Luna::LightingVolume>( object );

  const Luna::InstanceType* type = Reflect::ConstAssertCast<Luna::InstanceType>( lightingVolume->GetNodeType() );

  lightingVolume->SetMaterial( type->GetMaterial() );

  // draw type pointer
  type->GetPointer()->Draw( args );
}

void LightingVolume::DrawShape( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
  const Luna::LightingVolume* lightingVolume = Reflect::ConstAssertCast<Luna::LightingVolume>( object );

  const Luna::LightingVolumeType* type = Reflect::ConstAssertCast<Luna::LightingVolumeType>( lightingVolume->GetNodeType() );

  lightingVolume->SetMaterial( type->GetMaterial() );

  // draw cube
  type->GetCube()->Draw( args );
}

bool LightingVolume::Pick( PickVisitor* pick )
{
  bool result = false;

  Luna::LightingVolumeType* type = Reflect::AssertCast<Luna::LightingVolumeType>(m_NodeType);

  pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());
  result |= type->GetPointer()->Pick (pick);

  pick->SetCurrentObject (this, pick->State().m_Matrix);
  result |= type->GetCube()->Pick(pick);

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Luna::LightingVolume.
//
bool LightingVolume::ValidatePanel(const std::string& name)
{
  if (name == "LightingVolume")
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

///////////////////////////////////////////////////////////////////////////////
// Static function for creating the UI panel that allows users to edit
// Luna::LightingVolume.
//
void LightingVolume::CreatePanel( CreatePanelArgs& args )
{
  LightingVolumePanel* panel = new LightingVolumePanel ( args.m_Enumerator, args.m_Selection );

  args.m_Enumerator->Push( panel );
  {
    panel->SetCanvas( args.m_Enumerator->GetContainer()->GetCanvas() );
    panel->Create();
  }
  args.m_Enumerator->Pop();
}

bool LightingVolume::GetExcludeGlobalLights() const
{
  return GetPackage< Content::LightingVolume >()->m_ExcludeGlobalLights;
}

void LightingVolume::SetExcludeGlobalLights( bool exclude )
{
  GetPackage< Content::LightingVolume >()->m_ExcludeGlobalLights = exclude;
  m_Changed.Raise( LightingVolumeChangeArgs( this ) );
}

void LightingVolume::PackageChanged( const Reflect::ElementChangeArgs& args )
{
  __super::PackageChanged( args );

  const Content::SunShadowMergeAttribute*   sunShadowMergeAttr    = GetPackage< Content::SunShadowMergeAttribute >();
  const Content::GlossControlAttribute*     glossControlAttr      = GetPackage< Content::GlossControlAttribute >();
  const Content::GroundLightAttribute*      groundLightAttr       = GetPackage< Content::GroundLightAttribute >();
//  const Content::RimLightAttribute*         rimLightAttr          = GetPackage< Content::RimLightAttribute >();

  if( ( sunShadowMergeAttr    == args.m_Element ) ||
      ( glossControlAttr      == args.m_Element ) ||
      ( groundLightAttr       == args.m_Element ) )
//    || ( rimLightAttr          == args.m_Element ))
  {
    m_Changed.Raise( LightingVolumeChangeArgs( this ) );
  }
}


bool LightingVolume::IsLinkableLight( Light* light )
{
  if( light->GetScene() != GetScene() )
    return false;

  if( light->HasType( Reflect::GetType< Luna::DirectionalLight >() )|| 
      light->HasType( Reflect::GetType< Luna::ShadowDirection >() ) || 
      light->HasType( Reflect::GetType< Luna::AmbientLight >()  )   || 
      light->HasType( Reflect::GetType< Luna::SunLight >() )          )
  {
    return true;
  }

  return false;
}

void LightingVolume::GetLinkedLights( V_LightDumbPtr& lights )
{
  Luna::Scene* volumeScene = GetScene();
  if( volumeScene )
  {
    std::vector< Layer* > layers;
    volumeScene->GetAll< Layer >( layers );

    std::vector< Layer* >::const_iterator layerItr = layers.begin();
    std::vector< Layer* >::const_iterator layerEnd = layers.end();

    //Loop over the layers
    for ( ; layerItr != layerEnd; ++layerItr )
    {
      Luna::Layer*                    lunaLayer     = (*layerItr);
      OS_SelectableDumbPtr            layerMembers  = lunaLayer->GetMembers();

      OS_SelectableDumbPtr::Iterator  memberItr     = layerMembers.Begin();
      OS_SelectableDumbPtr::Iterator  memberEnd     = layerMembers.End();

      for ( ; memberItr != memberEnd; ++memberItr )
      {
        Selectable* member = (*memberItr);

        //Check for the supported types
        Light* light = Reflect::ObjectCast< Luna::Light >( member );
        if( light )
        {
          lights.push_back( light );     
        }
      }
    }
  }
}