#include "Precompile.h"
#include "LightingTool.h"

#include "Curve.h"
#include "InstanceType.h"
#include "InstanceSet.h"
#include "LightingEnvironment.h"
#include "Light.h"
#include "Pick.h"
#include "Scene.h"
#include "SceneEditor.h"
#include "SceneManager.h"
#include "TranslateManipulator.h"
#include "Zone.h"

#include "Asset/BakedLightingAttribute.h"
#include "AssetBuilder/AssetBuilder.h"
#include "Attribute/AttributeHandle.h"
#include "Console/Console.h"
#include "Content/Camera.h"
#include "Dependencies/Dependencies.h"
#include "FileSystem/FileSystem.h"
#include "Inspect/Action.h"
#include "LightingJob/LightingJob.h"
#include "Editor/SessionManager.h"
#include "Task/Build.h"
#include "MentalRayExport/MentalRayExport.h"


using namespace Math;
using namespace Luna;

LUNA_DEFINE_TYPE(Luna::LightingTool);

bool LightingTool::RenderSelectedOnly = false;
u32 LightingTool::PreviewWidth = 800;
u32 LightingTool::PreviewHeight = 800;

void LightingTool::InitializeType()
{
  Reflect::RegisterClass< Luna::LightingTool >( "Luna::LightingTool" );
}

void LightingTool::CleanupType()
{
  Reflect::UnregisterClass< Luna::LightingTool >();
}

LightingTool::LightingTool(Luna::Scene *scene, Luna::Enumerator *enumerator)
: Luna::SceneTool( scene, enumerator )
{  
  Initialize();
}

LightingTool::~LightingTool()
{
}

bool LightingTool::AllowSelection()
{
  return true;
}


void LightingTool::CreateProperties()
{
  __super::CreateProperties();

  m_Enumerator->PushPanel( "Lighting", true );
  {
    m_Enumerator->PushContainer();
    {
      Inspect::Action* renderButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate ( &OnButtonPreviewRender ) );
      renderButton->SetText( "Render Preview" );
      renderButton->SetClientData( this );
    }
    m_Enumerator->Pop();
    m_Enumerator->PushContainer();
    {
      m_Enumerator->AddLabel( "Render Selected Only" );   
      m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::LightingTool, bool> (this, &LightingTool::GetRenderSelected, &LightingTool::SetRenderSelected) );
    }
    m_Enumerator->Pop();
  }
  m_Enumerator->Pop();
}

bool LightingTool::ValidateSelection( OS_SelectableDumbPtr& items )
{
  OS_SelectableDumbPtr result;

  OS_SelectableDumbPtr::Iterator itr = items.Begin();
  OS_SelectableDumbPtr::Iterator end = items.End();
  for( ; itr != end; ++itr )
  {
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *itr );

    if ( !entity )
    {
      continue;
    }

    result.Append( entity );
  }

  items = result;

  return !items.Empty();
}

bool LightingTool::Initialize()
{
  Luna::LightingEnvironment::GetDefaultLightingEnvironment(m_Scene);
  return __super::Initialize();
}





void LightingTool::RenderSelected( Content::Scene& exportScene, S_tuid& assetClasses )
{
  // add all selected entities (with valid baked lighting attrib) and non-realtime lights to the Content::Scene
  OS_SelectableDumbPtr selection = m_Scene->GetSelection().GetItems();

  OS_SelectableDumbPtr::Iterator itr = selection.Begin();
  OS_SelectableDumbPtr::Iterator end = selection.End();
  for ( ; itr != end; ++itr )
  {
    if( (*itr)->HasType( Reflect::GetType<Luna::Entity>() ) )
    {
      Luna::Entity* entity = Reflect::DangerousCast< Luna::Entity >( *itr );
      Asset::Entity* contentEntity = entity->GetPackage< Asset::Entity >();
      Attribute::AttributeViewer< Asset::BakedLightingAttribute > lighting( contentEntity );
      if( lighting.Valid() )
      {
        entity->Pack();
        contentEntity->m_Selected = false;
        exportScene.Add( contentEntity );
        assetClasses.insert( contentEntity->GetEntityAssetID() );
      }
    }
    else if( (*itr)->HasType( Reflect::GetType<Luna::Light>() ) )
    {
      Luna::Light* light = Reflect::DangerousCast< Luna::Light >( *itr );
      Content::LightPtr contentLight = light->GetPackage< Content::Light >();
      if ( contentLight->m_RenderType != Content::LightRenderTypes::RealTime )
      {
        light->Pack();
        exportScene.Add( contentLight );    
      }
    }
  }
}

void LightingTool::RenderAllVisible( Content::Scene& exportScene, S_tuid& assetClasses )
{
  Luna::SceneManager* manager = m_Scene->GetManager();
  Luna::Scene* worldScene = manager->GetRootScene();

  V_ZoneDumbPtr zones;
  worldScene->GetAll< Zone >( zones );

  V_ZoneDumbPtr::iterator itor = zones.begin();
  V_ZoneDumbPtr::iterator end  = zones.end();
  for( ; itor != end; ++itor )
  {
    Luna::Scene* zoneScene = manager->GetScene( (*itor)->GetFileID() );
    if( zoneScene )
    {
      V_InstanceDumbPtr instances;
      zoneScene->GetAll< Luna::Instance >( instances );

      V_InstanceDumbPtr::iterator instanceItor = instances.begin();
      V_InstanceDumbPtr::iterator instanceEnd  = instances.end();
      for( ; instanceItor != instanceEnd; ++instanceItor )
      {
        if( !(*instanceItor)->IsVisible() )
          continue;

        Content::SceneNodePtr contentNode = (*instanceItor)->GetPackage< Content::SceneNode >();
        if( contentNode.ReferencesObject() )
        {
          if( LightingJob::IsValidRenderObject( contentNode ) )
          {
            if( contentNode->HasType( Reflect::GetType< Asset::Entity >() ) )
            {
              Asset::EntityPtr entity = Reflect::DangerousCast<Asset::Entity>( contentNode );
              assetClasses.insert( entity->GetEntityAssetID() );
            }

            (*instanceItor)->Pack();
            contentNode->m_Selected = false;
            exportScene.Add( contentNode );
          }
        }
      }
    }
  }
}

void LightingTool::OnButtonPreviewRender( Inspect::Button* button )
{
  Content::Scene exportScene;
  S_tuid assetClasses;

  LightingToolPtr thisTool = Reflect::DangerousCast< Luna::LightingTool >( button->GetClientData() );

  // if it's not in "Render Selected Only" mode, we want to render all visible renderable items
  if( RenderSelectedOnly )
  {
    thisTool->RenderSelected( exportScene, assetClasses );
  }
  else
  {
    thisTool->RenderAllVisible( exportScene, assetClasses);
  }
  
  // add the sky
  const Luna::LightingEnvironment* env = Luna::LightingEnvironment::GetDefaultLightingEnvironment( thisTool->m_Scene );

 if( env )
   exportScene.Add( env->GetPackage< Content::LightingEnvironment >() );

  // make a Content camera from this luna camera
  Luna::Camera* camera = thisTool->m_Scene->GetView()->GetCamera();
  Content::CameraPtr contentCamera = new Content::Camera();
  contentCamera->m_View = camera->GetView();
  contentCamera->m_FOV = Luna::Camera::FieldOfView;
  contentCamera->m_Width  = camera->GetWidth();
  contentCamera->m_Height = camera->GetHeight();
 
  // add the Content Camera
  exportScene.Add( contentCamera );

  // update the Content Scene because of all the newly added objects
  exportScene.Update();

  // make sure all the assets are up-to-date
  SessionManager::GetInstance()->SaveAllOpenDocuments();
  Luna::BuildAssets( assetClasses, thisTool->m_Scene->GetManager()->GetEditor(), NULL, false, true );

  tuid randomPath;
  TUID::Generate( randomPath );
  std::ostringstream exportPath;
  exportPath << Finder::ProjectTemp() << "/renders/" << randomPath << "/" << randomPath << ".mi";

  MentalRayExport::MentalRayExportVisitor exportVisitor( exportPath.str() );

  try
  {
    exportVisitor.ExportAsScene( exportScene ); 
    exportVisitor.LaunchRender( false, true );
  }
  catch( Nocturnal::Exception& e )
  {
    std::stringstream stream;
    stream << "Could not render with mentalray!\n\nReason: " << e.what() << "\n";

    ::MessageBox( NULL, stream.str().c_str(), "Render Error", MB_OK | MB_ICONEXCLAMATION );
    Console::Error( "Could not render with mentalray!\n\nReason: %s\n", e.what() );
  }  
}


bool LightingTool::GetRenderSelected() const
{
  return RenderSelectedOnly;
}

void LightingTool::SetRenderSelected( bool selected )
{
  RenderSelectedOnly = selected;
}
