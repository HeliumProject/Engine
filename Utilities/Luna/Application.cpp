#include "Precompile.h"
#include "Application.h"

#include "AnimationEventsEditor/AnimationEventsInit.h"
#include "AppUtils/AppUtils.h"
#include "AssetEditor/AssetInit.h"
#include "Asset/Tracker.h"
#include "Browser/Browser.h"
#include "CharacterEditor/CharacterInit.h"
#include "CinematicEventsEditor/CinematicEventsInit.h"
#include "Common/InitializerStack.h"
#include "Console/Console.h"
#include "Core/CoreInit.h"
#include "Debug/Exception.h"
#include "DebugUI/DebugUI.h"
#include "Editor/ApplicationPreferences.h"
#include "Editor/Editor.h"
#include "Editor/EditorChooser.h"
#include "Editor/EditorInit.h"
#include "Editor/Preferences.h"
#include "Editor/SessionFrame.h"
#include "Editor/SessionManager.h"
#include "File/Manager.h"
#include "FileBrowser/FileBrowser.h"
#include "FileSystem/FileSystem.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/Finder.h"
#include "Finder/LunaSpecs.h"
#include "Live/LiveInit.h"  
#include "Math/Utils.h"
#include "PerforceUI/PerforceUI.h"
#include "Scene/SceneEditor.h"
#include "Scene/SceneInit.h"
#include "Symbol/Inheritance.h"
#include "Symbol/SymbolBuilder.h"
#include "Task/TaskInit.h"
#include "UIToolKit/ImageManager.h"
#include "Windows/Process.h"
#include "Worker/Process.h"

#include <wx/cmdline.h>
#include <wx/splash.h>


using namespace Luna;


IMPLEMENT_APP( Application );


///////////////////////////////////////////////////////////////////////////////
// Called from OnInit.  Adds the command line description to the parser.
// 
void Application::OnInitCmdLine( wxCmdLineParser& parser )
{
  SetVendorName( "Insomniac Games" );

  parser.SetLogo( wxT( "Luna (c) 2009 - Insomniac Games\n" ) );

  parser.AddOption( "f",    "File",               "Open a file" );
  parser.AddSwitch( "a",    "AssetEditor",        "Launch Asset Editor" );
  parser.AddSwitch( "s",    "SceneEditor",        "Launch Scene Editor" );
  parser.AddSwitch( "e",    "EffectEventsEditor", "Launch Animation Events Editor" );
  parser.AddSwitch( "b",    "FileBrowser",        "Asset File Broswer" );
  parser.AddSwitch( "c",    "CharacterEditor",    "Character Editor" ); 
  parser.AddSwitch( "pipe", "Pipe",               "Use pipe for console connection" ); 
  parser.AddSwitch( "m",    "CinematicEditor",    "Cinematic Editor" );
  parser.AddSwitch( "disable_tracker", "DisableTracker", "Disable Asset Tracker" );

  parser.AddSwitch( WindowSettings::s_Reset, WindowSettings::s_ResetLong, "Reset all window positions (other prefs/mru will remain)" );
  parser.AddSwitch( Preferences::s_ResetPreferences, Preferences::s_ResetPreferencesLong, "Resets all preferences for all of Luna" );

  parser.AddSwitch( Worker::Args::Debug,      "Debug",                "Debug use of background processes" );
  parser.AddSwitch( Worker::Args::Wait,       "Wait",                 "Wait forever for background processes" );
  parser.AddSwitch( AppUtils::Args::Script,   "Script",               "Omit prefix and suffix in console output" );
  parser.AddSwitch( AppUtils::Args::Attach,   "Attach",               "Wait for a debugger to attach to the process on startup" );
  parser.AddSwitch( AppUtils::Args::Profile,  "Profile",              "Enable profile output to the console windows" );
  parser.AddSwitch( AppUtils::Args::Memory,   "Memory",               "Profile and report memory usage to the console" );
  parser.AddSwitch( AppUtils::Args::Verbose,  "Verbose",              "Output a verbose level of console output" );
  parser.AddSwitch( AppUtils::Args::Extreme,  "Extreme",              "Output an extremely verbose level of console output" );
  parser.AddSwitch( AppUtils::Args::Debug,    "Debug",                "Output debug console output" );

  __super::OnInitCmdLine( parser );
}

///////////////////////////////////////////////////////////////////////////////
// Called after OnInitCmdLine.  The base class handles the /help command line
// switch and exits.  If we get this far, we need to parse the command line
// and determine what mode to launch the app in.
// 
bool Application::OnCmdLineParsed( wxCmdLineParser& parser )
{
  // enable heap defragmenting
  bool lowFragHeap = Windows::EnableLowFragmentationHeap();
  Console::Debug("Low Fragmentation Heap is %s\n", lowFragHeap ? "enabled" : "not enabled");

  UIToolKit::ImageManagerInit( FinderSpecs::Luna::DEFAULT_THEME_FOLDER.GetFolder(), FinderSpecs::Luna::GAME_THEME_FOLDER.GetFolder() );
  UIToolKit::GlobalImageManager().LoadGuiArt();

  V_string splashes;
  u32 rand;
  rand_s(&rand);
  const char* splashImage = "luna_logo.png";
  std::string directory = FinderSpecs::Luna::SPLASH_SCREEN_FOLDER.GetFolder();
  if ( FileSystem::Exists( directory ) )
  {
    FileSystem::GetFiles( directory, splashes, "*.png" );

    if ( splashes.size() > 0 )
    {
      splashImage = splashes[rand % splashes.size()].c_str();
    }
  }

  // display splash
  wxBitmap bitmap;
  wxSplashScreen* splash = NULL;
  if ( splashImage )
  {
    bitmap = UIToolKit::GlobalImageManager().GetBitmap( splashImage );
    if ( bitmap.IsOk() )
    {
      splash = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_NO_TIMEOUT, 0, NULL, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxFRAME_NO_TASKBAR );
    }
  }

  // give up some time for the splash to show
  Yield();

  // don't spend a lot of time updating idle events for windows that don't need it
  wxUpdateUIEvent::SetMode( wxUPDATE_UI_PROCESS_SPECIFIED );
  wxIdleEvent::SetMode( wxIDLE_PROCESS_SPECIFIED );

  {
    Console::Bullet initialize ("Initializing\n");

    m_InitializerStack.Push( PerforceUI::Initialize, PerforceUI::Cleanup );

    {
      Console::Bullet systems ("Systems:\n");

      {
        Console::Bullet bullet ("File Manager...\n");
        m_InitializerStack.Push( File::Initialize, File::Cleanup );
      }

      {
        Console::Bullet bullet ("Symbol System...\n");
        m_InitializerStack.Push( Symbol::Initialize, Symbol::Cleanup );
      }

      {
        Console::Bullet bullet ("Symbol Builder...\n");
        Symbol::SymbolBuilder::GetInstance()->Load();
      }

      {
        Console::Bullet bullet ("Symbol Inheritance Hierarchy...\n");
        Symbol::Inheritance::InitializeInheritance();
      }

      {
        Console::Bullet vault ("Asset Tracker...\n");
        m_InitializerStack.Push( Asset::Tracker::Initialize, Asset::Tracker::Cleanup );
        SessionManager::GetInstance()->UseTracker( !parser.Found( "disable_tracker" ) );
      }
    }

    {
      Console::Bullet modules ("Modules:\n");

      {
        Console::Bullet bullet ("Core...\n");
        m_InitializerStack.Push( CoreInitialize, CoreCleanup );
      }

      {
        Console::Bullet bullet ("Live...\n");
        m_InitializerStack.Push( LiveInitialize, LiveCleanup );
      }

      {
        Console::Bullet bullet ("Editor...\n");
        m_InitializerStack.Push( EditorInitialize, EditorCleanup );
      }

      {
        Console::Bullet bullet ("Task...\n");
        m_InitializerStack.Push( TaskInitialize, TaskCleanup );
      }

      {
        Console::Bullet vault ("Asset Vault...\n");
        m_InitializerStack.Push( Browser::Initialize, Browser::Cleanup );
      }

      {
        Console::Bullet bullet ("Asset Editor...\n");
        m_InitializerStack.Push( LunaAsset::InitializeModule, LunaAsset::CleanupModule );
      }

      {
        Console::Bullet bullet ("Animation Events Editor...\n");
        m_InitializerStack.Push( LunaAnimationEvents::InitializeModule, LunaAnimationEvents::CleanupModule );
      }

      {
        Console::Bullet bullet ("Cinematic Events Editor...\n");
        m_InitializerStack.Push( LunaCinematicEvents::InitializeModule, LunaCinematicEvents::CleanupModule );
      }

      {
        Console::Bullet bullet ("Scene Editor...\n");
        m_InitializerStack.Push( SceneInitialize, SceneCleanup );
      }

      {
        Console::Bullet bullet ("Character Editor...\n"); 
        m_InitializerStack.Push( LunaCharacter::InitializeModule, LunaCharacter::CleanupModule );
      }
    }
  }

  Console::Print("\n"); 

  if ( SessionManager::GetInstance()->UseTracker() )
  {
    // Start the tracker after we've initialized everything else
    Asset::GlobalTracker()->StartThread();
  }

  // kill splash screen
  delete splash;

  if ( Console::GetErrorCount() )
  {
    std::stringstream str;
    str << "There were errors during startup, use Luna with caution.";
    wxMessageBox( str.str().c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK );
  }

  wxString file;

  // if the user just wants to browse files
  if ( parser.Found( "b" ) )
  {
    std::string filePath = ShowFileBrowser();
    if ( !filePath.empty() )
    {
      SessionManager::GetInstance()->Edit( file.c_str() );
    }
    else
    {
      Console::Error("No file selected!\n", file.c_str());
      OnExit();
      return false;
    }
  }
  else if ( parser.Found( "f", &file ) )
  {
    if ( FileSystem::Exists( file.c_str() ) )
    {
      SessionManager::GetInstance()->Edit( file.c_str() );
    }
    else
    {
      Console::Error("File does not exist: '%s'\n", file.c_str());
      OnExit();
      return false;
    }
  }
  else
  {
    Editor* editor = NULL;

    // Determine which editors to show at startup
    if ( parser.Found( "s" ) )
    {
      editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::Scene );
    }

    if ( parser.Found( "a" ) )
    {
      editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::Asset );
    }

    if ( parser.Found( "e" ) )
    {
      editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::AnimationEvents );
    }

    if (parser.Found( "c" ) )
    {
      editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::Character ); 
    }

    if ( parser.Found( "m" ) )
    {
      editor = SessionManager::GetInstance()->LaunchEditor( EditorTypes::CinematicEvents );
    }

    // No editor was specified via a command line switch, so prompt the user 
    // for which editor to launch.
    if ( !editor )
    {
      SessionFrame* session = new SessionFrame();
      session->Show();
    }
  }

  return __super::OnCmdLineParsed( parser );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the application is ready to start running.
// 
int Application::OnRun()
{
  return __super::OnRun();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the application is being exited.  Cleans up resources.
// 
int Application::OnExit()
{
  // Save preferences
  Luna::GetApplicationPreferences()->SavePreferences();

  if ( SessionManager::GetInstance()->UseTracker() )
  {
    // Stop the thread before we start breaking down the InitStack
    Asset::GlobalTracker()->StopThread();
  }

  UIToolKit::ImageManagerCleanup();

  m_InitializerStack.Cleanup();

  return __super::OnExit();
}

std::string Application::ShowFileBrowser()
{
  File::FileBrowser browserDlg( NULL, -1, "Luna File Browser" );

  browserDlg.AddFilter( FinderSpecs::Asset::ANIM_DECORATION );
  browserDlg.AddFilter( FinderSpecs::Asset::ANIMSET_DECORATION );
  browserDlg.AddFilter( FinderSpecs::Asset::CINEMATIC_DECORATION ); 
  browserDlg.AddFilter( FinderSpecs::Asset::CONTENT_DECORATION );
  browserDlg.AddFilter( FinderSpecs::Asset::ENTITY_DECORATION );
  browserDlg.AddFilter( FinderSpecs::Asset::FOLIAGE_DECORATION );
  browserDlg.AddFilter( FinderSpecs::Asset::LEVEL_DECORATION );
  browserDlg.AddFilter( FinderSpecs::Asset::MATERIAL_DECORATION );
  browserDlg.AddFilter( FinderSpecs::Extension::MAYA_BINARY );
  browserDlg.AddFilter( FinderSpecs::Asset::SHADER_DECORATION );
  browserDlg.AddFilter( FinderSpecs::Extension::SPEEDTREE );
  browserDlg.AddFilter( FinderSpecs::Asset::SKY_DECORATION );
  browserDlg.AddFilter( FinderSpecs::Extension::TEXTUREMAP_FILTER );

  if ( browserDlg.ShowModal() == wxID_OK )
  {
    std::string fullPath = browserDlg.GetPath();
    if ( FileSystem::Exists( fullPath ) )
    {
      return fullPath;
    }
  }

  return "";
}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for the application.
//
int main()
{
  Nocturnal::InitializerStack initializerStack( true );

  initializerStack.Push( &DebugUI::Initialize, &DebugUI::Cleanup );

  return AppUtils::StandardWinMain( &wxEntry );
}
