#include "Precompile.h"
#include "App.h"

#include "Platform/Process.h"
#include "Platform/Exception.h"
#include "Platform/Trace.h"
#include "Platform/Timer.h"
#include "Platform/Console.h"
#include "Platform/Timer.h"

#include "Foundation/Exception.h"
#include "Foundation/Log.h"
#include "Foundation/Math.h"
#include "Foundation/Name.h"

#include "Reflect/Registry.h"

#include "Application/Startup.h"
#include "Application/InitializerStack.h"
#include "Application/CmdLineProcessor.h"
#include "Application/DocumentManager.h"

#include "EditorSupport/Precompile.h"
#include "EditorSupport/FontResourceHandler.h"

#include "EditorScene/EditorSceneInit.h"
#include "EditorScene/SettingsManager.h"

#include "Editor/ArtProvider.h"
#include "Editor/Input.h"
#include "Editor/EditorGeneratedWrapper.h"
#include "Editor/Perforce/Perforce.h"
#include "Editor/ProjectViewModel.h"
#include "Editor/Settings/EditorSettings.h"
#include "Editor/Settings/WindowSettings.h"
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

bool g_HelpFlag = false;
bool g_DisableTracker = false;

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
// TODO: This needs fixing otherwise dialogs will not be modal -geoff
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
	SetVendorName( HELIUM_APP_NAME );

#if !HELIUM_RELEASE && !HELIUM_PROFILE
	HELIUM_TRACE_SET_LEVEL( TraceLevels::Debug );
	Helium::InitializeSymbols();
#endif

	// Initialize sibling dynamically loaded modules.
	Helium::FilePath path ( Helium::GetProcessPath() );
	for ( DirectoryIterator itr ( FilePath( path.Directory() ) ); !itr.IsDone(); itr.Next() )
	{
		std::string ext = itr.GetItem().m_Path.Extension();
		if ( ext == HELIUM_MODULE_EXTENSION )
		{
			ModuleHandle module = LoadModule( itr.GetItem().m_Path.Data() );
			HELIUM_ASSERT( module != HELIUM_INVALID_MODULE );
		}
	}

	// don't spend a lot of time updating idle events for windows that don't need it
	wxUpdateUIEvent::SetMode( wxUPDATE_UI_PROCESS_SPECIFIED );
	wxIdleEvent::SetMode( wxIDLE_PROCESS_SPECIFIED );

	Helium::FilePath exePath( GetProcessPath() );
	Helium::FilePath iconFolder( exePath.Directory() + "Icons/" );

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
	
	Editor::PerforceWaitDialog::EnableWaitDialog( true );
	Perforce::Startup();
	Reflect::Startup();

	LoadSettings();

	Connect( wxEVT_CHAR, wxKeyEventHandler( App::OnChar ), NULL, this );

	m_Frame = new MainFrame( m_SettingsManager );

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

///////////////////////////////////////////////////////////////////////////////
// Run the main application message pump
// 
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

	Reflect::Shutdown();
	Perforce::Shutdown();
	Editor::PerforceWaitDialog::EnableWaitDialog( false );

	m_SettingsManager.Release();

	wxImage::CleanUpHandlers();

	int result = wxApp::OnExit();

	// Always clear out memory heaps last.
	ThreadLocalStackAllocator::ReleaseMemoryHeap();

	return result;
}

void App::OnChar( wxKeyEvent& event )
{
	// It seems like this is swallowing all events to all text fields.. disabling for now
	event.Skip();
	return;

#if KEYBOARD_REFACTOR
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
#endif
}

#ifdef IDLE_LOOP
void App::OnIdle( wxIdleEvent& event )
{
	if ( m_Running )
	{
		WorldManager* pWorldManager = WorldManager::GetInstance();
		HELIUM_ASSERT( pWorldManager );
		pWorldManager->Update();
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

#if wxUSE_EXCEPTIONS

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

bool App::OnExceptionInMainLoop()
{
	HELIUM_BREAK();
	throw;
}

#endif

void App::SaveSettings()
{
	Helium::FilePath path;
	Helium::GetPreferencesDirectory( path );
	path += "EditorSettings.json";

	std::string error;

	if ( !path.MakePath() )
	{
		error = std::string( "Could not save '" ) + path.Get() + "': We could not create the directory to store the settings file.";
		wxMessageBox( error.c_str(), wxT( "Error" ), wxOK | wxCENTER | wxICON_ERROR );
		return;
	}

	if ( Helium::IsDebuggerPresent() )
	{
		Persist::ArchiveWriter::WriteToFile( path, m_SettingsManager.Ptr() );
	}
	else
	{
		if ( !Persist::ArchiveWriter::WriteToFile( path, m_SettingsManager.Ptr() ) )
		{
			error = std::string( "Could not save '" ) + path.Get() + "'.";
			wxMessageBox( error.c_str(), wxT( "Error" ), wxOK | wxCENTER | wxICON_ERROR );
		}
	}
}

void App::LoadSettings()
{
	Helium::FilePath path;
	Helium::GetPreferencesDirectory( path );
	path += "EditorSettings.json";

	if ( !path.Exists() )
	{
		return;
	}

	SettingsManagerPtr settingsManager = Reflect::SafeCast< SettingsManager >( Persist::ArchiveReader::ReadFromFile( path ) );
	if ( settingsManager.ReferencesObject() )
	{
		settingsManager->Clean();
		m_SettingsManager = settingsManager;
	}
	else
	{
		wxMessageBox( wxT("Unfortunately, we could not parse your existing settings.  Your settings have been reset to defaults.  We apologize for the inconvenience."), wxT( "Error" ), wxOK | wxCENTER | wxICON_ERROR );
	}
}

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
			Helium::Print(Helium::ConsoleColors::Red, stderr, "%s", Helium::GetExceptionInfo(args.m_Info).c_str());

			// display result
			std::string message( "A break point was triggered in the application:\n\n" );
			message += Helium::GetSymbolInfo( args.m_Info->ContextRecord->IPREG );
			message += "\n\nWhat do you wish to do?";

			const char* nothing = "Let the OS handle this as an exception";
			const char* thisOnce = "Skip this break point once";
			const char* thisDisable = "Skip this break point and disable it";
			const char* allDisable = "Skip all break points";

			wxArrayString choices;
			choices.Add(nothing);
			choices.Add(thisOnce);
			choices.Add(thisDisable);
			choices.Add(allDisable);
			wxString choice = ::wxGetSingleChoice( message.c_str(), "Break Point Triggered", choices );

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

#endif // HELIUM_OS_WIN

///////////////////////////////////////////////////////////////////////////////
// A top level routine to parse arguments before we boot up wx via our
//  custom exception-handling entry points
// 
int Main( int argc, const char** argv )
{
	std::vector< std::string > options;
	for ( int i = 1; i < argc; ++i )
	{
		options.push_back( argv[ i ] );
	}
	std::vector< std::string >::const_iterator argsBegin = options.begin(), argsEnd = options.end();

	bool success = true;
	std::string error; 

	Processor processor( "Helium-Tools-Editor", "[COMMAND <ARGS>]", "Editor (c) 20xx - Helium" );

	ProfileDumpCommand profileDumpCommand;
	success &= profileDumpCommand.Initialize( error );
	success &= processor.RegisterCommand( &profileDumpCommand, error );

	Helium::CommandLine::HelpCommand helpCommand;
	helpCommand.SetOwner( &processor );
	success &= helpCommand.Initialize( error );
	success &= processor.RegisterCommand( &helpCommand, error );

	success &= processor.AddOption( new FlagOption( &g_HelpFlag, "h|help", "print program usage" ), error );
	success &= processor.AddOption( new FlagOption( &g_DisableTracker, "disable_tracker", "disable Asset Tracker" ), error );
	success &= processor.ParseOptions( argsBegin, argsEnd, error );

	if ( success )
	{
		if ( g_HelpFlag )
		{
			// TODO: This needs to be a message box, it will never be seen in release builds
			Log::Print( "\nPrinting help for Editor...\n" );
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
					error = "Unknown option, or option passed out of order: " + arg;
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
						error = "Unknown commandline parameter: " + arg + "\n\n";
						success = false;
					}
				}
			}
		}
		else
		{
#if HELIUM_OS_WIN
			HELIUM_CONVERT_TO_CHAR( ::GetCommandLineW(), convertedCmdLine );
			return wxEntry( ::GetModuleHandle(NULL), NULL, convertedCmdLine, SW_SHOWNORMAL );
#else // HELIUM_OS_WIN
			return wxEntry( argc, const_cast<char**>( argv ) );
#endif // HELIUM_OS_WIN
		}
	}

	if ( !success && !error.empty() )
	{
		Log::Error( "%s\n", error.c_str() );
	}

	return success ? 0 : 1;
}


///////////////////////////////////////////////////////////////////////////////
// The actual os entry point function
// 
#if HELIUM_OS_WIN
int wmain( int argc, const wchar_t** argv )
#else // HELIUM_OS_WIN
int main( int argc, const char* argv[] )
#endif // HELIUM_OS_WIN
{
	int result = 0;

#if HELIUM_OS_WIN
	
	// convert wchar_t argc/argv into UTF-8
	std::vector< std::string > strings;
	const char** av = (const char**)alloca( argc * sizeof( const char* ) );
	for ( int i=0; i<argc; i++ )
	{
		strings.push_back( std::string() );
		ConvertString( argv[i], strings.back() );
		av[i] = strings.back().c_str();
	}

	// attach a callback to the global breakpoint exception event
	Helium::g_BreakpointOccurred.Set( &ShowBreakpointDialog );

	result = Helium::StandardMain( &Main, argc, av );

	// release our callback for handling breakpoint exceptions
	Helium::g_BreakpointOccurred.Clear();

#else // HELIUM_OS_WIN

	// hooray for UTF-8 sanity on the part of all non-windows os-es
	result = Helium::StandardMain( &Main, argc, argv );

#endif // HELIUM_OS_WIN

	return result;
}
