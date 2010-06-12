#include "Precompile.h"
#include "Application.h"
#include "AppPreferences.h"
#include "ArtProvider.h"

#include "Application/Application.h"
#include "AssetEditor/AssetInit.h"
#include "Asset/Tracker.h"
#include "Browser/Browser.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Log.h"
#include "Core/CoreInit.h"
#include "Debug/Exception.h"
#include "Editor/ApplicationPreferences.h"
#include "Editor/Editor.h"
#include "Editor/EditorInit.h"
#include "Editor/Preferences.h"
#include "FileSystem/FileSystem.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/Finder.h"
#include "Finder/LunaSpecs.h"
#include "Math/Utils.h"
#include "Scene/SceneEditor.h"
#include "Scene/SceneInit.h"
#include "Task/TaskInit.h"
#include "UIToolKit/ImageManager.h"
#include "Platform/Windows/Windows.h"
#include "Platform/Process.h"
#include "Application/Worker/Process.h"

#include "UI/DebugUI/DebugUI.h"
#include "UI/PerforceUI/PerforceUI.h"

#include <wx/cmdline.h>
#include <wx/splash.h>

using namespace Luna;
using namespace Nocturnal;

namespace Luna
{
    IMPLEMENT_APP( LunaApp );
}

LunaApp::LunaApp()
: wxApp()
, m_AssetTracker( NULL )
, m_DocumentManager( new DocumentManager() )
, m_SceneEditor( NULL )
{
}

LunaApp::~LunaApp()
{
    if ( m_AssetTracker )
    {
        delete m_AssetTracker;
    }

    if ( m_DocumentManager )
    {
        delete m_DocumentManager;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called from OnInit.  Adds the command line description to the parser.
// 
void LunaApp::OnInitCmdLine( wxCmdLineParser& parser )
{
  SetVendorName( "Nocturnal" );

  parser.SetLogo( wxT( "Luna (c) 2010 - Nocturnal\n" ) );

  parser.AddSwitch( "pipe", "Pipe",               "Use pipe for console connection" ); 
  parser.AddSwitch( "disable_tracker", "DisableTracker", "Disable Asset Tracker" );

  parser.AddSwitch( WindowSettings::s_Reset, WindowSettings::s_ResetLong, "Reset all window positions (other prefs/mru will remain)" );
  parser.AddSwitch( Preferences::s_ResetPreferences, Preferences::s_ResetPreferencesLong, "Resets all preferences for all of Luna" );

  parser.AddSwitch( Worker::Args::Debug,      "Debug",                "Debug use of background processes" );
  parser.AddSwitch( Worker::Args::Wait,       "Wait",                 "Wait forever for background processes" );
  parser.AddSwitch( Application::Args::Script,   "Script",               "Omit prefix and suffix in console output" );
  parser.AddSwitch( Application::Args::Attach,   "Attach",               "Wait for a debugger to attach to the process on startup" );
  parser.AddSwitch( Application::Args::Profile,  "Profile",              "Enable profile output to the console windows" );
  parser.AddSwitch( Application::Args::Memory,   "Memory",               "Profile and report memory usage to the console" );
  parser.AddSwitch( Application::Args::Verbose,  "Verbose",              "Output a verbose level of console output" );
  parser.AddSwitch( Application::Args::Extreme,  "Extreme",              "Output an extremely verbose level of console output" );
  parser.AddSwitch( Application::Args::Debug,    "Debug",                "Output debug console output" );

  __super::OnInitCmdLine( parser );
}

///////////////////////////////////////////////////////////////////////////////
// Called after OnInitCmdLine.  The base class handles the /help command line
// switch and exits.  If we get this far, we need to parse the command line
// and determine what mode to launch the app in.
// 
bool LunaApp::OnCmdLineParsed( wxCmdLineParser& parser )
{
  wxArtProvider::Push( new ::Luna::ArtProvider() );

  // don't spend a lot of time updating idle events for windows that don't need it
  wxUpdateUIEvent::SetMode( wxUPDATE_UI_PROCESS_SPECIFIED );
  wxIdleEvent::SetMode( wxIDLE_PROCESS_SPECIFIED );

  {
    Log::Bullet initialize ("Initializing\n");

    m_InitializerStack.Push( PerforceUI::Initialize, PerforceUI::Cleanup );

    {
      Log::Bullet modules ("Modules:\n");

      {
        Log::Bullet bullet ("Core...\n");
        m_InitializerStack.Push( CoreInitialize, CoreCleanup );
      }

      {
        Log::Bullet bullet ("Editor...\n");
        m_InitializerStack.Push( PreferencesBase::InitializeType, PreferencesBase::CleanupType );
        m_InitializerStack.Push( Preferences::InitializeType, Preferences::CleanupType );
        m_InitializerStack.Push( AppPreferences::InitializeType, AppPreferences::CleanupType );
        m_InitializerStack.Push( EditorInitialize, EditorCleanup );
      }

      {
        Log::Bullet bullet ("Task...\n");
        m_InitializerStack.Push( TaskInitialize, TaskCleanup );
      }

      {
        Log::Bullet vault ("Asset Vault...\n");
        m_InitializerStack.Push( Browser::Initialize, Browser::Cleanup );
      }

      {
        Log::Bullet bullet ("Asset Editor...\n");
        m_InitializerStack.Push( LunaAsset::InitializeModule, LunaAsset::CleanupModule );
      }

      {
        Log::Bullet bullet ("Scene Editor...\n");
        m_InitializerStack.Push( SceneInitialize, SceneCleanup );
      }
    }

    {
      Log::Bullet systems ("Systems:\n");

      {
        Log::Bullet vault ("Asset Tracker...\n");
        GetAppPreferences()->UseTracker( false ); //!parser.Found( "disable_tracker" ) );
      }
    }
  }

  Log::Print("\n"); 

  if ( Log::GetErrorCount() )
  {
    std::stringstream str;
    str << "There were errors during startup, use Luna with caution.";
    wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK );
  }

  GetSceneEditor()->Show();

  return __super::OnCmdLineParsed( parser );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the application is ready to start running.
// 
int LunaApp::OnRun()
{
  return __super::OnRun();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the application is being exited.  Cleans up resources.
// 
int LunaApp::OnExit()
{
  // Save preferences
    ::Luna::GetApplicationPreferences()->SavePreferences();

  if ( m_AssetTracker )
  {
      m_AssetTracker->StopThread();
      delete m_AssetTracker;
  }

  UIToolKit::ImageManagerCleanup();

  m_InitializerStack.Cleanup();

  return __super::OnExit();
}

std::string LunaApp::ShowFileBrowser()
{
    NOC_BREAK();
#pragma TODO( "Reimplent to use the Vault" )
  //File::FileBrowser browserDlg( NULL, -1, "Luna File Browser" );

  //browserDlg.AddFilter( FinderSpecs::Asset::ANIM_DECORATION );
  //browserDlg.AddFilter( FinderSpecs::Asset::ANIMSET_DECORATION );
  //browserDlg.AddFilter( FinderSpecs::Asset::CINEMATIC_DECORATION ); 
  //browserDlg.AddFilter( FinderSpecs::Asset::CONTENT_DECORATION );
  //browserDlg.AddFilter( FinderSpecs::Asset::ENTITY_DECORATION );
  //browserDlg.AddFilter( FinderSpecs::Asset::FOLIAGE_DECORATION );
  //browserDlg.AddFilter( FinderSpecs::Asset::LEVEL_DECORATION );
  //browserDlg.AddFilter( FinderSpecs::Asset::MATERIAL_DECORATION );
  //browserDlg.AddFilter( FinderSpecs::Extension::MAYA_BINARY );
  //browserDlg.AddFilter( FinderSpecs::Asset::SHADER_DECORATION );
  //browserDlg.AddFilter( FinderSpecs::Extension::SPEEDTREE );
  //browserDlg.AddFilter( FinderSpecs::Asset::SKY_DECORATION );
  //browserDlg.AddFilter( FinderSpecs::Extension::TEXTUREMAP_FILTER );

  //if ( browserDlg.ShowModal() == wxID_OK )
  //{
  //  std::string fullPath = browserDlg.GetPath();
  //  if ( FileSystem::Exists( fullPath ) )
  //  {
  //    return fullPath;
  //  }
  //}

  return "";
}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for the application.
//
int main()
{
  Nocturnal::InitializerStack initializerStack( true );

  initializerStack.Push( &DebugUI::Initialize, &DebugUI::Cleanup );

  return Application::StandardWinMain( &wxEntry );
}
