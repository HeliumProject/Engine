#include "Precompile.h"
#include "RemoteConstruct.h"

#include "IPC/TCP.h"
#include "IPC/Pipe.h"
#include "File/Manager.h"
#include "Debug/Exception.h"
#include "Windows/Process.h"
#include "Console/Console.h"

#include "Entity.h"
#include "EntityAssetSet.h"

#include "SceneEditor.h"
#include "SceneManager.h"
#include "Editor/SessionManager.h"

#include "Asset/AssetClass.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"

#include <wx/progdlg.h>

using namespace RPC;
using namespace Luna;
using namespace Luna::RemoteConstruct;


RPC::IConstructionHost* RemoteConstruct::g_ConstructionHost = NULL;
RPC::IConstructionTool* RemoteConstruct::g_ConstructionTool = NULL;

IPC::PipeConnection     g_Connection;
RPC::Host               g_Host;

static bool             g_Initialized = false;
static bool             g_Enabled = false;

class MayaConnectionTimer : public wxTimer
{
  virtual void Notify() NOC_OVERRIDE
  {
    g_Host.Dispatch();
  }
};

static MayaConnectionTimer* g_Timer = NULL;

struct ConstructionHost : IConstructionHost
{
  LOCAL_IMPL(ConstructionHost, RPC::IConstructionHost)

  virtual void Reset();
  virtual void RefreshEntityAsset(RPC::EntityAssetParam* param);
  virtual void TransformInstance(RPC::TransformInstanceParam* param);
  virtual void CreateInstance(RPC::CreateInstanceParam* param);
};

void ConstructionHost::Reset()
{
  if (g_Enabled)
  {
    SceneEditor* editor = static_cast< SceneEditor* > (SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene ));

    if (editor == NULL)
    {
      return;
    }

    Luna::Scene* scene = editor->GetSceneManager()->GetCurrentScene();

    if (scene == NULL)
    {
      return;
    }

    RemoteConstruct::g_ConstructionTool->BeginScene();

    OS_SelectableDumbPtr::Iterator itr = scene->GetSelection().GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = scene->GetSelection().GetItems().End();

    S_tuid uniqueUfragArtFiles;
    for ( ; itr != end; ++itr )
    {
      Luna::Entity* entity = Reflect::ObjectCast<Luna::Entity>( *itr );

      if (entity)
      {
        bool createProxy = true;
        if( entity->GetClassSet()->GetEntityAsset()->GetEngineType() == Asset::EngineTypes::Ufrag )
        {
          Attribute::AttributeViewer< Asset::ArtFileAttribute > model( entity->GetClassSet()->GetEntityAsset() );
          createProxy = uniqueUfragArtFiles.insert( model->GetFileID() ).second;
        }

        if( createProxy )
        {
          tuid assetClass = entity->GetClassSet()->GetEntityAssetID();
          CreateInstanceParam param;
          {
            param.m_ID = entity->GetID();
            param.m_EntityAsset = assetClass;
            strncpy(param.m_Name.Characters, entity->GetName().c_str(), RPC_STRING_MAX);
            param.m_Name.Characters[ RPC_STRING_MAX-1 ] = 0; 
            memcpy(&param.m_Transform, &entity->GetGlobalTransform(), sizeof(Math::Matrix4));
          }
          RemoteConstruct::g_ConstructionTool->CreateProxy(&param);
          memcpy(&param.m_Transform, &entity->GetGlobalTransform(), sizeof(Math::Matrix4));
        }
      }
    }

    RemoteConstruct::g_ConstructionTool->EndScene();
  }
}

void ConstructionHost::RefreshEntityAsset(EntityAssetParam* param)
{
  if (g_Enabled)
  {
    SceneEditor* editor = static_cast< SceneEditor* > (SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene ));

    if (editor == NULL)
    {
      return;
    }

    std::string path;

    try
    {
      path = File::GlobalManager().GetPath( param->m_EntityAsset );
    }
    catch ( const File::Exception& e )
    {
      Console::Error("%s\n", e.what());
      return;
    }

    Luna::Scene* scene = editor->GetSceneManager()->GetScene( path );

    if (scene)
    {
      editor->GetSceneManager()->RemoveScene( scene );

      scene->Execute(true);
    }
  }
}

void ConstructionHost::TransformInstance(TransformInstanceParam* param)
{
  if (g_Enabled)
  {
    SceneEditor* editor = static_cast< SceneEditor* > (SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene ));

    if (editor == NULL)
    {
      return;
    }

    Luna::Scene* scene = editor->GetSceneManager()->GetCurrentScene();

    if (scene == NULL)
    {
      return;
    }

    Luna::SceneNode* node = scene->FindNode( param->m_ID );

    if (node == NULL)
    {
      return;
    }

    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( node );

    if (entity == NULL)
    {
      return;
    }

    Console::Debug("Updating %s\n", entity->GetName().c_str());

    if ( scene->IsEditable() )
    {
      entity->SetGlobalTransform( (const Math::Matrix4&)param->m_Transform );

      scene->Execute(true);
    }
  }
}

void ConstructionHost::CreateInstance( CreateInstanceParam* param )
{
  if (g_Enabled)
  {
    SceneEditor* editor = static_cast< SceneEditor* > (SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene ));

    if (editor == NULL)
    {
      return;
    }

    Luna::Scene* scene = editor->GetSceneManager()->GetCurrentScene();

    if( scene )
    {
      scene->IsEditable();

      tuid uid = param->m_ID;
      tuid entityClassID = param->m_EntityAsset;
      
      Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( entityClassID );
      if( assetClass.ReferencesObject() )
      {
        S_tuid entityClasses;

        V_EntityDumbPtr entities;
        scene->GetAll< Luna::Entity >( entities );

        V_EntityDumbPtr::iterator itor = entities.begin();
        V_EntityDumbPtr::iterator end  = entities.end();
        for ( ; itor != end; ++itor )
        {
          entityClasses.insert( (*itor)->GetClassSet()->GetEntityAssetID() );
        }

        // don't create a new instance of a ufrag if an instance already exists
        if( assetClass->GetEngineType() == Asset::EngineTypes::Ufrag )
        {
          if( entityClasses.find( assetClass->m_AssetClassID ) != entityClasses.end() )
          {
            return;
          }
        }

        Asset::Entity* contentEntity = new Asset::Entity( uid, entityClassID );
        contentEntity->m_DefaultName = param->m_Name.Characters;

        Luna::Entity* newEntity = new Luna::Entity( scene, contentEntity );
        
        scene->AddObject( newEntity );

        newEntity->Initialize();
      }
    }
  }
}

void RemoteConstruct::Initialize()
{

  // if we had a connection that was not cleaned up 
  // (i.e. maya crash while connected )

  IPC::Connection* conn = g_Host.GetConnection();
  if( conn && conn->GetState() == IPC::ConnectionStates::Failed )
  {
    Cleanup();                // clean up 
    g_Connection.Cleanup();   // clean up connection (client/server threads)
  }

  if( g_Initialized )
  {
    return;
  }

  g_Initialized = true;

  g_Connection.Initialize(true, "Construction Connection", "construction");
  g_Host.SetConnection(&g_Connection);

  // create a local repeater to send messages to our viewer
  g_ConstructionTool = new IConstructionToolRemote (&g_Host);

  // create a local implementation of our functionality
  g_ConstructionHost = new ConstructionHost (&g_Host);
  g_Host.SetLocalInterface(kConstructionHost, g_ConstructionHost);

  g_Timer = new MayaConnectionTimer();
  g_Timer->Start( 20 );
}

void RemoteConstruct::Cleanup()
{
  if (!g_Initialized)
  {
    return;
  }

  g_Initialized = false;

  delete g_ConstructionHost;
  g_ConstructionHost = NULL;

  delete g_ConstructionTool;
  g_ConstructionTool = NULL;

  delete g_Timer;
  g_Timer = NULL;
}

bool RemoteConstruct::EstablishConnection()
{
  // this is re-entrant safe
  RemoteConstruct::Initialize();

  // turn on message handling
  RemoteConstruct::Enable( true );

  // give a little time for a natural connection (maya is ready and waiting)
  ::Sleep( 500 );

  // while we are not connected
  while (!g_Host.Connected())
  {
    int result = wxMessageBox( "Unable to connect to Maya.  Would you like to open new instance of Maya? (Click \"No\" to keep waiting)", "Unable to Connect!", wxCENTER | wxYES_NO | wxCANCEL | wxICON_QUESTION, wxTheApp->GetTopWindow() );
    if (result == wxYES)
    {
      std::ostringstream cmd;
      cmd << "maya.exe -command \"construction -connect\"";
      Windows::Execute( cmd.str(), true );
    }
    else if (result == wxCANCEL)
    {
      break;
    }

    wxProgressDialog dialog ("Waiting", "Waiting for Maya connection...", 100, wxTheApp->GetTopWindow(), wxPD_CAN_ABORT );

    bool wait = true;
    while( !RemoteConstruct::Connected() && wait )
    {
      wait = dialog.Pulse();
      if (wait)
      {
        Sleep(100);
      }
    }

    if (!wait)
    {
      break;
    }

    dialog.Hide();
  }

  return g_Host.Connected();
}

bool RemoteConstruct::Connected()
{
  return g_Host.Connected();
}

void RemoteConstruct::Enable( bool enable )
{
  g_Enabled = enable;
}