#include "PipelinePch.h"
#include "PipelineApp.h"

#include "Platform/Process.h"
#include "Platform/Encoding.h"

#include "Foundation/Exception.h"
#include "Foundation/DirectoryIterator.h"

#include "Application/Startup.h"
#include "Application/InitializerStack.h"
#include "Application/CmdLineProcessor.h"
#include "ApplicationUI/ArtProvider.h"

#include "Pipeline/PipelineEngine.h"

#include <wx/wx.h>
#include <wx/choicdlg.h>
#include <wx/cmdline.h>
#include <wx/splash.h>
#include <wx/cshelp.h>

#if HELIUM_OS_WIN
# include <wx/msw/private.h>
#endif

using namespace Helium;
using namespace Helium::Pipeline;
using namespace Helium::CommandLine;

bool g_HelpFlag = false;
bool g_DisableTracker = false;

namespace Helium
{
	namespace Pipeline
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
: m_AppVersion( HELIUM_APP_VERSION )
, m_AppName( HELIUM_APP_NAME )
, m_AppVerName( HELIUM_APP_VER_NAME )
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
	Helium::InitializeSymbols();
#endif

#if HELIUM_SHARED
	// Initialize sibling dynamically loaded modules.
	FilePath path ( "C:\\helium\\Bin\\Debug\\Helium-Tools-Editor.exe" );
	for ( DirectoryIterator itr ( FilePath( path.Directory() ) ); !itr.IsDone(); itr.Next() )
	{
		std::string ext = itr.GetItem().m_Path.Extension();
		if ( ext == HELIUM_MODULE_EXTENSION )
		{
			ModuleHandle module = LoadModule( itr.GetItem().m_Path.c_str() );
			HELIUM_ASSERT( module != HELIUM_INVALID_MODULE );
		}
	}
#else
	// TODO: call into generated code
	HELIUM_ASSERT( false );
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

	if (!PipelineEngine::Init())
	{
		return false;
	}

	m_InitializerStack.Push( PipelineEngine::Cleanup );
	m_TrayIcon.Initialize();

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// Run the main application message pump
// 
int App::OnRun()
{
	return wxApp::OnRun();
}

///////////////////////////////////////////////////////////////////////////////
// Called when the application is being exited.  Cleans up resources.
// 
int App::OnExit()
{
	m_TrayIcon.Cleanup();
	m_InitializerStack.Cleanup();

	wxImage::CleanUpHandlers();

	int result = wxApp::OnExit();

	// Always clear out memory heaps last.
	ThreadLocalStackAllocator::ReleaseMemoryHeap();

	return result;
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

	Processor processor( TXT( "Helium-Tools-Editor" ), TXT( "[COMMAND <ARGS>]" ), TXT( "Helium Pipeline" ) );

	Helium::CommandLine::HelpCommand helpCommand;
	helpCommand.SetOwner( &processor );
	success &= helpCommand.Initialize( error );
	success &= processor.RegisterCommand( &helpCommand, error );

	success &= processor.AddOption( new FlagOption( &g_HelpFlag, TXT( "h|help" ), TXT( "print program usage" ) ), error );
	success &= processor.AddOption( new FlagOption( &g_DisableTracker, TXT( "disable_tracker" ), TXT( "disable Asset Tracker" ) ), error );
	success &= processor.ParseOptions( argsBegin, argsEnd, error );

	if ( success )
	{
		if ( g_HelpFlag )
		{
			// TODO: This needs to be a message box, it will never be seen in release builds
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
		Log::Error( TXT( "%s\n" ), error.c_str() );
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
