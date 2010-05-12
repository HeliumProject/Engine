#include "Precompile.h"
#include "EntityPanel.h"
#include "EntityAssetSet.h"
#include "InstanceCodeSet.h"

#include "Inspect/Value.h"
#include "Inspect/Button.h"
#include "Task/Build.h"
#include "Editor/SessionManager.h"

#include "Asset/AssetInit.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/DrawAttribute.h"
#include "Asset/FoliageAttribute.h"
#include "Asset/FoliageOverrideAttribute.h"
#include "Asset/UpdateAttribute.h"
#include "Asset/SegmentAttribute.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/CubeMapAttribute.h"
#include "Asset/AssetClass.h"
#include "Asset/ShaderGroupAttribute.h"
#include "Asset/ShaderGroupOverrideAttribute.h"
#include "Asset/VisualAttribute.h"
#include "Asset/VisualOverrideAttribute.h"
#include "Asset/InstanceCollisionAttribute.h"
#include "Asset/TexelsPerMeterAttribute.h"
#include "Asset/UpdateInstanceAttribute.h"
#include "FileSystem/FileSystem.h"
#include "File/Manager.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ContentSpecs.h"
#include "Console/Console.h"
#include "Windows/Process.h"
#include "Task/Export.h"
#include "Volume.h"

#include "Light.h"

#include "Scene.h"
#include "SceneManager.h"
#include "SceneEditor.h"
#include "RemoteScene.h"

#include "IPC/Connection.h"
#include "RemoteConstruct.h"

#include "Editor/Editor.h"
#include "Symbol/Inheritance.h"

#include "LightmapUVViewer.h"

using namespace Reflect;
using namespace Asset;
using namespace Luna;

template <class T>
bool SelectionHasAttribute(const OS_SelectableDumbPtr& selection)
{
  OS_SelectableDumbPtr::Iterator itr = selection.Begin();
  OS_SelectableDumbPtr::Iterator end = selection.End();
  for ( ; itr != end; ++itr )
  {
    Luna::SceneNode* node = Reflect::ObjectCast< Luna::SceneNode >( *itr );

    if (!node)
    {
      return false;
    }

    Content::SceneNode* packageNode = node->GetPackage<Content::SceneNode>();

    if (!packageNode)
    {
      return false;
    }

    Attribute::AttributeViewer<T> attr ( packageNode );

    if (!attr.Valid())
    {
      return false;
    }
  }

  return true;
}

template <class T>
bool SelectionHasSameAttribute(const OS_SelectableDumbPtr& selection, Attribute::AttributeViewer< T >& attribute)
{
  OS_SelectableDumbPtr::Iterator itr = selection.Begin();
  OS_SelectableDumbPtr::Iterator end = selection.End();
  for ( ; itr != end; ++itr )
  {
    Luna::SceneNode* node = Reflect::ObjectCast< Luna::SceneNode >( *itr );

    if (!node)
    {
      return false;
    }

    Content::SceneNode* packageNode = node->GetPackage<Content::SceneNode>();

    if (!packageNode)
    {
      return false;
    }

    if ( itr == selection.Begin() )
    {
      attribute.View( packageNode );
      if ( !attribute.Valid() )
      {
        return false;
      }
    }
    else
    {
      Attribute::AttributeViewer< T > nextAttribute = Attribute::AttributeViewer< T >( packageNode );
      if ( !nextAttribute.Valid() || ( attribute.operator->() != nextAttribute.operator->() ) )
      {
        return false;
      }
    }
  }

  return true;
}

bool IsEngineType( const OS_SelectableDumbPtr& selection, Asset::EngineTypes::EngineType engineType )
{
  if ( selection.Empty() )
  {
    return false;
  }

  OS_SelectableDumbPtr::Iterator itr = selection.Begin();
  OS_SelectableDumbPtr::Iterator end = selection.End();
  for ( ; itr != end; ++itr )
  {
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *itr );
    if ( entity && entity->GetClassSet() && entity->GetClassSet()->GetEntityAsset() )
    {
      if ( entity->GetClassSet()->GetEntityAsset()->GetEngineType() != engineType )
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }

  return true;
}

EntityPanel::EntityPanel(Enumerator* enumerator, const OS_SelectableDumbPtr& selection)
: InstancePanel (enumerator, selection)
, m_LightingPanel ( NULL )
, m_DrawDistance ( NULL )
, m_UpdateDistance ( NULL )
, m_ShaderGroup ( NULL )
, m_Attenuation( NULL )
, m_FarShadowFadeout( NULL )
, m_CastsBakedShadows( NULL )
, m_DoBakedShadowCollisions( NULL )
, m_BakedShadowAABBExt ( NULL )
, m_BakedShadowMergeGroups( NULL )
, m_HighResShadowMap( NULL )
, m_SegmentEnabler ( NULL )
, m_ShaderGroupEnabler ( NULL )
, m_FarShadowFadeoutEnabler( NULL )
, m_CastsBakedShadowsEnabler( NULL )
, m_DoBakedShadowCollisionsEnabler( NULL )
, m_BakedShadowAABBExtEnabler( NULL )
, m_BakedShadowMergeGroupsEnabler( NULL )
, m_HighResShadowMapEnabler( NULL )
, m_AllEntitiesSpecialized ( false )
, m_EnableSpecializeCheckbox ( false )
{
  m_Expanded = true;
  m_Text = "Entity";

  OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator end = m_Selection.End();
  for ( ; itr != end; ++itr )
  {
    Luna::Entity* entity = Reflect::AssertCast< Luna::Entity >( *itr );
    if ( entity && entity->GetClassSet() )
    {
      entity->GetClassSet()->AddClassLoadedListener( EntityAssetSetChangeSignature::Delegate ( this, &EntityPanel::EntityAssetReloaded ) );
      m_Entities.push_back( entity );
    }
  }

  m_SegmentEnabler = new ControlEnabler ();
  m_ShaderGroupEnabler = new ControlEnabler ();
  m_FarShadowFadeoutEnabler = new ControlEnabler ();
  m_CastsBakedShadowsEnabler = new ControlEnabler ();
  m_DoBakedShadowCollisionsEnabler = new ControlEnabler ();
  m_BakedShadowAABBExtEnabler = new ControlEnabler ();
  m_BakedShadowMergeGroupsEnabler = new ControlEnabler ();
  m_HighResShadowMapEnabler = new ControlEnabler ();
}

EntityPanel::~EntityPanel()
{
  V_EntitySmartPtr::const_iterator itr = m_Entities.begin();
  V_EntitySmartPtr::const_iterator end = m_Entities.end();
  for ( ; itr != end; ++itr )
  {
    Luna::Entity* entity = *itr;
    if ( entity->GetClassSet() )
    {
      entity->GetClassSet()->RemoveClassLoadedListener( EntityAssetSetChangeSignature::Delegate ( this, &EntityPanel::EntityAssetReloaded ) );
    }
  }

  delete m_SegmentEnabler;
  delete m_ShaderGroupEnabler;
  delete m_FarShadowFadeoutEnabler;
  delete m_CastsBakedShadowsEnabler;
  delete m_DoBakedShadowCollisionsEnabler;
  delete m_BakedShadowAABBExtEnabler;
  delete m_BakedShadowMergeGroupsEnabler;
  delete m_HighResShadowMapEnabler;
}

void EntityPanel::CreateEngineType()
{
  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel( "Engine Type" );
    Inspect::Value* textBox = m_Enumerator->AddValue<Luna::Entity, std::string>( m_Selection, &Luna::Entity::GetEngineTypeName, &Luna::Entity::SetEngineTypeName );
    textBox->SetReadOnly( true );
  }
  m_Enumerator->Pop();
}

void EntityPanel::CreateClassPath()
{
  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel( "Class Path" );

    m_TextBox = m_Enumerator->AddValue<Luna::Entity, std::string>( m_Selection, &Luna::Entity::GetEntityAssetPath, &Luna::Entity::SetEntityAssetPath );
    m_TextBox->AddBoundDataChangingListener( Inspect::ChangingSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanging ) );
    m_TextBox->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanged ) );

    Inspect::FileDialogButton* fileButton = m_Enumerator->AddFileDialogButton<Luna::Entity, std::string>( m_Selection, &Luna::Entity::GetEntityAssetPath, &Luna::Entity::SetEntityAssetPath );
    fileButton->AddBoundDataChangingListener( Inspect::ChangingSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanging ) );
    fileButton->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanged ) );

    Inspect::FileBrowserButton* browserButton = m_Enumerator->AddFileBrowserButton<Luna::Entity, std::string>( m_Selection, &Luna::Entity::GetEntityAssetPath, &Luna::Entity::SetEntityAssetPath );
    browserButton->AddBoundDataChangingListener( Inspect::ChangingSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanging ) );
    browserButton->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanged ) );

    const Finder::FinderSpec* spec = NULL;
    std::string specName;
    if ( Reflect::GetClass<Asset::EntityAsset>()->GetProperty( Asset::AssetProperties::FilterSpec, specName ) )
    {
      spec = Finder::GetFinderSpec( specName );
    }
    else if ( Reflect::GetClass<Asset::EntityAsset>()->GetProperty( Asset::AssetProperties::ModifierSpec, specName ) )
    {
      spec = Finder::GetFinderSpec( specName );
    }

    if ( spec )
    {
      fileButton->SetFilter( spec->GetDialogFilter() );
      browserButton->SetFilter( spec->GetDialogFilter() );
    }
    else
    {
      // There's a problem, better disable the button
      fileButton->SetEnabled( false );
      browserButton->SetEnabled( false );
    }

    Inspect::FilteredDropTarget* filteredDropTarget = new Inspect::FilteredDropTarget( spec );
    filteredDropTarget->AddDroppedListener( Inspect::FilteredDropTargetSignature::Delegate( this, &EntityPanel::OnEntityAssetDrop ) );
    m_TextBox->SetDropTarget( filteredDropTarget );
  }
  m_Enumerator->Pop();

}

void EntityPanel::CreateClassActions()
{
  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel( "Class Actions" );

    Inspect::Action* refreshButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnEntityAssetRefresh ) );
    refreshButton->SetIcon( "refresh_16.png" );
    refreshButton->SetToolTip( "Refresh" );

    bool singular = m_Selection.Size() == 1;

    Inspect::Action* lunaButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnEntityAssetEditAsset ) );
    lunaButton->SetIcon( "asset_editor_16.png" );
    lunaButton->SetToolTip( "Edit this entity class in Luna's Asset Editor" );

    Inspect::Action* mayaButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnEntityAssetEditArt ) );
    mayaButton->SetIcon( "maya_16.png" );
    mayaButton->SetEnabled( singular );
    mayaButton->SetToolTip( "Edit this entity class's art in Maya" );

    Inspect::Action* buildButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnEntityAssetBuild ) );
    buildButton->SetIcon( "build_16.png" );
    buildButton->SetToolTip( "Build this entity class's data into the game (Shift-click for build options)" );

    Inspect::Action* viewButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnEntityAssetView ) );
    viewButton->SetIcon( "view_16.png" );
    viewButton->SetEnabled( singular );
    viewButton->SetToolTip( "View this entity class in the appropriate viewer" );

    Inspect::Action* historyButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnEntityAssetRevisionHistory ) );
    historyButton->SetIcon( "p4_16.png" );
    historyButton->SetToolTip( "Display revision history for this file in Perforce." );

    bool buildable = true;
    bool viewable = true;

    V_EntitySmartPtr::const_iterator itr = m_Entities.begin();
    V_EntitySmartPtr::const_iterator end = m_Entities.end();
    for ( ; itr != end; ++itr )
    {
      // get the entity
      const Luna::Entity* entity = *itr;

      // get the entity class
      Asset::EntityAsset* entityClass = entity->GetClassSet()->GetEntityAsset();

      if (entityClass && entityClass->GetEngineType() != Asset::EngineTypes::Null)
      {
        buildable &= entityClass->IsBuildable();
        viewable &= entityClass->IsViewable();
      }
      else
      {
        buildable = false;
        viewable = false;
      }
    }

    buildButton->SetEnabled( buildable );
    viewButton->SetEnabled( singular && viewable );

  }
  m_Enumerator->Pop();
}

 

void EntityPanel::CreateChildImportExport()
{
  m_Enumerator->PushPanel("Child Import/Export");
  {
    m_Enumerator->PushContainer();
    {
      Inspect::Action* button1 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnExport< Luna::Transform, Content::Transform > ) );
      button1->SetText( "Export All" );

      Inspect::Action* button2 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnImport< Luna::Transform, Content::Transform > ) );
      button2->SetText( "Import All" );

      Inspect::Action* button3 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnSelectChildren< Luna::Transform > ) );
      button3->SetText( "Select All" );

      Inspect::Action* button4 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnExportToFile< Luna::Transform, Content::Transform > ) );
      button4->SetText( "Export To File" );

      Inspect::Action* button5 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnImportFromFile< Luna::Transform, Content::Transform > ) );
      button5->SetText( "Import From File" );
 
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      Inspect::Action* button1 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnExport< Luna::Entity, Asset::Entity > ) );
      button1->SetText( "Export Entities" );

      Inspect::Action* button2 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnImport< Luna::Entity, Asset::Entity > ) );
      button2->SetText( "Import Entities" );

      Inspect::Action* button3 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnSelectChildren< Luna::Entity > ) );
      button3->SetText( "Select Entities" );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      Inspect::Action* button1 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnExport< Luna::Volume, Content::Volume > ) );
      button1->SetText( "Export Weather Volumes" );

      Inspect::Action* button2 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnImport< Luna::Volume, Content::Volume > ) );
      button2->SetText( "Import Weather Volumes" );

      Inspect::Action* button3 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnSelectChildren< Luna::Volume > ) );
      button3->SetText( "Select Weather Volumes" );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      Inspect::Action* button1 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnExport< Luna::Light, Content::Light > ) );
      button1->SetText( "Export Lights" );

      Inspect::Action* button2 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnImport< Luna::Light, Content::Light > ) );
      button2->SetText( "Import Lights" );

      Inspect::Action* button3 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnSelectChildren< Luna::Light > ) );
      button3->SetText( "Select Lights" );
    }
    m_Enumerator->Pop();
  }
  m_Enumerator->Pop();
}

void EntityPanel::CreateLighting()
{
  m_LightingPanel = m_Enumerator->PushPanel("Lighting");
  bool hasBakedLighting = SelectionHasAttribute< Asset::BakedLightingAttribute >( m_Selection );
  {
    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Indirect Lighting" );
      m_Enumerator->AddCheckBox<Luna::Entity, bool >( m_Selection, &Luna::Entity::GetIndirectLightingEnabled, &Luna::Entity::SetIndirectLightingEnabled );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Texels Per Meter" );
      m_Enumerator->AddValue<Luna::Entity, f32>( m_Selection, &Luna::Entity::GetTexelsPerMeter, &Luna::Entity::SetTexelsPerMeter ); 
      m_Enumerator->AddCheckBox<Luna::Entity, bool >( m_Selection, &Luna::Entity::GetSetRuntimeLightmap, &Luna::Entity::SetSetRuntimeLightmap );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {     

      m_Enumerator->AddLabel("Lightmap Setting");
      Inspect::Choice* choice = m_Enumerator->AddChoice<Luna::Entity, int>(m_Selection, &Luna::Entity::GetLightmapSetting, &Luna::Entity::SetLightmapSetting);
      choice->SetDropDown( true );

      Inspect::V_Item items;
      {
        {
          std::ostringstream str;
          str << Asset::LightmapSettings::VertexOnly;
          items.push_back( Inspect::Item( "VertexOnly", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Asset::LightmapSettings::Lightmap4;
          items.push_back( Inspect::Item( "4x4", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Asset::LightmapSettings::Lightmap8;
          items.push_back( Inspect::Item( "8x8", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Asset::LightmapSettings::Lightmap16;
          items.push_back( Inspect::Item( "16x16", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Asset::LightmapSettings::Lightmap32;
          items.push_back( Inspect::Item( "32x32", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Asset::LightmapSettings::Lightmap64;
          items.push_back( Inspect::Item( "64x64", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Asset::LightmapSettings::Lightmap128;
          items.push_back( Inspect::Item( "128x128", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Asset::LightmapSettings::Lightmap256;
          items.push_back( Inspect::Item( "256x256", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Asset::LightmapSettings::Lightmap512;
          items.push_back( Inspect::Item( "512x512", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Asset::LightmapSettings::Lightmap1024;
          items.push_back( Inspect::Item( "1024x1024", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Asset::LightmapSettings::Lightmap2048;
          items.push_back( Inspect::Item( "2048x2048", str.str() ) );
        }
      }
      choice->SetItems( items );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Max Indirect Distance" );
      m_Enumerator->AddValue<Luna::Entity, float >( m_Selection, &Luna::Entity::GetMaxIndirectDistance, &Luna::Entity::SetMaxIndirectDistance );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Indirect Multiplier" );
      m_Enumerator->AddValue<Luna::Entity, float >( m_Selection, &Luna::Entity::GetIndirectMultiplier, &Luna::Entity::SetIndirectMultiplier );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel("Shadow Color");
      m_Enumerator->AddColorPicker<Luna::Entity, Math::Color3>( m_Selection, &Luna::Entity::GetShadowColor, &Luna::Entity::SetShadowColor );
    }
    m_Enumerator->Pop();

    // lightmap tweaking (should only display these if lightmapping is on?)
    Inspect::V_Item runtimeLightmapFormats;
    {
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapFormats::A8R8G8B8;
        runtimeLightmapFormats.push_back( Inspect::Item( "RGB32", str.str() ) );
      }
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapFormats::R5G6B5;
        runtimeLightmapFormats.push_back( Inspect::Item( "RGB16", str.str() ) );
      }
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapFormats::DXT1;
        runtimeLightmapFormats.push_back( Inspect::Item( "DXT1", str.str() ) );
      }
    }

    Asset::LightmapSetting maxSetting = Asset::LightmapSettings::VertexOnly;

    if (hasBakedLighting)
    {
      OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
      OS_SelectableDumbPtr::Iterator end = m_Selection.End();
      for ( ; itr != end; ++itr )
      {
        Luna::SceneNode* node = Reflect::ObjectCast< Luna::SceneNode >( *itr );
        if( node )
        {
          Content::SceneNodePtr contentNode = node->GetPackage< Content::SceneNode >();
          Attribute::AttributeViewer< Asset::BakedLightingAttribute > lighting( contentNode );
          if ( lighting.Valid() )
          {
            if (lighting->m_LightmapSetting > maxSetting)
            {
              maxSetting = lighting->m_LightmapSetting;
            }
          }
        }
      }
    }

    Inspect::V_Item runtimeLightmapSizes;

    if(maxSetting > Asset::LightmapSettings::VertexOnly)
    {
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapSizes::Lightmap4;
        runtimeLightmapSizes.push_back( Inspect::Item( "4x4", str.str() ) );
      }
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapSizes::Lightmap8;
        runtimeLightmapSizes.push_back( Inspect::Item( "8x8", str.str() ) );
      }
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapSizes::Lightmap16;
        runtimeLightmapSizes.push_back( Inspect::Item( "16x16", str.str() ) );
      }
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapSizes::Lightmap32;
        runtimeLightmapSizes.push_back( Inspect::Item( "32x32", str.str() ) );
      }
      if(maxSetting >= Asset::LightmapSettings::Lightmap64)
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapSizes::Lightmap64;
        runtimeLightmapSizes.push_back( Inspect::Item( "64x64", str.str() ) );
      }
      if(maxSetting >= Asset::LightmapSettings::Lightmap128)
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapSizes::Lightmap128;
        runtimeLightmapSizes.push_back( Inspect::Item( "128x128", str.str() ) );
      }
      if(maxSetting >= Asset::LightmapSettings::Lightmap256)
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapSizes::Lightmap256;
        runtimeLightmapSizes.push_back( Inspect::Item( "256x256", str.str() ) );
      }
      if(maxSetting >= Asset::LightmapSettings::Lightmap512)
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapSizes::Lightmap512;
        runtimeLightmapSizes.push_back( Inspect::Item( "512x512", str.str() ) );
      }
      if(maxSetting >= Asset::LightmapSettings::Lightmap1024)
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapSizes::Lightmap1024;
        runtimeLightmapSizes.push_back( Inspect::Item( "1024x1024", str.str() ) );
      }
      if(maxSetting >= Asset::LightmapSettings::Lightmap2048)
      {
        std::ostringstream str;
        str << Asset::RuntimeLightmapSizes::Lightmap2048;
        runtimeLightmapSizes.push_back( Inspect::Item( "2048x2048", str.str() ) );
      }
    }

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Runtime LM1 Format" );
      Inspect::Choice* choice = m_Enumerator->AddChoice<Luna::Entity, u32>( m_Selection, &Luna::Entity::GetRuntimeLM1Format, &Luna::Entity::SetRuntimeLM1Format );
      choice->SetDropDown( true );
      choice->SetItems( runtimeLightmapFormats );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Runtime LM1 Size" );
      Inspect::Choice* choice = m_Enumerator->AddChoice<Luna::Entity, u32>( m_Selection, &Luna::Entity::GetRuntimeLM1Size, &Luna::Entity::SetRuntimeLM1Size );
      choice->SetDropDown( true );
      choice->SetItems( runtimeLightmapSizes );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Runtime LM2 Format" );
      Inspect::Choice* choice = m_Enumerator->AddChoice<Luna::Entity, u32>( m_Selection, &Luna::Entity::GetRuntimeLM2Format, &Luna::Entity::SetRuntimeLM2Format );
      choice->SetDropDown( true );
      choice->SetItems( runtimeLightmapFormats );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Runtime LM2 Size" );
      Inspect::Choice* choice = m_Enumerator->AddChoice<Luna::Entity, u32>( m_Selection, &Luna::Entity::GetRuntimeLM2Size, &Luna::Entity::SetRuntimeLM2Size );
      choice->SetDropDown( true );
      choice->SetItems( runtimeLightmapSizes );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Bias Normals (Shrubs Only)" );
      m_Enumerator->AddCheckBox<Luna::Entity, bool >( m_Selection, &Luna::Entity::GetBiasNormals, &Luna::Entity::SetBiasNormals );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      Inspect::Action* button1 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnExport< Luna::Light, Content::Light > ) );
      button1->SetText( "Export Lights" );

      Inspect::Action* button2 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnImport< Luna::Light, Content::Light > ) );
      button2->SetText( "Import Lights" );

      Inspect::Action* button3 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnSelectChildren< Luna::Light > ) );
      button3->SetText( "Select Lights" );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      Inspect::Action* button1 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnViewUVs ) );
      button1->SetText( "View UVs" );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      Inspect::Action* button1 = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnAutoLightingSetup ) );
      button1->SetText( "Auto Lighting Setup" );
    }
    m_Enumerator->Pop();

  }
  m_Enumerator->Pop();

}

void EntityPanel::CreateCubemap()
{
  m_Enumerator->PushPanel( "CubeMap" );
  {
    m_Enumerator->PushContainer();
    {
      // Controls for changing the entity class
      m_Enumerator->AddLabel( "Class Path" );
      m_Enumerator->AddValue<Luna::Entity, std::string>( m_Selection, &Luna::Entity::GetCubeMapPath, &Luna::Entity::SetCubeMapPath );

      Inspect::FileDialogButton* fileButton = m_Enumerator->AddFileDialogButton<Luna::Entity, std::string>( m_Selection, &Luna::Entity::GetCubeMapPath, &Luna::Entity::SetCubeMapPath );
      fileButton->SetFilter( FinderSpecs::Asset::CUBEMAP_DECORATION.GetDialogFilter() );

      Inspect::FileBrowserButton* browserButton = m_Enumerator->AddFileBrowserButton<Luna::Entity, std::string>( m_Selection, &Luna::Entity::GetCubeMapPath, &Luna::Entity::SetCubeMapPath );
      browserButton->SetFilter( FinderSpecs::Asset::CUBEMAP_DECORATION.GetDialogFilter() );

      Inspect::Action* editButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnCubeMapEditAsset ) );
      editButton->SetIcon( "asset_editor_16.png" );
      editButton->SetToolTip( "Edit this cube map class in Luna's Asset Editor" );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel("Intensity");
      Inspect::Slider* slider = m_Enumerator->AddSlider<Luna::Entity, f32>( m_Selection, &Luna::Entity::GetCubeMapIntensity, &Luna::Entity::SetCubeMapIntensity );
      slider->SetRangeMin( 0.0f );
      slider->SetRangeMax( 4.0f );

      m_Enumerator->AddValue<Luna::Entity, f32>( m_Selection, &Luna::Entity::GetCubeMapIntensity, &Luna::Entity::SetCubeMapIntensity );
    }
    m_Enumerator->Pop();
  }
  m_Enumerator->Pop();
}

void EntityPanel::CreateShowFlags()
{
  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel( "Show Pointer" );
    m_Enumerator->AddCheckBox<Luna::Entity, bool>( m_Selection, 
                                              &Luna::Entity::IsPointerVisible, 
                                              &Luna::Entity::SetPointerVisible, false );
  }
  m_Enumerator->Pop();

  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel( "Show Bounds" );
    m_Enumerator->AddCheckBox<Luna::Entity, bool>( m_Selection, 
                                              &Luna::Entity::IsBoundsVisible, 
                                              &Luna::Entity::SetBoundsVisible, false );
  }
  m_Enumerator->Pop();

  m_Enumerator->PushContainer();
  {
    m_Enumerator->AddLabel( "Show Geometry" );
    m_Enumerator->AddCheckBox<Luna::Entity, bool>( m_Selection, 
                                              &Luna::Entity::IsGeometryVisible, 
                                              &Luna::Entity::SetGeometryVisible, false );
  }
  m_Enumerator->Pop();
}

void EntityPanel::CreateVisual()
{
  if ( !IsEngineType( m_Selection, Asset::EngineTypes::Moby ))
  {
    return; 
  }

  m_Enumerator->PushPanel( "Visual Overrides" );
  {
    bool hasVisualAttrib = SelectionHasAttribute< Asset::VisualAttribute >( m_Selection );

    m_Enumerator->PushContainer();
    {
      Inspect::Label* label = m_Enumerator->AddLabel( "Shadow Fadeout" );

      typedef Nocturnal::MemberProperty< EntityPanel, bool > EnablerPropertyType;
      EnablerPropertyType* enablerProp = new EnablerPropertyType
        (
        this,
        &EntityPanel::GetBoolOverride<Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_OverrideFarShadowFadeout>,
        &EntityPanel::SetBoolOverride<Asset::VisualOverrideAttribute, Inspect::CheckBox*, &EntityPanel::m_FarShadowFadeout, &Asset::VisualOverrideAttribute::m_OverrideFarShadowFadeout>
        );

      Inspect::CheckBox* checkBox = m_Enumerator->AddCheckBox<bool>( enablerProp );

      typedef Nocturnal::MemberProperty< EntityPanel, std::string > PropertyType;
      PropertyType* prop = new PropertyType
        (
        this,
        &EntityPanel::GetValue< bool, Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_FarShadowFadeout >,
        &EntityPanel::SetValue< bool, Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_FarShadowFadeout >
        );

      m_FarShadowFadeout = m_Enumerator->AddCheckBox< std::string >( prop );
      label->SetEnabled( hasVisualAttrib );
      checkBox->SetEnabled( hasVisualAttrib );
      m_FarShadowFadeoutEnabler->m_Items.push_back( m_FarShadowFadeout );
      m_FarShadowFadeoutEnabler->SetEnabled( hasVisualAttrib && GetBoolOverride<Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_OverrideFarShadowFadeout>() );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
  {
      Inspect::Label* label = m_Enumerator->AddLabel( "Casts Shadows" );

      typedef Nocturnal::MemberProperty< EntityPanel, bool > EnablerPropertyType;
      EnablerPropertyType* enablerProp = new EnablerPropertyType
        (
        this,
        &EntityPanel::GetBoolOverride<Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_OverrideCastsBakedShadows>,
        &EntityPanel::SetBoolOverride<Asset::VisualOverrideAttribute, Inspect::CheckBox*, &EntityPanel::m_CastsBakedShadows, &Asset::VisualOverrideAttribute::m_OverrideCastsBakedShadows>
        );

      Inspect::CheckBox* checkBox = m_Enumerator->AddCheckBox<bool>( enablerProp );

      typedef Nocturnal::MemberProperty< EntityPanel, std::string > PropertyType;
      PropertyType* prop = new PropertyType
        (
        this,
        &EntityPanel::GetValue< bool, Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_CastsBakedShadows >,
        &EntityPanel::SetValue< bool, Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_CastsBakedShadows >
        );

      m_CastsBakedShadows = m_Enumerator->AddCheckBox< std::string >( prop );
      label->SetEnabled( hasVisualAttrib );
      checkBox->SetEnabled( hasVisualAttrib );
      m_CastsBakedShadowsEnabler->m_Items.push_back( m_CastsBakedShadows );
      m_CastsBakedShadowsEnabler->SetEnabled( hasVisualAttrib && GetBoolOverride<Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_OverrideCastsBakedShadows>() );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      Inspect::Label* label = m_Enumerator->AddLabel( "Do Shadow Collisions" );

      typedef Nocturnal::MemberProperty< EntityPanel, bool > EnablerPropertyType;
      EnablerPropertyType* enablerProp = new EnablerPropertyType
        (
        this,
        &EntityPanel::GetBoolOverride<Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_OverrideDoBakedShadowCollisions>,
        &EntityPanel::SetBoolOverride<Asset::VisualOverrideAttribute, Inspect::CheckBox*, &EntityPanel::m_DoBakedShadowCollisions, &Asset::VisualOverrideAttribute::m_OverrideDoBakedShadowCollisions>
        );

      Inspect::CheckBox* checkBox = m_Enumerator->AddCheckBox<bool>( enablerProp );

      typedef Nocturnal::MemberProperty< EntityPanel, std::string > PropertyType;
      PropertyType* prop = new PropertyType
        (
        this,
        &EntityPanel::GetValue< bool, Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_DoBakedShadowCollisions >,
        &EntityPanel::SetValue< bool, Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_DoBakedShadowCollisions >
        );

      m_DoBakedShadowCollisions = m_Enumerator->AddCheckBox< std::string >( prop );
      label->SetEnabled( hasVisualAttrib );
      checkBox->SetEnabled( hasVisualAttrib );
      m_DoBakedShadowCollisionsEnabler->m_Items.push_back( m_DoBakedShadowCollisions );
      m_DoBakedShadowCollisionsEnabler->SetEnabled( hasVisualAttrib && GetBoolOverride<Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_OverrideDoBakedShadowCollisions>() );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      Inspect::Label* label = m_Enumerator->AddLabel( "Shadow AABB Extent" );

      typedef Nocturnal::MemberProperty< EntityPanel, bool > EnablerPropertyType;
      EnablerPropertyType* enablerProp = new EnablerPropertyType
        (
        this,
        &EntityPanel::GetBoolOverride<Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_OverrideBakedShadowAABBExt>,
        &EntityPanel::SetBoolOverride<Asset::VisualOverrideAttribute, Inspect::Value*, &EntityPanel::m_BakedShadowAABBExt, &Asset::VisualOverrideAttribute::m_OverrideBakedShadowAABBExt>
        );

      Inspect::CheckBox* checkBox = m_Enumerator->AddCheckBox<bool>( enablerProp );

      typedef Nocturnal::MemberProperty< EntityPanel, std::string > PropertyType;
      PropertyType* prop = new PropertyType
        ( 
        this,
        &EntityPanel::GetValue< float, Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_BakedShadowAABBExt >,
        &EntityPanel::SetValue< float, Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_BakedShadowAABBExt >
        );

      m_BakedShadowAABBExt = m_Enumerator->AddValue< std::string >( prop );
      label->SetEnabled( hasVisualAttrib );
      checkBox->SetEnabled( hasVisualAttrib );
      m_BakedShadowAABBExtEnabler->m_Items.push_back( m_BakedShadowAABBExt );
      m_BakedShadowAABBExtEnabler->SetEnabled( hasVisualAttrib && GetBoolOverride<Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_OverrideBakedShadowAABBExt>() );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      Inspect::Label* label = m_Enumerator->AddLabel( "Shadow Merge Groups" );

      typedef Nocturnal::MemberProperty< EntityPanel, bool > EnablerPropertyType;
      EnablerPropertyType* enablerProp = new EnablerPropertyType
        (
        this,
        &EntityPanel::GetBoolOverride<Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_OverrideBakedShadowMergeGroups>,
        &EntityPanel::SetBoolOverride<Asset::VisualOverrideAttribute, Inspect::Value*, &EntityPanel::m_BakedShadowMergeGroups, &Asset::VisualOverrideAttribute::m_OverrideBakedShadowMergeGroups>
        );

      Inspect::CheckBox* checkBox = m_Enumerator->AddCheckBox<bool>( enablerProp );

      typedef Nocturnal::MemberProperty< EntityPanel, std::string > PropertyType;
      PropertyType* prop = new PropertyType
        (
        this,
        &EntityPanel::GetValue< u32, Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_BakedShadowMergeGroups >,
        &EntityPanel::SetValue< u32, Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_BakedShadowMergeGroups >
        );

      m_BakedShadowMergeGroups = m_Enumerator->AddValue< std::string >( prop );
      label->SetEnabled( hasVisualAttrib );
      checkBox->SetEnabled( hasVisualAttrib );
      m_BakedShadowMergeGroupsEnabler->m_Items.push_back( m_BakedShadowMergeGroups );
      m_BakedShadowMergeGroupsEnabler->SetEnabled( hasVisualAttrib && GetBoolOverride<Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_OverrideBakedShadowMergeGroups>() );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      Inspect::Label* label = m_Enumerator->AddLabel( "Force High Res Shadows" );

      typedef Nocturnal::MemberProperty< EntityPanel, bool > EnablerPropertyType;
      EnablerPropertyType* enablerProp = new EnablerPropertyType
        (
        this,
        &EntityPanel::GetBoolOverride<Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_OverrideHighResShadowMap>,
        &EntityPanel::SetBoolOverride<Asset::VisualOverrideAttribute, Inspect::CheckBox*, &EntityPanel::m_HighResShadowMap, &Asset::VisualOverrideAttribute::m_OverrideHighResShadowMap>
        );

      Inspect::CheckBox* checkBox = m_Enumerator->AddCheckBox<bool>( enablerProp );

      typedef Nocturnal::MemberProperty< EntityPanel, std::string > PropertyType;
      PropertyType* prop = new PropertyType
        (
        this,
        &EntityPanel::GetValue< bool, Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_HighResShadowMap >,
        &EntityPanel::SetValue< bool, Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_HighResShadowMap >
        );

      m_HighResShadowMap = m_Enumerator->AddCheckBox< std::string >( prop );
      label->SetEnabled( hasVisualAttrib );
      checkBox->SetEnabled( hasVisualAttrib );
      m_HighResShadowMapEnabler->m_Items.push_back( m_HighResShadowMap );
      m_HighResShadowMapEnabler->SetEnabled( hasVisualAttrib && GetBoolOverride<Asset::VisualOverrideAttribute, &Asset::VisualOverrideAttribute::m_OverrideHighResShadowMap>() );
    }
    m_Enumerator->Pop();
  }
  m_Enumerator->Pop();
}

void EntityPanel::CreateGameplay()
{
  m_Enumerator->PushPanel( "Gameplay" );
  {
    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Draw Distance" );
      bool hasAttr = SelectionHasAttribute<Asset::DrawAttribute>(m_Selection);

      {
        typedef Nocturnal::MemberProperty<EntityPanel, bool> PropertyType;

        PropertyType* prop = new PropertyType
          (
          this,
          &EntityPanel::GetOverride<Asset::DrawAttribute>,
          &EntityPanel::SetOverride<Asset::DrawAttribute, Inspect::Value*, &EntityPanel::m_DrawDistance>
          );

        Inspect::CheckBox* checkBox = m_Enumerator->AddCheckBox<bool>( prop );
        checkBox->SetEnabled( hasAttr );
      }

      {
        typedef Nocturnal::MemberProperty<EntityPanel, std::string> PropertyType;

        PropertyType* prop = new PropertyType
          (
          this,
          &EntityPanel::GetValue<float, Asset::DrawAttribute, &Asset::DrawAttribute::m_DrawDistance>,
          &EntityPanel::SetValue<float, Asset::DrawAttribute, &Asset::DrawAttribute::m_DrawDistance>
          );

        m_DrawDistance = m_Enumerator->AddValue<std::string>( prop );
        m_DrawDistance->SetEnabled( hasAttr && GetOverride<Asset::DrawAttribute>() );
      }
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Update Distance" );
      bool hasAttr = SelectionHasAttribute<Asset::UpdateAttribute>(m_Selection);

      {
        typedef Nocturnal::MemberProperty<EntityPanel, bool> PropertyType;

        PropertyType* prop = new PropertyType
          (
          this,
          &EntityPanel::GetOverride<Asset::UpdateAttribute>,
          &EntityPanel::SetOverride<Asset::UpdateAttribute, Inspect::Value*, &EntityPanel::m_UpdateDistance>
          );

        Inspect::CheckBox* checkBox = m_Enumerator->AddCheckBox<bool>( prop );
        checkBox->SetEnabled( hasAttr );
      }

      {
        typedef Nocturnal::MemberProperty<EntityPanel, std::string> PropertyType;

        PropertyType* prop = new PropertyType
          (
          this,
          &EntityPanel::GetValue<float, Asset::UpdateAttribute, &Asset::UpdateAttribute::m_UpdateDistance>,
          &EntityPanel::SetValue<float, Asset::UpdateAttribute, &Asset::UpdateAttribute::m_UpdateDistance>
          );

        m_UpdateDistance = m_Enumerator->AddValue<std::string>( prop );
        m_UpdateDistance->SetEnabled( hasAttr && GetOverride<Asset::UpdateAttribute>() );
      }
    }
    m_Enumerator->Pop();

    bool hasAttr = SelectionHasAttribute<Asset::SegmentAttribute>(m_Selection);

    m_Enumerator->PushContainer();
    {
      typedef Nocturnal::MemberProperty<EntityPanel, bool> PropertyType;

      PropertyType* prop = new PropertyType
        (
        this,
        &EntityPanel::GetOverride<Asset::SegmentAttribute>,
        &EntityPanel::SetOverride<Asset::SegmentAttribute, ControlEnabler*, &EntityPanel::m_SegmentEnabler>
        );

      Inspect::Label* label = m_Enumerator->AddLabel( "Segment Override" );
      Inspect::CheckBox* checkBox = m_Enumerator->AddCheckBox<bool>( prop );
      checkBox->SetEnabled( hasAttr );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      typedef Nocturnal::MemberProperty<EntityPanel, std::string> PropertyType;

      PropertyType* prop = new PropertyType
        (
        this,
        &EntityPanel::GetValue<bool, Asset::SegmentAttribute, &Asset::SegmentAttribute::m_MajorSegment>,
        &EntityPanel::SetValue<bool, Asset::SegmentAttribute, &Asset::SegmentAttribute::m_MajorSegment>
        );

      m_SegmentEnabler->m_Items.push_back( m_Enumerator->AddLabel( "Major" ) );
      m_SegmentEnabler->m_Items.push_back( m_Enumerator->AddCheckBox<std::string>( prop ) );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      typedef Nocturnal::MemberProperty<EntityPanel, std::string> PropertyType;

      PropertyType* prop = new PropertyType
        (
        this,
        &EntityPanel::GetValue<bool, Asset::SegmentAttribute, &Asset::SegmentAttribute::m_MinorSegment>,
        &EntityPanel::SetValue<bool, Asset::SegmentAttribute, &Asset::SegmentAttribute::m_MinorSegment>
        );

      m_SegmentEnabler->m_Items.push_back( m_Enumerator->AddLabel( "Minor" ) );
      m_SegmentEnabler->m_Items.push_back( m_Enumerator->AddCheckBox<std::string>( prop ) );
    }
    m_Enumerator->Pop();

    m_Enumerator->PushContainer();
    {
      typedef Nocturnal::MemberProperty<EntityPanel, std::string> PropertyType;

      PropertyType* prop = new PropertyType
        (
        this,
        &EntityPanel::GetValue<i32, Asset::SegmentAttribute, (i32 Asset::SegmentAttribute::*)&Asset::SegmentAttribute::m_RespawnType>,
        &EntityPanel::SetValue<i32, Asset::SegmentAttribute, (i32 Asset::SegmentAttribute::*)&Asset::SegmentAttribute::m_RespawnType>
        );

      m_SegmentEnabler->m_Items.push_back( m_Enumerator->AddLabel( "Respawn" ) );
      m_SegmentEnabler->m_Items.push_back( m_Enumerator->AddChoice<std::string>( Reflect::Registry::GetInstance()->GetEnumeration( "RespawnType" ), prop ) );
    }
    m_Enumerator->Pop();

    m_SegmentEnabler->SetEnabled( hasAttr && GetOverride<Asset::SegmentAttribute>() );

    CreateRuntimeData();
  }
  m_Enumerator->Pop();
}

void EntityPanel::CreateShaderGroups()
{
  m_Enumerator->PushPanel( "Shader Group" );
  {
    m_Enumerator->PushContainer();
    {
      Inspect::Label* label = m_Enumerator->AddLabel( "Shader Group" );
      Attribute::AttributeViewer< Asset::ShaderGroupAttribute > viewer;
      bool hasShaderGroupAttrib = SelectionHasSameAttribute< Asset::ShaderGroupAttribute >( m_Selection, viewer );

      typedef Nocturnal::MemberProperty< EntityPanel, bool > EnablerPropertyType;
      EnablerPropertyType* enablerProp = new EnablerPropertyType
        (
        this,
        &EntityPanel::GetOverride<Asset::ShaderGroupOverrideAttribute>,
        &EntityPanel::SetOverride<Asset::ShaderGroupOverrideAttribute, Inspect::Choice*, &EntityPanel::m_ShaderGroup>
        );

      Inspect::CheckBox* checkBox = m_Enumerator->AddCheckBox<bool>( enablerProp );

      typedef Nocturnal::MemberProperty< EntityPanel, std::string > PropertyType;
      PropertyType* prop = new PropertyType
        ( 
        this,
        &EntityPanel::GetValue< tuid, Asset::ShaderGroupOverrideAttribute, &Asset::ShaderGroupOverrideAttribute::m_ShaderGroup>,
        &EntityPanel::SetValue< tuid, Asset::ShaderGroupOverrideAttribute, &Asset::ShaderGroupOverrideAttribute::m_ShaderGroup>
        );

      m_ShaderGroup = m_Enumerator->AddChoice< std::string >( prop );
      m_ShaderGroup->SetDropDown( true );

      if ( hasShaderGroupAttrib && viewer.Valid() )
      {
        Inspect::V_Item choices;
        choices.push_back( Inspect::Item( "<NONE>", "0" ) ); // TUID::Null
        for each ( const Asset::ShaderGroupPtr& group in viewer->m_ShaderGroups )
        {
          std::stringstream idStr;
          idStr << group->m_ID;
          choices.push_back( Inspect::Item ( group->m_Name, idStr.str() ) );
        }
        m_ShaderGroup->SetItems( choices );
      }

      const bool enable = hasShaderGroupAttrib && viewer.Valid() && viewer->m_ShaderGroups.size() > 0;
      label->SetEnabled( enable );
      checkBox->SetEnabled( enable );
      m_ShaderGroupEnabler->m_Items.push_back( m_ShaderGroup );
      m_ShaderGroupEnabler->SetEnabled( enable && GetOverride< Asset::ShaderGroupOverrideAttribute >() );
    }
    m_Enumerator->Pop();
  }
  m_Enumerator->Pop();
}

void EntityPanel::CreateRuntimeData()
{
  RectifyRuntimeData();
  
  m_Enumerator->PushPanel("Runtime Class");
  {
    // specialize button
    //
    m_Enumerator->PushContainer(); 
    {
      m_Enumerator->AddLabel( "Specialize Runtime Class" ); 
      m_SpecializeCheckbox = m_Enumerator->AddCheckBox<Luna::Entity, bool>(m_Selection, &Luna::Entity::IsSpecialized, &Luna::Entity::SetSpecialized);
      m_SpecializeCheckbox->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &EntityPanel::OnSpecializedChanged ) );

    }
    m_Enumerator->Pop(); 

    // Controls for changing the runtime class 
    // (only enabled if all instances have runtime data)
    //
    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Runtime Class" );

      m_RuntimeClassesChoice = m_Enumerator->AddChoice< Luna::Entity, std::string >( m_Selection, &Luna::Entity::GetRuntimeClassName, &Luna::Entity::SetRuntimeClassName );
      m_RuntimeClassesChoice->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &EntityPanel::OnRuntimeClassChanged ) );

      m_RuntimeClassesButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnRuntimeClassButton ) );
      m_RuntimeClassesButton->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &EntityPanel::OnRuntimeClassChanged ) );

      RefreshClassPicker(); 
    }
    m_Enumerator->Pop();

    m_RuntimeDataPanel = m_Enumerator->PushPanel("Runtime Data");
    {
      m_SymbolInterpreter = m_Enumerator->CreateInterpreter<Inspect::SymbolInterpreter> ( m_RuntimeDataPanel );

      RefreshInstances(false);
    }
    m_Enumerator->Pop();
  }
  m_Enumerator->Pop(); 

  RefreshSpecializeCheckbox(); 
}

void EntityPanel::CreateCollision()
{
  if ( !IsEngineType( m_Selection, Asset::EngineTypes::Tie )
    && !IsEngineType( m_Selection, Asset::EngineTypes::Moby ))
  {
    return; 
  }

  m_CollisionReflectInterpreter = m_Enumerator->CreateInterpreter< Inspect::ReflectInterpreter >( this );

  std::vector< Reflect::Element* > instances;
  instances.reserve( m_Selection.Size() );

  OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator end = m_Selection.End();
  for ( ; itr != end; ++itr )
  {
    Persistent* persistent = Reflect::ObjectCast< Persistent >( *itr );
    if ( persistent )
    {
      Attribute::AttributeCollection* collection = persistent->GetPackage< Attribute::AttributeCollection >();
      if ( collection )
      {
        Asset::InstanceCollisionAttributePtr collision = collection->GetAttribute< Asset::InstanceCollisionAttribute >();

        NOC_ASSERT( collision );

        instances.push_back( collision );
      }
    }
  }

  if ( !instances.empty() )
  {
    m_CollisionReflectInterpreter->Interpret( instances, -1, Attribute::AttributeFlags::AttributeEnabledMember );
  }
}

void EntityPanel::CreateAttenuation()
{
  // Selection must contain only ties that reference foliage for this panel
  // to be shown.
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.End();
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *selItr );
    if ( entity && entity->GetClassSet() && entity->GetClassSet()->GetEntityAsset() )
    {
      const Asset::EntityAsset* entityClass = entity->GetClassSet()->GetEntityAsset();
      if ( entityClass->GetEngineType() != Asset::EngineTypes::Tie )
      {
        return;
      }
      else
      {
        if ( !( Attribute::AttributeViewer< Asset::FoliageAttribute >( entityClass, false ).Valid() ) )
        {
          return;
        }
      }
    }
    else
    {
      return;
    }
  }

  m_Enumerator->PushPanel( "Foliage" );
  {
    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Attenuation" );

      {
        typedef Nocturnal::MemberProperty<EntityPanel, bool> PropertyType;

        PropertyType* prop = new PropertyType
          (
          this,
          &EntityPanel::GetOverride<Asset::FoliageOverrideAttribute>,
          &EntityPanel::SetOverride<Asset::FoliageOverrideAttribute, Inspect::Value*, &EntityPanel::m_Attenuation>
          );

        Inspect::CheckBox* checkBox = m_Enumerator->AddCheckBox<bool>( prop );
        checkBox->SetEnabled( true );
      }

      {
        typedef Nocturnal::MemberProperty<EntityPanel, std::string> PropertyType;

        PropertyType* prop = new PropertyType
          (
          this,
          &EntityPanel::GetValue<float, Asset::FoliageOverrideAttribute, &Asset::FoliageOverrideAttribute::m_Attenuation>,
          &EntityPanel::SetValue<float, Asset::FoliageOverrideAttribute, &Asset::FoliageOverrideAttribute::m_Attenuation>
          );

        m_Attenuation = m_Enumerator->AddValue<std::string>( prop );
        m_Attenuation->SetEnabled( GetOverride<Asset::FoliageOverrideAttribute>() );
      }
    }
    m_Enumerator->Pop();

  }
  m_Enumerator->Pop();
}

void EntityPanel::RectifyRuntimeData()
{
  __super::RectifyRuntimeData(); 

  m_AllEntitiesSpecialized = true; 

  OS_SelectableDumbPtr::Iterator selItr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.End();
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::Entity* lunaEntity = Reflect::ObjectCast< Luna::Entity >( *selItr );
    if ( !lunaEntity )
    {
      continue; 
    }

    Asset::EntityPtr entity = lunaEntity->GetPackage<Asset::Entity>(); 

    Attribute::AttributeViewer< Asset::UpdateInstanceAttribute > updateInstance ( entity );

    m_AllEntitiesSpecialized &= updateInstance.Valid() && updateInstance->IsSpecialized();
  }

  // entities which have valid UpdateClassAttributes will have runtime data 
  // if they have any valid classes at all. 
  //
  // therefore if all instances have runtime data, enable the specialized checkbox
  // 
  m_EnableSpecializeCheckbox = m_AllInstancesHaveRuntimeData; 

  // then specialization controls the rest of the UI
  // 
  m_RecurseSelectableClasses = m_AllEntitiesSpecialized; 
  m_EnableClassBrowser       = m_AllEntitiesSpecialized; 
  m_EnableSymbolInterpreter  = m_AllEntitiesSpecialized; 
}  

void EntityPanel::Create()
{
  CreateApplicationType();
  CreateEngineType(); 

  CreateClassPath(); 
  CreateClassActions(); 

  CreateShowFlags(); 
  CreateAppearanceFlags();

  CreateLighting(); 
  CreateCubemap(); 
  CreateGameplay(); 

  CreateShaderGroups(); 
  CreateVisual();
  CreateCollision();
  CreateAttenuation();
  CreateChildImportExport();

  Inspect::Panel::Create();
}


///////////////////////////////////////////////////////////////////////////////
// Validation function for the controls that change the Entity Class field.
// Returns true if the new value for the Entity Class field can be resolved to
// a file TUID.
//
bool EntityPanel::OnEntityAssetChanging( const Inspect::ChangingArgs& args )
{
  bool result = false;

  std::string newValue;
  Reflect::Serializer::GetValue(args.m_NewValue, newValue);

  try
  {
    // Make sure the file exists on disc
    if ( FileSystem::Exists( newValue ) )
    {
      // Make sure the file can be resolved to a TUID
      if ( File::GlobalManager().GetID( newValue ) != TUID::Null )
      {
        // Make sure the file has the entity class extension
        std::string ext;
        FinderSpecs::Asset::ENTITY_DECORATION.Modify( ext );
        if ( FileSystem::HasExtension( newValue, ext ) )
        {
          result = true;
        }
      }
    }
  }
  catch ( const Nocturnal::Exception& )
  {
    result = false;
  }

  if ( !result )
  {
    // Message to the user that the value is not correct.
    wxMessageBox( "Invalid Entity Class specified!", "Error", wxOK | wxCENTER | wxICON_ERROR, GetWindow() );
  }

  return result;
}

void EntityPanel::RefreshSpecializeCheckbox()
{
  m_SpecializeCheckbox->SetEnabled(m_EnableSpecializeCheckbox); 
}

void EntityPanel::OnSpecializedChanged( const Inspect::ChangeArgs& args )
{
  RectifyRuntimeData(); 

  RefreshClassPicker(); 

  RefreshInstances(true); 

  RefreshSpecializeCheckbox(); 
}

void EntityPanel::OnEntityAssetChanged( const Inspect::ChangeArgs& args )
{
  // this code should be in a function
  // 
  RectifyRuntimeData(); 

  RefreshClassPicker();

  RefreshInstances( true );

  RefreshSpecializeCheckbox();
}

void EntityPanel::OnEntityAssetRefresh( Inspect::Button* button )
{
  Luna::Scene* scene = NULL;

  // when we refresh, reload the common class set information in case
  // we did something like reexport an art class, while luna is still opened
  std::set< EntityAssetSet* > reloadQueue;     // entities we want to reload

  S_string files;

  OS_SelectableDumbPtr::Iterator selectionIter = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selectionEnd = m_Selection.End();
  for (; selectionIter != selectionEnd; ++selectionIter )
  {
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *selectionIter );

    if ( !scene )
    {
      Luna::SceneNode* node = Reflect::ObjectCast< Luna::SceneNode >( *selectionIter );
      scene = node->GetScene();
    }

    if( entity->GetClassSet( ) )
    {
      reloadQueue.insert( entity->GetClassSet( ) );
    }

    if (entity->IsGeometryVisible())
    {
      for ( int i=0; i<GeometryModes::Count; i++ )
      {
        Luna::Scene* nestedScene = entity->GetNestedScene(entity->GetScene()->GetView()->GetGeometryMode());

        if (nestedScene)
        {
          if ( files.insert( nestedScene->GetFullPath() ).second )
          {
            // Only reload the scene if we haven't already done so during this iteration.
            nestedScene->Reload();
          }
        }
      }
    }
  }

  std::set< EntityAssetSet* >::iterator itr = reloadQueue.begin();
  std::set< EntityAssetSet* >::iterator end = reloadQueue.end();

  while( itr != end )
  {
    EntityAssetSet* entClassSet = ObjectCast< EntityAssetSet >( *itr );

    if( entClassSet )
    {
      Asset::AssetClass::InvalidateCache( entClassSet->GetEntityAssetID() );
      entClassSet->LoadAssetClass( );
    }

    itr++;
  }

  if (scene)
  {
    scene->Execute(false);
  }
}

void EntityPanel::OnEntityAssetEditAsset( Inspect::Button* button )
{
  S_string files;
  OS_SelectableDumbPtr::Iterator selectionIter = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selectionEnd = m_Selection.End();
  for ( ; selectionIter != selectionEnd; ++selectionIter )
  {
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *selectionIter );
    if ( entity )
    {
      std::string fileToEdit = entity->GetEntityAssetPath();
      if ( !fileToEdit.empty() )
      {
        files.insert( fileToEdit );
      }
    }
  }

  S_string::const_iterator fileItr = files.begin();
  S_string::const_iterator fileEnd = files.end();
  for ( ; fileItr != fileEnd; ++fileItr )
  {
    SessionManager::GetInstance()->Edit( *fileItr );
  }
}

void EntityPanel::OnEntityAssetBuild( Inspect::Button* button )
{
  S_tuid assets;

  OS_SelectableDumbPtr::Iterator selectionIter = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selectionEnd = m_Selection.End();
  for (; selectionIter != selectionEnd; ++selectionIter )
  {
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *selectionIter );
    assets.insert( entity->GetClassSet()->GetEntityAssetID() );
  }

  bool showOptions = wxIsShiftDown();
  SessionManager::GetInstance()->SaveAllOpenDocuments();
  Luna::BuildAssets( assets, SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene ), NULL, showOptions );
}

void EntityPanel::OnEntityAssetEditArt( Inspect::Button* button )
{
  if ( m_Selection.Size() == 1 )
  {
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( m_Selection.Front() );

    // connect if necessary and open the specified AssetClass's art file in maya
    if( RemoteConstruct::EstablishConnection() )
    {
      RPC::EntityEditParam param;
      {
        entity->Pack();
        tuid id = entity->GetClassSet()->GetEntityAssetID();
        memcpy( &param.m_EntityAsset, &id, sizeof(param.m_EntityAsset) );
        memcpy( &param.m_Transform, &entity->GetGlobalTransform(), sizeof(param.m_Transform) );
      }
      RemoteConstruct::g_ConstructionTool->OpenArtFile( &param );
    }
  }
}

void EntityPanel::OnEntityAssetView( Inspect::Button* button )
{
  if ( m_Selection.Size() == 1 )
  {
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( m_Selection.Front() );

    SessionManager::GetInstance()->GiveViewerControl( NULL );

    SceneEditor* editor = static_cast< SceneEditor* >( SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene ) );
    
    editor->GetRemoteScene()->EnableCamera( false );
    editor->GetRemoteScene()->Enable( true, true );

    if ( entity->GetClassSet()->GetEntityAsset()->GetEngineType() == Asset::EngineTypes::Moby )
    {
      editor->GetRemoteScene()->SetMode( RPC::LunaViewModes::MobyView );
    }
    else
    {
      editor->GetRemoteScene()->SetMode( RPC::LunaViewModes::Normal );
    }

    bool showOptions = wxIsShiftDown();
    SessionManager::GetInstance()->SaveAllOpenDocuments();
    Luna::ViewAsset( entity->GetClassSet()->GetEntityAssetID(), editor, NULL, showOptions );
  }
}

void EntityPanel::OnEntityAssetRevisionHistory( Inspect::Button* button )
{
  S_string files;
  SceneEditor* editor = static_cast< SceneEditor* >( SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene ) );
  OS_SelectableDumbPtr::Iterator selectionItr = m_Selection.Begin();
  OS_SelectableDumbPtr::Iterator selectionEnd = m_Selection.End();
  for ( ; selectionItr != selectionEnd; ++selectionItr )
  {
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *selectionItr );
    std::string path = entity->GetEntityAssetPath();
    if ( !path.empty() )
    {
      files.insert( path );
    }
  }

  S_string::const_iterator pathItr = files.begin();
  S_string::const_iterator pathEnd = files.end();
  for ( ; pathItr != pathEnd; ++pathItr )
  {
    editor->RevisionHistory( *pathItr );
  }
}

void EntityPanel::OnEntityAssetDrop( const Inspect::FilteredDropTargetArgs& args )
{
  if ( args.m_Paths.size() )
  {
    m_TextBox->WriteData( args.m_Paths[ 0 ] );
  }
}

void EntityPanel::OnCubeMapEditAsset( Inspect::Button* button )
{
  if ( !m_Selection.Empty() )
  {
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( m_Selection.Front() );
    if ( entity )
    {
      Asset::Entity* contentEntity = entity->GetPackage< Asset::Entity >();
      if( contentEntity )
      {
        Attribute::AttributeViewer< Asset::CubeMapAttribute > cubeMap(contentEntity);
        if( cubeMap.Valid() )
        {
          std::string cubeMapPath = cubeMap->GetFilePath();
          if ( !cubeMapPath.empty() )
          {
            SessionManager::GetInstance()->Edit( cubeMapPath );
          }
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when one of the selected entity class sets is reloaed.  Updates
// the baked lighting panel to be enabled or disabled accordingly.
// 
void EntityPanel::EntityAssetReloaded( const EntityAssetSetChangeArgs& args )
{
  bool hasBakedLighting = SelectionHasAttribute< Asset::BakedLightingAttribute >( m_Selection );
  m_LightingPanel->SetEnabled( hasBakedLighting );

  bool hasSegment = SelectionHasAttribute< Asset::SegmentAttribute >( m_Selection );
  m_SegmentEnabler->SetEnabled( hasSegment );

  Read();
}

void EntityPanel::OnViewUVs( Inspect::Button* button )
{
  SceneEditor* editor = static_cast<SceneEditor*>( SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene ) );

  if( editor )
  {
    S_tuid assets;

    OS_SelectableDumbPtr::Iterator selectionIter = m_Selection.Begin();
    OS_SelectableDumbPtr::Iterator selectionEnd = m_Selection.End();
    for (; selectionIter != selectionEnd; ++selectionIter )
    {
      Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *selectionIter );
      assets.insert( entity->GetClassSet()->GetEntityAssetID() );
    }

    S_tuid::iterator itor = assets.begin();
    S_tuid::iterator end  = assets.end();

    for( ; itor != end; ++itor )
    {
      LightmapUVViewer* viewer = new LightmapUVViewer( editor, *itor, "Lightmap UV Viewer", 100, 100, 600, 650 );
      viewer->Enable();
      viewer->CentreOnScreen();
      viewer->Show();
    }
  }
}

void EntityPanel::OnAutoLightingSetup( Inspect::Button* button )
{
  SceneEditor* editor = static_cast<SceneEditor*>( SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene ) );

  if( editor )
  {
    S_tuid assets;

    OS_SelectableDumbPtr::Iterator selectionIter = m_Selection.Begin();
    OS_SelectableDumbPtr::Iterator selectionEnd = m_Selection.End();
    for (; selectionIter != selectionEnd; ++selectionIter )
    {
      //make sure we don't get any mobys up in there
      Luna::Entity* lunaEntity = Reflect::ObjectCast< Luna::Entity >( *selectionIter );
      Asset::Entity* assetEntity = lunaEntity->GetPackage< Asset::Entity >();
      NOC_ASSERT( assetEntity );
      Asset::EntityAssetPtr entityClass = assetEntity->GetEntityAsset();
      NOC_ASSERT( entityClass );

      if( entityClass->GetEngineType() != Asset::EngineTypes::Moby )
        assets.insert( lunaEntity->GetClassSet()->GetEntityAssetID() );
    }

    Luna::ExportAssets( assets, NULL, false, false, true );
  }
}

