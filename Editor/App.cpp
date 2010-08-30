#include "Precompile.h"
#include "Editor/App.h"

#include "Platform/Windows/Windows.h"
#include "Platform/Windows/Console.h"
#include "Platform/Process.h"
#include "Platform/Exception.h"
#include "Platform/Debug.h"

#include "Foundation/Log.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Math/Utils.h"
#include "Foundation/CommandLine/Option.h"
#include "Foundation/CommandLine/Command.h"
#include "Foundation/CommandLine/Commands/Help.h"
#include "Foundation/CommandLine/Processor.h"
#include "Foundation/Reflect/Registry.h"

#include "Foundation/Startup.h"
#include "Foundation/Exception.h"
#include "Editor/ArtProvider.h"
#include "Foundation/Worker/Process.h"
#include "Foundation/Inspect/InspectInit.h"
#include "Foundation/Inspect/Interpreters/Reflect/InspectReflectInit.h"
#include "Editor/Perforce/Perforce.h"

#include "Core/CoreInit.h"
#include "Core/MRUData.h"

#include "Core/Scene/SceneInit.h"
#include "Core/Scene/Object.h"
#include "Core/Scene/Selectable.h"
#include "Core/Scene/Persistent.h"
#include "Core/Scene/PropertiesGenerator.h"

#include "Core/Scene/GridSettings.h"
#include "Core/Scene/SceneSettings.h"

#include "WindowSettings.h"
#include "Foundation/Document/Document.h"

#include "Editor/Tracker/Tracker.h"
#include "Editor/Task/TaskInit.h"
#include "Editor/Perforce/Perforce.h"
#include "Editor/PerforceWaitDialog.h"
#include "Editor/Vault/VaultSettings.h"

//#include "Commands/BuildCommand.h"
#include "Commands/ProfileDumpCommand.h"
#include "Commands/RebuildCommand.h"

#include <set>
#include <tchar.h>
#include <wx/wx.h>
#include <wx/choicdlg.h>
#include <wx/msw/private.h>
#include <wx/cmdline.h>
#include <wx/splash.h>
#include <wx/cshelp.h>

using namespace Helium;
using namespace Helium::Editor;
using namespace Helium::CommandLine;

static int ShowBreakpointDialog(const Debug::BreakpointArgs& args )
{
    static std::set<uintptr> disabled;
    static bool skipAll = false;
    bool skip = skipAll;

    // are we NOT skipping everything?
    if (!skipAll)
    {
        // have we disabled this break point?
        if (disabled.find(args.m_Info->ContextRecord->IPREG) != disabled.end())
        {
            skip = true;
        }
        // we have NOT disabled this break point yet
        else
        {
            Debug::ExceptionArgs exArgs ( Debug::ExceptionTypes::SEH, args.m_Fatal ); 
            Debug::GetExceptionDetails( args.m_Info, exArgs ); 

            // dump args.m_Info to console
            Helium::Print(Helium::ConsoleColors::Red, stderr, TXT( "%s" ), Debug::GetExceptionInfo(args.m_Info).c_str());

            // display result
            tstring message( TXT( "A break point was triggered in the application:\n\n" ) );
            message += Debug::GetSymbolInfo( args.m_Info->ContextRecord->IPREG );
            message += TXT("\n\nWhat do you wish to do?");

            const tchar* nothing = TXT( "Let the OS handle this as an exception" );
            const tchar* thisOnce = TXT( "Skip this break point once" );
            const tchar* thisDisable = TXT( "Skip this break point and disable it" );
            const tchar* allDisable = TXT( "Skip all break points" );

            wxArrayString choices;
            choices.Add(nothing);
            choices.Add(thisOnce);
            choices.Add(thisDisable);
            choices.Add(allDisable);
            wxString choice = ::wxGetSingleChoice( message.c_str(), TXT( "Break Point Triggered" ), choices );

            if (choice == nothing)
            {
                // we are not continuable, so unhook the top level filter
                SetUnhandledExceptionFilter( NULL );

                // this should let the OS prompt for the debugger
                return EXCEPTION_CONTINUE_SEARCH;
            }
            else if (choice == thisOnce)
            {
                skip = true;
            }
            else if (choice == thisDisable)
            {
                skip = true;
                disabled.insert(args.m_Info->ContextRecord->IPREG);
            }
            else if (choice == allDisable)
            {
                skip = true;
                skipAll = true;
            }
        }
    }

    if (skipAll || skip)
    {
        // skip break instruction (move the ip ahead one byte)
        args.m_Info->ContextRecord->IPREG += 1;

        // continue execution past the break instruction
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    else
    {
        // fall through and let window's crash API run
        return EXCEPTION_CONTINUE_SEARCH;
    }
}

namespace Helium
{
    namespace Editor
    {
        IMPLEMENT_APP( App );
    }
}

App::App()
#pragma TODO("This needs fixing otherwise dialogs will not be modal -Geoff")
: m_SettingsManager( new Core::SettingsManager() )
, m_VaultSearch( NULL )
, m_Frame( NULL )
{
}

App::~App()
{
    if ( m_VaultSearch )
    {
        delete m_VaultSearch;
        m_VaultSearch = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Called after OnInitCmdLine.  The base class handles the /help command line
// switch and exits.  If we get this far, we need to parse the command line
// and determine what mode to launch the app in.
// 
bool App::OnInit()
{
    SetVendorName( TXT( "Helium" ) );

    //parse.SetLogo( wxT( "Editor (c) 2010 - Helium\n" ) );

    // don't spend a lot of time updating idle events for windows that don't need it
    wxUpdateUIEvent::SetMode( wxUPDATE_UI_PROCESS_SPECIFIED );
    wxIdleEvent::SetMode( wxIDLE_PROCESS_SPECIFIED );

    tchar module[MAX_PATH];
    GetModuleFileName( 0, module, MAX_PATH );

    Helium::Path exePath( module );
    Helium::Path iconFolder( exePath.Directory() + TXT( "Icons/" ) );

    wxInitAllImageHandlers();
    wxImageHandler* curHandler = wxImage::FindHandler( wxBITMAP_TYPE_CUR );
    if ( curHandler )
    {
        // Force the cursor handler to the end of the list so that it doesn't try to
        // open TGA files.
        wxImage::RemoveHandler( curHandler->GetName() );
        curHandler = NULL;
        wxImage::AddHandler( new wxCURHandler );
    }

    ArtProvider* artProvider = new ArtProvider();
    wxArtProvider::Push( artProvider );

    wxSimpleHelpProvider* helpProvider = new wxSimpleHelpProvider();
    wxHelpProvider::Set( helpProvider );

    // libs
    Editor::PerforceWaitDialog::Enable( true );
    m_InitializerStack.Push( Perforce::Initialize, Perforce::Cleanup );
    m_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
    m_InitializerStack.Push( Inspect::Initialize, Inspect::Cleanup );
    m_InitializerStack.Push( InspectReflect::Initialize, InspectReflect::Cleanup );

    // core
    m_InitializerStack.Push( Core::Initialize, Core::Cleanup );
    m_InitializerStack.Push( Core::Object::InitializeType, Core::Object::CleanupType );
    m_InitializerStack.Push( Core::Selectable::InitializeType, Core::Selectable::CleanupType );
    m_InitializerStack.Push( Core::Persistent::InitializeType, Core::Persistent::CleanupType );
    m_InitializerStack.Push( Core::PropertiesGenerator::Initialize, Core::PropertiesGenerator::Cleanup );
    m_InitializerStack.Push( Reflect::RegisterClassType< Core::MRUData >( TXT( "Core::MRUData" ) ) );

    // task
#pragma TODO("Move init into here")
    m_InitializerStack.Push( TaskInitialize, TaskCleanup );

    // scene
#pragma TODO("Move init into here")
    m_InitializerStack.Push( Core::SceneInitialize, Core::SceneCleanup );

    // vault
    m_InitializerStack.Push( Reflect::RegisterEnumType<Editor::SearchTypes::SearchType>( &Editor::SearchTypes::SearchTypesEnumerateEnum, TXT( "SearchType" ) ) );
    m_InitializerStack.Push( Reflect::RegisterClassType<SearchQuery>( TXT( "SearchQuery" ) ) );

    // settings
    m_InitializerStack.Push( Reflect::RegisterClassType< Core::SettingsManager >( TXT( "Core::SettingsManager" ) ) ); 
    m_InitializerStack.Push( Reflect::RegisterClassType< Core::CameraSettings >( TXT( "Core::CameraSettings" ) ) ); 
    m_InitializerStack.Push( Reflect::RegisterClassType< Core::ViewportSettings >( TXT( "Core::ViewportSettings" ) ) ); 
    m_InitializerStack.Push( Reflect::RegisterClassType< Core::GridSettings >( TXT( "Core::GridSettings" ) ) );
    m_InitializerStack.Push( Reflect::RegisterClassType< Core::SceneSettings >( TXT( "Core::SceneSettings" ) ) );
    m_InitializerStack.Push( Reflect::RegisterClassType< WindowSettings >( TXT( "Editor::WindowSettings" ) ) );
    
    m_InitializerStack.Push( Reflect::RegisterEnumType<Editor::ViewOptionIDs::ViewOptionID>( &Editor::ViewOptionIDs::ViewOptionIDEnumerateEnum, TXT( "ViewOptionID" ) ) );
    m_InitializerStack.Push( Reflect::RegisterClassType< VaultSettings >( TXT( "Editor::VaultSettings" ) ) );

    LoadSettings();

    if ( Log::GetErrorCount() )
    {
        wxMessageBox( TXT( "There were errors during startup, use Editor with caution." ), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK );
    }

    GetFrame()->Show();

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Called when the application is being exited.  Cleans up resources.
// 
int App::OnExit()
{
    m_TrackerThread.Wait();

    SaveSettings();

    m_InitializerStack.Cleanup();

    wxImage::CleanUpHandlers();

    return __super::OnExit();
}

///////////////////////////////////////////////////////////////////////////////
// Called when an assert failure occurs
// 
void App::OnAssertFailure(const wxChar *file, int line, const wxChar *func, const wxChar *cond, const wxChar *msg)
{
    tstring function = func;

#pragma TODO("Remove post wxWidgets 2.9.0")
    if ( function == wxT("DoNotifyWindowAboutCaptureLost") )
    {
        return;
    }

    if ( wxStrcmp( msg, wxT( "invalid tool button bitmap" ) ) == 0 )
    {
        return;
    }

    HELIUM_BREAK();
}

void App::SaveSettings()
{
    Helium::Path path;
    Helium::GetPreferencesDirectory( path );
    path += TXT("EditorSettings.xml");

    tstring error;
    if ( Helium::IsDebuggerPresent() )
    {
        Reflect::Archive::ToFile( m_SettingsManager, path );
    }
    else
    {
        try
        {
            Reflect::Archive::ToFile( m_SettingsManager, path );
        }
        catch ( const Helium::Exception& ex )
        {
            error = ex.What();
        }

        if ( error.size() )
        {
            wxMessageBox( error.c_str(), wxT( "Error" ), wxOK | wxCENTER | wxICON_ERROR );
        }
    }
}

void App::LoadSettings()
{
    Helium::Path path;
    Helium::GetPreferencesDirectory( path );
    path += TXT("EditorSettings.xml");

	if ( !path.Exists() )
	{
		return;
	}

    Core::SettingsManagerPtr settingsManager = NULL;

    if ( Helium::IsDebuggerPresent() )
    {
		settingsManager = Reflect::Archive::FromFile< Core::SettingsManager >( path );
    }
    else
    {
        tstring error;
        try
        {
			settingsManager = Reflect::Archive::FromFile< Core::SettingsManager >( path );
        }
        catch ( const Helium::Exception& ex )
        {
            error = ex.What();
        }

        if ( error.size() )
        {
            wxMessageBox( error.c_str(), wxT( "Error" ), wxOK | wxCENTER | wxICON_ERROR );
        }
    }

    if ( settingsManager.ReferencesObject() )
    {
        m_SettingsManager = settingsManager;
    }
    else
    {
        wxMessageBox( TXT( "Unfortunately, we could not parse your existing settings.  Your settings have been reset to defaults.  We apologize for the inconvenience." ), wxT( "Error" ), wxOK | wxCENTER | wxICON_ERROR );
    }
}

#pragma TODO("Apparently wxWidgets doesn't support unicode command lines, please to fix in wxWidgets 2.9.x")
static int wxEntryWrapper(HINSTANCE hInstance, HINSTANCE hPrevInstance, tchar* pCmdLine, int nCmdShow)
{
    std::string cmdLine;
    Helium::ConvertString( pCmdLine, cmdLine );
    return wxEntry( hInstance, hPrevInstance, const_cast<char*>(cmdLine.c_str()), nCmdShow );
}

/////////////////////////////////////////////////////////////////////////////////
int Main ( int argc, const tchar** argv )
{
    // print physical memory
    MEMORYSTATUSEX status;
    memset(&status, 0, sizeof(status));
    status.dwLength = sizeof(status);
    ::GlobalMemoryStatusEx(&status);
    Log::Print( TXT( "Physical Memory: %I64u M bytes total, %I64u M bytes available\n" ), status.ullTotalPhys >> 20, status.ullAvailPhys >> 20);

    // fill out the options vector
    std::vector< tstring > options;
    for ( int i = 1; i < argc; ++i )
    {
        options.push_back( argv[ i ] );
    }
    std::vector< tstring >::const_iterator& argsBegin = options.begin(), argsEnd = options.end();

    bool success = true;
    tstring error; 


    Processor processor( TXT( "luna" ), TXT( "[COMMAND <ARGS>]" ), TXT( "Editor (c) 2010 - Helium" ) );

    //BuildCommand buildCommand;
    //success &= buildCommand.Initialize( error );
    //success &= processor.RegisterCommand( &buildCommand, error );

    ProfileDumpCommand profileDumpCommand;
    success &= profileDumpCommand.Initialize( error );
    success &= processor.RegisterCommand( &profileDumpCommand, error );

    RebuildCommand rebuildCommand;
    success &= rebuildCommand.Initialize( error );
    success &= processor.RegisterCommand( &rebuildCommand, error );

    Helium::CommandLine::Help helpCommand;
    helpCommand.SetOwner( &processor );
    success &= helpCommand.Initialize( error );
    success &= processor.RegisterCommand( &helpCommand, error );

    //success &= processor.AddOption( new FlagOption(  , "pipe", "use pipe for console connection" ), error ); 

    bool disableTracker = false;
    success &= processor.AddOption( new FlagOption( &disableTracker, TXT( "disable_tracker" ), TXT( "disable Asset Tracker" ) ), error );
    //GetAppSettings()->UseTracker( disableTracker );

    //success &= processor.AddOption( new FlagOption(  , WindowSettings::s_Reset, "reset all window positions" ), error );
    //success &= processor.AddOption( new FlagOption(  , Settings::s_ResetSettings, "resets all preferences for all of Editor" ), error );

    //success &= processor.AddOption( new FlagOption(  , Worker::Args::Debug, "debug use of background processes" ), error );
    //success &= processor.AddOption( new FlagOption(  , Worker::Args::Wait, "wait forever for background processes" ), error );

    bool scriptFlag = false;
    success &= processor.AddOption( new FlagOption( &scriptFlag, StartupArgs::Script, TXT( "omit prefix and suffix in console output" ) ), error );

    bool attachFlag = false;
    success &= processor.AddOption( new FlagOption( &attachFlag, StartupArgs::Attach, TXT( "wait for a debugger to attach to the process on startup" ) ), error );

    bool profileFlag = false;
    success &= processor.AddOption( new FlagOption( &profileFlag, StartupArgs::Profile, TXT( "enable profile output to the console windows" ) ), error );

    bool memoryFlag = false;
    success &= processor.AddOption( new FlagOption( &memoryFlag, StartupArgs::Memory, TXT( "profile and report memory usage to the console" ) ), error );

    bool vreboseFlag = false;
    success &= processor.AddOption( new FlagOption( &vreboseFlag, StartupArgs::Verbose, TXT( "output a verbose level of console output" ) ), error );

    bool extremeFlag = false;
    success &= processor.AddOption( new FlagOption( &extremeFlag, StartupArgs::Extreme, TXT( "output an extremely verbose level of console output" ) ), error );

    bool debugFlag = false;
    success &= processor.AddOption( new FlagOption( &debugFlag, StartupArgs::Debug, TXT( "output debug console output" ) ), error );

    int nice = 0;
    success &= processor.AddOption( new SimpleOption<int>( &nice , TXT( "nice" ), TXT( "<NUM>" ), TXT( "number of processors to nice (for other processes)" ) ), error );

    bool helpFlag;
    success &= processor.AddOption( new FlagOption( &helpFlag, TXT( "h|help" ), TXT( "print program usage" ) ), error );

    success &= processor.ParseOptions( argsBegin, argsEnd, error );

    if ( success )
    {
        if ( helpFlag )
        {
            Log::Print( TXT( "\nPrinting help for Editor...\n" ) );
            Log::Print( processor.Help().c_str() );
            Log::Print( TXT( "\n" ) );
            success = true;
        }
        else if ( argsBegin != argsEnd )
        {
            while ( success && ( argsBegin != argsEnd ) )
            {
                const tstring& arg = (*argsBegin);
                ++argsBegin;

                if ( arg.length() < 1 )
                {
                    continue;
                }

                if ( arg[ 0 ] == '-' )
                {
                    error = TXT( "Unknown option, or option passed out of order: " ) + arg;
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
                        error = TXT( "Unknown commandline parameter: " ) + arg + TXT( "\n\n" );
                        success = false;
                    }
                }
            }
        }
        else
        {
            //buildCommand.Cleanup();
            rebuildCommand.Cleanup();

#ifndef _DEBUG
            ::FreeConsole();
#endif

            return Helium::StandardWinMain( &wxEntryWrapper );
        }
    }

    //buildCommand.Cleanup();
    rebuildCommand.Cleanup();

    if ( !success && !error.empty() )
    {
        Log::Error( TXT( "%s\n" ), error.c_str() );
    }

    return success ? 0 : 1;
}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for the application.
//
int _tmain( int argc, const tchar** argv )
{
    Helium::InitializerStack initializerStack( true );

    Debug::g_BreakpointOccurred.Set( &ShowBreakpointDialog );

    int result = Helium::StandardMain( &Main, argc, argv );

    Debug::g_BreakpointOccurred.Clear();

    return result;
}
