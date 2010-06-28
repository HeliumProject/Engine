#include "Precompile.h"
#include "Application.h"
#include "AppPreferences.h"
#include "ArtProvider.h"

#include "Application/Application.h"
#include "AssetEditor/AssetInit.h"
#include "Browser/Browser.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Log.h"
#include "Core/CoreInit.h"
#include "Foundation/Exception.h"
#include "Editor/ApplicationPreferences.h"
#include "Editor/Editor.h"
#include "Editor/EditorInit.h"
#include "Editor/Preferences.h"
#include "Foundation/Math/Utils.h"
#include "Scene/SceneEditor.h"
#include "Scene/SceneInit.h"
#include "Task/TaskInit.h"
#include "Application/UI/ImageManager.h"
#include "Platform/Windows/Windows.h"
#include "Platform/Process.h"
#include "Application/Worker/Process.h"

#include "Application/UI/DebugUI/DebugUI.h"
#include "Application/UI/PerforceUI/PerforceUI.h"


#include "Foundation/CommandLine/Option.h"
#include "Foundation/CommandLine/Command.h"
#include "Foundation/CommandLine/Commands/Help.h"
#include "Foundation/CommandLine/Processor.h"

//#include "Commands/BuildCommand.h"
#include "Commands/ProfileDumpCommand.h"
#include "Commands/RebuildCommand.h"

#include <wx/cmdline.h>
#include <wx/splash.h>

using namespace Luna;
using namespace Nocturnal;
using namespace Nocturnal::CommandLine;

namespace Luna
{
    IMPLEMENT_APP( LunaApp );
}

LunaApp::LunaApp()
: wxApp()
, m_DocumentManager( new DocumentManager() )
, m_SceneEditor( NULL )
{
}

LunaApp::~LunaApp()
{
    if ( m_DocumentManager )
    {
        delete m_DocumentManager;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called after OnInitCmdLine.  The base class handles the /help command line
// switch and exits.  If we get this far, we need to parse the command line
// and determine what mode to launch the app in.
// 
bool LunaApp::OnInit()
{
    wxArtProvider::Push( new ::Luna::ArtProvider() );

    SetVendorName( "Nocturnal" );

    //SetLogo( wxT( "Luna (c) 2010 - Nocturnal\n" ) );

    // don't spend a lot of time updating idle events for windows that don't need it
    wxUpdateUIEvent::SetMode( wxUPDATE_UI_PROCESS_SPECIFIED );
    wxIdleEvent::SetMode( wxIDLE_PROCESS_SPECIFIED );

    char module[MAX_PATH];
    GetModuleFileName( 0, module, MAX_PATH );

    Nocturnal::Path exePath( module );
    Nocturnal::Path iconFolder( exePath.Directory() + "Icons/" );

    Nocturnal::ImageManagerInit( iconFolder.Get(), "" );
    Nocturnal::GlobalImageManager().LoadGuiArt();

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

    //return __super::OnCmdLineParsed( parser );
    return true;
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

    Nocturnal::ImageManagerCleanup();

    m_InitializerStack.Cleanup();

    return __super::OnExit();
}

/////////////////////////////////////////////////////////////////////////////////
int Main ( int argc, const char** argv )
{
	// print physical memory
	MEMORYSTATUSEX status;
	memset(&status, 0, sizeof(status));
	status.dwLength = sizeof(status);
	::GlobalMemoryStatusEx(&status);
	Log::Print("Physical Memory: %I64u M bytes total, %I64u M bytes available\n", status.ullTotalPhys >> 20, status.ullAvailPhys >> 20);

	// fill out the options vector
	std::vector< std::string > options;
	for ( int i = 1; i < argc; ++i )
	{
		options.push_back( argv[ i ] );
	}
    std::vector< std::string >::const_iterator& argsBegin = options.begin(), argsEnd = options.end();

    bool success = true;
	std::string error; 


    Processor processor( "luna", "[COMMAND <ARGS>]", "Luna (c) 2010 - Nocturnal" );

    //BuildCommand buildCommand;
    //success &= buildCommand.Initialize( error );
    //success &= processor.RegisterCommand( &buildCommand, error );

    ProfileDumpCommand profileDumpCommand;
    success &= profileDumpCommand.Initialize( error );
    success &= processor.RegisterCommand( &profileDumpCommand, error );

    RebuildCommand rebuildCommand;
    success &= rebuildCommand.Initialize( error );
    success &= processor.RegisterCommand( &rebuildCommand, error );

    Nocturnal::CommandLine::Help helpCommand;
    helpCommand.SetOwner( &processor );
    success &= helpCommand.Initialize( error );
    success &= processor.RegisterCommand( &helpCommand, error );

    //success &= processor.AddOption( new FlagOption(  , "pipe", "use pipe for console connection" ), error ); 
    //success &= processor.AddOption( new FlagOption(  , "disable_tracker", "disable Asset Tracker" ), error );
    //
    //success &= processor.AddOption( new FlagOption(  , WindowSettings::s_Reset, "reset all window positions" ), error );
    //success &= processor.AddOption( new FlagOption(  , Preferences::s_ResetPreferences, "resets all preferences for all of Luna" ), error );
    //
    //success &= processor.AddOption( new FlagOption(  , Worker::Args::Debug, "debug use of background processes" ), error );
    //success &= processor.AddOption( new FlagOption(  , Worker::Args::Wait, "wait forever for background processes" ), error );

    bool scriptFlag = false;
    success &= processor.AddOption( new FlagOption( &scriptFlag, Application::Args::Script, "omit prefix and suffix in console output" ), error );
    
    bool attachFlag = false;
    success &= processor.AddOption( new FlagOption( &attachFlag, Application::Args::Attach, "wait for a debugger to attach to the process on startup" ), error );
    
    bool profileFlag = false;
    success &= processor.AddOption( new FlagOption( &profileFlag, Application::Args::Profile, "enable profile output to the console windows" ), error );
    
    bool memoryFlag = false;
    success &= processor.AddOption( new FlagOption( &memoryFlag, Application::Args::Memory, "profile and report memory usage to the console" ), error );
    
    bool vreboseFlag = false;
    success &= processor.AddOption( new FlagOption( &vreboseFlag, Application::Args::Verbose, "output a verbose level of console output" ), error );
    
    bool extremeFlag = false;
    success &= processor.AddOption( new FlagOption( &extremeFlag, Application::Args::Extreme, "output an extremely verbose level of console output" ), error );
    
    bool debugFlag = false;
    success &= processor.AddOption( new FlagOption( &debugFlag, Application::Args::Debug, "output debug console output" ), error );
    
    int nice = 0;
    success &= processor.AddOption( new SimpleOption<int>( &nice , "nice", "<NUM>", "number of processors to nice (for other processes)" ), error );
    
    bool helpFlag;
    success &= processor.AddOption( new FlagOption( &helpFlag, "h|help", "print program usage" ), error );

    success &= processor.ParseOptions( argsBegin, argsEnd, error );

	if ( success )
	{
        if ( helpFlag )
        {
            Log::Print( "\nPrinting help for Luna...\n" );
            Log::Print( processor.Help().c_str() );
            Log::Print( "\n" );
            success = true;
        }
        else if ( argsBegin != argsEnd )
        {
            while ( success && ( argsBegin != argsEnd ) )
            {
                const std::string& arg = (*argsBegin);
                ++argsBegin;

                if ( arg.length() < 1 )
                {
                    continue;
                }

                if ( arg[ 0 ] == '-' )
                {
                    error = std::string( "Unknown option, or option passed out of order: " ) + arg;
                    success = false;
                }
                else
                {
                    Command* command = processor.GetCommand( arg );
                    if ( command )
                    {
                        success = command->Process( argsBegin, argsEnd, error );
                    }
                    else
                    {
                        error = std::string( "Unknown commandline parameter: " ) + arg + "\n\n";
                        success = false;
                    }
                }
            }
        }
        else
        {
            //buildCommand.Cleanup();
            rebuildCommand.Cleanup();

            ::FreeConsole();
            return Application::StandardWinMain( &wxEntry );
        }
    }

    //buildCommand.Cleanup();
    rebuildCommand.Cleanup();

    if ( !success && !error.empty() )
    {
        Log::Error( "%s\n", error.c_str() );
    }

    return success ? 0 : 1;
}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for the application.
//
int main( int argc, const char** argv )
{
    Nocturnal::InitializerStack initializerStack( true );
    initializerStack.Push( &DebugUI::Initialize, &DebugUI::Cleanup );

    return Application::StandardMain( &Main, argc, argv );
}
