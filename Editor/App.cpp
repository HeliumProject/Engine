#include "EditorPch.h"
#include "App.h"

#include "Platform/Process.h"
#include "Platform/Exception.h"
#include "Platform/Trace.h"
#include "Platform/Timer.h"
#include "Platform/Console.h"
#include "Platform/Timer.h"

#include "Foundation/Log.h"
#include "Application/Startup.h"
#include "Foundation/Exception.h"
#include "Application/InitializerStack.h"
#include "Application/CmdLineProcessor.h"
#include "Application/DocumentManager.h"
#include "Engine/FileLocations.h"
#include "Foundation/Name.h"

#include "Reflect/Registry.h"

#include "Inspect/Inspect.h"
#include "Inspect/Interpreters/InspectReflectInit.h"

#include "Foundation/Math.h"

#include "Engine/AsyncLoader.h"
#include "Engine/CacheManager.h"
#include "Engine/Config.h"
#include "Engine/Asset.h"
#include "Engine/JobManager.h"
#include "Framework/TaskScheduler.h"

#include "EngineJobs/EngineJobs.h"

#include "GraphicsJobs/GraphicsJobs.h"

#include "PcSupport/ConfigPc.h"
#include "PcSupport/AssetPreprocessor.h"
#include "PcSupport/PlatformPreprocessor.h"

#include "PreprocessingPc/PcPreprocessor.h"

#include "PcSupport/LooseAssetLoader.h"
#include "EditorSupport/EditorSupportPch.h"
#include "EditorSupport/FontResourceHandler.h"

#include "Framework/WorldManager.h"

#include "SceneGraph/SceneGraphInit.h"
#include "SceneGraph/SettingsManager.h"

#include "Editor/ArtProvider.h"
#include "Editor/Input.h"
#include "Editor/EditorGeneratedWrapper.h"
#include "Editor/Perforce/Perforce.h"
#include "Editor/ProjectViewModel.h"
#include "Editor/Settings/EditorSettings.h"
#include "Editor/Settings/WindowSettings.h"
#include "Editor/Tracker.h"
#include "Editor/Perforce/Perforce.h"
#include "Editor/Dialogs/PerforceWaitDialog.h"
#include "Editor/Vault/VaultSettings.h"

#include "Editor/Commands/ProfileDumpCommand.h"

#include "Editor/Clipboard/ClipboardDataWrapper.h"
#include "Editor/Clipboard/ClipboardFileList.h"

#include "Editor/Inspect/Widgets/DrawerWidget.h"
#include "Editor/Inspect/Widgets/LabelWidget.h"
#include "Editor/Inspect/Widgets/ValueWidget.h"
#include "Editor/Inspect/Widgets/SliderWidget.h"
#include "Editor/Inspect/Widgets/ChoiceWidget.h"
#include "Editor/Inspect/Widgets/CheckBoxWidget.h"
#include "Editor/Inspect/Widgets/ColorPickerWidget.h"
#include "Editor/Inspect/Widgets/ListWidget.h"
#include "Editor/Inspect/Widgets/ButtonWidget.h"
#include "Editor/Inspect/Widgets/FileDialogButtonWidget.h"
#include "Editor/Inspect/TreeCanvas.h"
#include "Editor/Inspect/TreeCanvasWidget.h"
#include "Editor/Inspect/StripCanvas.h"
#include "Editor/Inspect/StripCanvasWidget.h"

#pragma TODO("Support dynamic loading of game types in the editor")
#include "Bullet/BulletPch.h"
#include "ExampleGame/ExampleGamePch.h"
#include "Components/ComponentsPch.h"

#include <set>
#include <wx/wx.h>
#include <wx/choicdlg.h>
#include <wx/cmdline.h>
#include <wx/splash.h>
#include <wx/cshelp.h>

#if HELIUM_OS_WIN
# include <wx/msw/private.h>
#endif

using namespace Helium;
using namespace Helium::Editor;
using namespace Helium::CommandLine;

#if HELIUM_OS_WIN

static void ShowBreakpointDialog(const Helium::BreakpointArgs& args )
{
    static std::set<uintptr_t> disabled;
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
            Helium::ExceptionArgs exArgs ( Helium::ExceptionTypes::Structured, args.m_Fatal ); 
            Helium::GetExceptionDetails( args.m_Info, exArgs ); 

            // dump args.m_Info to console
            Helium::Print(Helium::ConsoleColors::Red, stderr, TXT( "%s" ), Helium::GetExceptionInfo(args.m_Info).c_str());

            // display result
            std::string message( TXT( "A break point was triggered in the application:\n\n" ) );
            message += Helium::GetSymbolInfo( args.m_Info->ContextRecord->IPREG );
            message += TXT("\n\nWhat do you wish to do?");

            const char* nothing = TXT( "Let the OS handle this as an exception" );
            const char* thisOnce = TXT( "Skip this break point once" );
            const char* thisDisable = TXT( "Skip this break point and disable it" );
            const char* allDisable = TXT( "Skip all break points" );

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
                args.m_Result = EXCEPTION_CONTINUE_SEARCH;
                return;
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
        args.m_Result = EXCEPTION_CONTINUE_EXECUTION;
    }
    else
    {
        // fall through and let window's crash API run
        args.m_Result = EXCEPTION_CONTINUE_SEARCH;
    }
}

#endif

namespace Helium
{
    namespace Editor
    {
        IMPLEMENT_APP( App );
    }
}

#ifdef IDLE_LOOP
BEGIN_EVENT_TABLE( App, wxApp )
EVT_IDLE( App::OnIdle )
END_EVENT_TABLE()
#endif

App::App()
: m_Running( false )
, m_AppVersion( HELIUM_APP_VERSION )
, m_AppName( HELIUM_APP_NAME )
, m_AppVerName( HELIUM_APP_VER_NAME )
, m_SettingsManager( new SettingsManager() )
#pragma TODO("This needs fixing otherwise dialogs will not be modal -Geoff")
, m_Frame( NULL )
{
}

App::~App()
{
}

///////////////////////////////////////////////////////////////////////////////
// Called after OnInitCmdLine.  The base class handles the /help command line
// switch and exits.  If we get this far, we need to parse the command line
// and determine what mode to launch the app in.
// 
bool App::OnInit()
{
	ForceLoadBulletDll();
	ForceLoadComponentsDll();
	ForceLoadExampleGameDll();

    SetVendorName( HELIUM_APP_NAME );

	ForceLoadEditorSupportDll();

    Timer::StaticInitialize();
#if !HELIUM_RELEASE && !HELIUM_PROFILE
    Helium::InitializeSymbols();
#endif

    // don't spend a lot of time updating idle events for windows that don't need it
    wxUpdateUIEvent::SetMode( wxUPDATE_UI_PROCESS_SPECIFIED );
    wxIdleEvent::SetMode( wxIDLE_PROCESS_SPECIFIED );

    Helium::FilePath exePath( GetProcessPath() );
    Helium::FilePath iconFolder( exePath.Directory() + TXT( "Icons/" ) );

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

    // Make sure various module-specific heaps are initialized from the main thread before use.
    InitEngineJobsDefaultHeap();
    InitGraphicsJobsDefaultHeap();

    // Register shutdown for general systems.
    m_InitializerStack.Push( FileLocations::Shutdown );
    m_InitializerStack.Push( Name::Shutdown );
    m_InitializerStack.Push( AssetPath::Shutdown );

    // Async I/O.
    AsyncLoader& asyncLoader = AsyncLoader::GetStaticInstance();
    HELIUM_VERIFY( asyncLoader.Initialize() );
    m_InitializerStack.Push( AsyncLoader::DestroyStaticInstance );

    // Asset cache management.
    FilePath baseDirectory;
    if ( !FileLocations::GetBaseDirectory( baseDirectory ) )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "Could not get base directory." ) );
        return false;
    }

    HELIUM_VERIFY( CacheManager::InitializeStaticInstance( baseDirectory ) );
    m_InitializerStack.Push( CacheManager::DestroyStaticInstance );
	
    // libs
    Editor::PerforceWaitDialog::EnableWaitDialog( true );
    m_InitializerStack.Push( Perforce::Initialize, Perforce::Cleanup );
    m_InitializerStack.Push( Reflect::ObjectRefCountSupport::Shutdown );
    m_InitializerStack.Push( Asset::Shutdown );
    m_InitializerStack.Push( AssetType::Shutdown );
    m_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
    m_InitializerStack.Push( Inspect::Initialize, Inspect::Cleanup );
    m_InitializerStack.Push( InspectReflect::Initialize, InspectReflect::Cleanup );
    m_InitializerStack.Push( SceneGraph::Initialize,  SceneGraph::Cleanup );
    Helium::TaskScheduler::CalculateSchedule();
	Helium::Components::Initialize( NULL );
    m_InitializerStack.Push( Components::Cleanup );
    
    // Asset loader and preprocessor.
    HELIUM_VERIFY( LooseAssetLoader::InitializeStaticInstance() );
    m_InitializerStack.Push( LooseAssetLoader::DestroyStaticInstance );

    AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
    HELIUM_ASSERT( pAssetLoader );

    AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::CreateStaticInstance();
    HELIUM_ASSERT( pAssetPreprocessor );
    PlatformPreprocessor* pPlatformPreprocessor = new PcPreprocessor;
    HELIUM_ASSERT( pPlatformPreprocessor );
    pAssetPreprocessor->SetPlatformPreprocessor( Cache::PLATFORM_PC, pPlatformPreprocessor );

    m_InitializerStack.Push( AssetPreprocessor::DestroyStaticInstance );

    // Engine configuration.
    Config& rConfig = Config::GetStaticInstance();
    rConfig.BeginLoad();
    while( !rConfig.TryFinishLoad() )
    {
        pAssetLoader->Tick();
    }

    m_InitializerStack.Push( Config::DestroyStaticInstance );

    ConfigPc::SaveUserConfig();

    // Job manager.
    JobManager& rJobManager = JobManager::GetStaticInstance();
    HELIUM_VERIFY( rJobManager.Initialize() );
    m_InitializerStack.Push( JobManager::DestroyStaticInstance );

    LoadSettings();

    if ( Log::GetErrorCount() )
    {
        wxMessageBox( TXT( "There were errors during startup, use Editor with caution." ), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK );
    }

    Connect( wxEVT_CHAR, wxKeyEventHandler( App::OnChar ), NULL, this );

    m_Frame = new MainFrame( m_SettingsManager );

#if HELIUM_OS_WIN
    m_Engine.Initialize( &m_Frame->GetSceneManager(), GetHwndOf( m_Frame ) );
#else
    m_Engine.Initialize( &m_Frame->GetSceneManager(), NULL );
#endif

    HELIUM_VERIFY( m_Frame->Initialize() );
    m_Frame->Show();

    if ( GetSettingsManager()->GetSettings< EditorSettings >()->GetReopenLastProjectOnStartup() )
    {
        const std::vector< std::string >& mruPaths = wxGetApp().GetSettingsManager()->GetSettings<EditorSettings>()->GetMRUProjects();
        if ( !mruPaths.empty() )
        {
            FilePath projectPath( *mruPaths.rbegin() );
            if ( projectPath.Exists() )
            {
                m_Frame->OpenProject( FilePath( *mruPaths.rbegin() ) );
            }
        }
    }

    return true;
}

int App::OnRun()
{
    m_Running = true;

    return wxApp::OnRun();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the application is being exited.  Cleans up resources.
// 
int App::OnExit()
{
    Disconnect( wxEVT_CHAR, wxKeyEventHandler( App::OnChar ), NULL, this );

    SaveSettings();

    m_Engine.Shutdown();

    m_SettingsManager.Release();

    m_InitializerStack.Cleanup();

    wxImage::CleanUpHandlers();

    int result = wxApp::OnExit();

    // Always clear out memory heaps last.
    ThreadLocalStackAllocator::ReleaseMemoryHeap();

    return result;
}

void App::OnChar( wxKeyEvent& event )
{
    if ( !m_Frame )
    {
        return;
    }

    Helium::KeyboardInput input;
    Helium::ConvertEvent( event, input );
    std::string error;

    if ( input.IsCtrlDown() )
    {
        switch( input.GetKeyCode() )
        {
        case KeyCodes::a: // ctrl-a
            {
                wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, wxID_SELECTALL );
                m_Frame->GetEventHandler()->ProcessEvent( evt );
                event.Skip( false );
                return;
            }

        case KeyCodes::i: // ctrl-i
            {
                m_Frame->InvertSelection();
                event.Skip( false );
                return;
            }

        case KeyCodes::o: // ctrl-o
            {
                m_Frame->OpenProjectDialog();
                event.Skip( false );
                return;
            }

        case KeyCodes::s: // ctrl-s
            {
                if ( !m_Frame->SaveAll( error ) )
                {
                    wxMessageBox( error.c_str(), wxT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, m_Frame );
                }
                event.Skip( false );
                return;
            }

        case KeyCodes::v: // ctrl-v
            {
                wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, wxID_PASTE );
                m_Frame->GetEventHandler()->ProcessEvent( evt );
                event.Skip( false );
                return;
            }

        case KeyCodes::w: // ctrl-w
            {
                m_Frame->CloseProject();
                event.Skip( false );
                return;
            }

        case KeyCodes::x: // ctrl-x
            {
                wxCommandEvent evt ( wxEVT_COMMAND_MENU_SELECTED, wxID_CUT );
                m_Frame->GetEventHandler()->ProcessEvent( evt );
                event.Skip( false );
                return;
            }

        default:
            break;
        }
    }
}

#ifdef IDLE_LOOP
void App::OnIdle( wxIdleEvent& event )
{
    if ( m_Running )
    {
        WorldManager& rWorldManager = WorldManager::GetStaticInstance();
        rWorldManager.Update();
    }
}
#endif

///////////////////////////////////////////////////////////////////////////////
// Called when an assert failure occurs
// 
void App::OnAssertFailure(const wxChar *file, int line, const wxChar *func, const wxChar *cond, const wxChar *msg)
{
    HELIUM_BREAK();
}

///////////////////////////////////////////////////////////////////////////////
// Called when an exception occurs in the process of dispatching events
//  It is Helium's policy to not throw C++ exceptions into wxWidgets
//  If this is a Win32/SEH exception then set your debugger to break
//   on throw instead of break on user-unhandled
// 
void App::OnUnhandledException()
{
    HELIUM_BREAK();
}

///////////////////////////////////////////////////////////////////////////////
// See above
//
bool App::OnExceptionInMainLoop()
{
    HELIUM_BREAK();
    throw;
}

void App::SaveSettings()
{
    Helium::FilePath path;
    Helium::GetPreferencesDirectory( path );
    path += TXT("EditorSettings.json");

    std::string error;

    if ( !path.MakePath() )
    {
        error = std::string( TXT( "Could not save '" ) ) + path.c_str() + TXT( "': We could not create the directory to store the settings file." );
        wxMessageBox( error.c_str(), wxT( "Error" ), wxOK | wxCENTER | wxICON_ERROR );
        return;
    }

    if ( Helium::IsDebuggerPresent() )
    {
        Persist::ToArchive( path, m_SettingsManager );
    }
    else
    {
        if ( !Persist::ToArchive( path, m_SettingsManager ) )
        {
            error = std::string( TXT( "Could not save '" ) ) + path.c_str() + TXT( "'." );
            wxMessageBox( error.c_str(), wxT( "Error" ), wxOK | wxCENTER | wxICON_ERROR );
        }
    }
}

void App::LoadSettings()
{
    Helium::FilePath path;
    Helium::GetPreferencesDirectory( path );
    path += TXT("EditorSettings.xml"); // Leaving this as XML for now until I can figure out why the well-formed .json file is not parsing. I suspect something subtle with translating maps with pointers.

    if ( !path.Exists() )
    {
        return;
    }

	SettingsManagerPtr settingsManager = Reflect::SafeCast< SettingsManager >( Persist::FromArchive( path ) );
    if ( settingsManager.ReferencesObject() )
    {
        settingsManager->Clean();
        m_SettingsManager = settingsManager;
    }
    else
    {
        wxMessageBox( TXT( "Unfortunately, we could not parse your existing settings.  Your settings have been reset to defaults.  We apologize for the inconvenience." ), wxT( "Error" ), wxOK | wxCENTER | wxICON_ERROR );
    }
}

#if HELIUM_OS_WIN
#pragma TODO("Apparently wxWidgets doesn't support unicode command lines, please to fix in wxWidgets 2.9.x")
static int wxEntryWrapper(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow)
{
    std::string cmdLine;
    Helium::ConvertString( pCmdLine, cmdLine );
    return wxEntry( hInstance, hPrevInstance, const_cast<char*>(cmdLine.c_str()), nCmdShow );
}
#else
static int wxEntryWrapper(int argc, const char **argv)
{
    return wxEntry( argc, const_cast<char**>( argv ) );
}
#endif

/////////////////////////////////////////////////////////////////////////////////
int Main( int argc, const char** argv )
{
#if HELIUM_OS_WIN
    // print physical memory
    MEMORYSTATUSEX status;
    memset(&status, 0, sizeof(status));
    status.dwLength = sizeof(status);
    ::GlobalMemoryStatusEx(&status);
    Log::Print( TXT( "Physical Memory: %I64u M bytes total, %I64u M bytes available\n" ), status.ullTotalPhys >> 20, status.ullAvailPhys >> 20);
#endif

    // fill out the options vector
    std::vector< std::string > options;
    for ( int i = 1; i < argc; ++i )
    {
        options.push_back( argv[ i ] );
    }
    std::vector< std::string >::const_iterator argsBegin = options.begin(), argsEnd = options.end();

    bool success = true;
    std::string error; 

    Processor processor( TXT( "luna" ), TXT( "[COMMAND <ARGS>]" ), TXT( "Editor (c) 2010 - Helium" ) );

    ProfileDumpCommand profileDumpCommand;
    success &= profileDumpCommand.Initialize( error );
    success &= processor.RegisterCommand( &profileDumpCommand, error );

	Helium::CommandLine::HelpCommand helpCommand;
    helpCommand.SetOwner( &processor );
    success &= helpCommand.Initialize( error );
    success &= processor.RegisterCommand( &helpCommand, error );

    //success &= processor.AddOption( new FlagOption(  , "pipe", "use pipe for console connection" ), error ); 

    bool disableTracker = false;
    success &= processor.AddOption( new FlagOption( &disableTracker, TXT( "disable_tracker" ), TXT( "disable Asset Tracker" ) ), error );
    if ( disableTracker )
    {
        wxGetApp().GetSettingsManager()->GetSettings< EditorSettings >()->SetEnableAssetTracker( false );
    }

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

    bool verboseFlag = false;
    success &= processor.AddOption( new FlagOption( &verboseFlag, StartupArgs::Verbose, TXT( "output a verbose level of console output" ) ), error );

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
                const std::string& arg = (*argsBegin);
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
#ifndef _DEBUG
            ::FreeConsole();
#endif

#if HELIUM_OS_WIN
            return Helium::StandardWinMain( &wxEntryWrapper );
#else
            return Helium::StandardMain( &wxEntryWrapper, argc, argv );
#endif
        }
    }

    if ( !success && !error.empty() )
    {
        Log::Error( TXT( "%s\n" ), error.c_str() );
    }

    return success ? 0 : 1;
}


///////////////////////////////////////////////////////////////////////////////
// Main entry point for the application.
//
#if HELIUM_OS_WIN
int wmain( int argc, const wchar_t** argv )
#else
int main( int argc, const char* argv[] )
#endif
{
#if HELIUM_OS_WIN
	std::vector< std::string > strings;
	const char** av = (const char**)alloca( argc * sizeof( const char* ) );
	for ( int i=0; i<argc; i++ )
	{
		strings.push_back( std::string() );
		ConvertString( argv[i], strings.back() );
		av[i] = strings.back().c_str();
	}
#endif

    Helium::InitializerStack initializerStack( true );

#if HELIUM_OS_WIN
    Helium::g_BreakpointOccurred.Set( &ShowBreakpointDialog );
#endif

#if HELIUM_OS_WIN
    int result = Helium::StandardMain( &Main, argc, av );
#else
    int result = Helium::StandardMain( &Main, argc, argv );
#endif

#if HELIUM_OS_WIN
    Helium::g_BreakpointOccurred.Clear();
#endif

    return result;
}
