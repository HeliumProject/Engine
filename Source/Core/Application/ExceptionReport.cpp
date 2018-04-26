#include "Precompile.h"
#include "ExceptionReport.h"

#include "Platform/Console.h"
#include "Platform/Encoding.h"
#include "Platform/Exception.h"
#include "Platform/Process.h"

#include "Foundation/Log.h"
#include "Foundation/Profile.h"
#include "Foundation/FilePath.h"

#include "Application/CmdLine.h"

#include <sstream>
#include <time.h>
#include <sstream>
#include <iomanip>

using namespace Helium;

#if HELIUM_OS_WIN

ExceptionReport::ExceptionReport( const ExceptionArgs& args )
	: m_Args ( args )
{
	m_UserName = Helium::GetUserName();
	m_Computer = Helium::GetMachineName();
	m_ApplicationPath = Helium::GetProcessPath();
	m_ApplicationName = Helium::GetProcessName();

	size_t firstDot = m_ApplicationName.find_last_of( '.' );
	m_ApplicationName = m_ApplicationName.substr( 0, firstDot );

	m_CmdLineArgs.clear();
	{
		int cmdArgc;
		const char** cmdArgv = Helium::GetCmdLine( cmdArgc );
		if ( cmdArgc > 1 )
		{
			for ( int i = 1; i < cmdArgc; ++i )
			{
				if ( !m_CmdLineArgs.empty() )
				{
					m_CmdLineArgs += " ";
				}
				m_CmdLineArgs += cmdArgv[i];
			}
		}
	}

#ifdef DEBUG
	m_BuildConfig = "Debug";
#else
	m_BuildConfig = "Release";
#endif

	m_Environment.clear();
	// Get a pointer to the environment block. 
	const wchar_t* env = (const wchar_t*)::GetEnvironmentStringsW();
	if ( env )
	{
		// Variable strings are separated by NULL byte, and the block is terminated by a NULL byte. 
		for (const wchar_t* var = (const wchar_t*)env; *var; var++) 
		{
			// Variable strings are separated by NULL byte, and the block is terminated by a NULL byte. 
			for (const wchar_t* var = (const wchar_t*)env; *var; var++) 
			{
				if (*var != '=')
				{
					HELIUM_WIDE_TO_TCHAR( var, convertedVar );
					m_Environment += convertedVar;
					m_Environment += "\n";
				}

				while (*var)
				{
					var++;
				}
			}

			::FreeEnvironmentStringsW((wchar_t*)env);
		}
	}
}

static void HandleException( const Helium::ExceptionArgs& args )
{
	ExceptionReport report ( args );

	std::string subject;
	std::stringstream body;

	try
	{
		if ( report.m_Args.m_Fatal )
		{
			subject += "Fatal ";
		}
		subject += Helium::ExceptionTypes::Strings[ report.m_Args.m_Type ];
		subject += " Exception: " + report.m_ApplicationName + " " + report.m_UserName + "@" + report.m_Computer;

		body << "Username: " << report.m_UserName << std::endl;
		body << "Computer: " << report.m_Computer << std::endl;
		body << "Build Config: " << report.m_BuildConfig << std::endl;
		body << "Command Line: " << Helium::GetCmdLine() << std::endl;

		if ( !report.m_Args.m_State.empty() )
		{
			body << std::endl;
			body << "Outline State:   " << std::endl;
			body << report.m_Args.m_State << std::endl;
		}

		body << std::endl;
		body << "Type: " << Helium::ExceptionTypes::Strings[ report.m_Args.m_Type ] << std::endl;
		switch ( report.m_Args.m_Type )
		{
		case Helium::ExceptionTypes::Structured:
			{
				body << "Code: 0x" << std::hex << std::setfill( '0' ) << std::setw(8) << report.m_Args.m_SEHCode << std::endl;
				body << "MetaClass: " << report.m_Args.m_SEHClass << std::endl;

				if ( !report.m_Args.m_Message.empty() )
				{
					body << std::endl;
					body << "Message: " << std::endl;
					body << report.m_Args.m_Message << std::endl;
				}

				body << std::endl << report.m_Args.m_SEHControlRegisters;
				body << std::endl << report.m_Args.m_SEHIntegerRegisters;

				break;
			}

		case Helium::ExceptionTypes::CPP:
			{
				body << "MetaClass: " << report.m_Args.m_CPPClass << std::endl;

				if ( !report.m_Args.m_Message.empty() )
				{
					body << std::endl;
					body << "Message: " << std::endl;
					body << report.m_Args.m_Message << std::endl;
				}

				break;
			}
		}

		if ( !report.m_Args.m_Callstack.empty() )
		{
			body << std::endl;
			body << "Callstack:" << std::endl;
			body << report.m_Args.m_Callstack << std::endl;
		}

		std::vector< std::string >::const_iterator itr = report.m_Args.m_Threads.begin();
		std::vector< std::string >::const_iterator end = report.m_Args.m_Threads.end();
		for ( ; itr != end; ++itr )
		{
			body << std::endl << *itr << std::endl;
		}

		if ( !report.m_Environment.empty() )
		{
			body << std::endl;
			body << "Environment:" << std::endl;
			body << report.m_Environment << std::endl;
		}
	}
	catch ( Helium::Exception& ex )
	{
		Helium::Print(Helium::ConsoleColors::Red, stderr, "%s\n", ex.What() );
	}

	Helium::Print(Helium::ConsoleColors::Red, stderr, "%s\n%s\n", subject.c_str(), body.str().c_str() );
}

#endif

static int32_t g_InitCount = 0;

void Helium::InitializeExceptionListener()
{
	// init counting this API seems kind of silly, but we can actually get initialized from several places
	if ( ++g_InitCount == 1 )
	{
#if HELIUM_OS_WIN
		FilePath process ( GetProcessPath() );

		// Symbol path always starts with module directory
		FilePath symbolPath( process.Directory() );

		// initialize debug symbols
		Helium::InitializeSymbols( symbolPath.Get() );

		// from here on out, submit crash reports
		Helium::EnableExceptionFilter(true);

		// wait for an exception
		Helium::g_ExceptionOccurred.Set( &HandleException );
#endif
	}
}

void Helium::CleanupExceptionListener()
{
	if ( --g_InitCount == 0 )
	{
#if HELIUM_OS_WIN
		// stop waiting for exception
		Helium::g_ExceptionOccurred.Clear();

		// uninstall the exception filter function
		Helium::EnableExceptionFilter(false);
#endif
	}
}
