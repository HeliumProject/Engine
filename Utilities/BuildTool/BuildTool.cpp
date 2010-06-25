#include <wx/wx.h>
#include <strstream>
#include <iostream>
#include <fstream>
#include <string>

#include "Platform/Process.h"
#include "Platform/Windows/Console.h"

#include "Foundation/Log.h"
#include "Foundation/Profile.h"
#include "Foundation/Version.h"
#include "Foundation/Exception.h"
#include "Foundation/CommandLine/Option.h"
#include "Foundation/CommandLine/Utilities.h"
#include "Foundation/CommandLine/Command.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/File/Path.h"
#include "Foundation/IPC/Connection.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/String/Utilities.h"

#include "Application/RCS/RCS.h"
#include "Application/Exception.h"
#include "Application/Worker/Client.h"
#include "Application/Worker/Process.h"
#include "Application/Application.h"

#include "Pipeline/AssetBuilder/AssetBuilder.h"
#include "Pipeline/Asset/AssetInit.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Content/ContentInit.h"

using namespace Asset;
using namespace Nocturnal::CommandLine;

#define MAX_MATCHES 20

// globals
bool g_ImmortalWorker = false;


class BuildCommand : public Command
{
protected:
	std::string m_SearchQuery;
	std::set< Nocturnal::Path > m_AssetPaths;

	bool m_HelpFlag;
	bool m_AllFlag;
	bool m_NoMultipleFlag;
	bool m_GenerateReportFlag;
	bool m_ForceFlag;
	bool m_HaltOnErrorFlag;
	bool m_DisableCacheFilesFlag;
	bool m_SingleThreadFlag;
	bool m_WorkerFlag;
	std::string m_HackFileSpecOption;
	std::vector< std::string> m_RegionOption;

public:
	BuildCommand()
		: Command( "build", "<ASSET> [<ASSET> ...]", "Build assets" )
	{
	}

	virtual ~BuildCommand()
	{
	}

	///////////////////////////////////////////////////////////////////////////
	static bool QueryAssetPaths( const std::string& searchQuery, bool noMultiple, bool all, std::set< Nocturnal::Path >& assetPaths )
	{ 
		// get the asset files they want to build
		int maxMatches = noMultiple ? 1 : ( all ? -1 : MAX_MATCHES );

#pragma TODO( "make this search the tracker" )
		//    File::GlobalResolver().Find( searchQuery, assetPaths );
		if ( assetPaths.empty() )
		{
			return false;
		}

		if ( !all )
		{
#pragma TODO( "implement asset choosing" )
			NOC_BREAK();
			// determine which asset to build from the list
			int assetIndex = -1; //File::DetermineAssetIndex( assetPaths, "build" );

			if ( assetIndex == -1 )
			{
				return false;
			}

			return true;
		}

		return true;
	}

	///////////////////////////////////////////////////////////////////////////////
	void Except( const Nocturnal::Exception& ex, const Asset::AssetClassPtr& assetClass = NULL )
	{
		// Log all exceptions (they will be non-fatal) for debugging purposes.  If 
		// you want to disable this functionality, look for Debug::ProcessException 
		// in all of the builder code and comment it out.  We could also add a flag
		// for this option if it becomes useful.
		Debug::ProcessException( ex );

		std::string type;
		try
		{
			type = typeid(ex).name();

			size_t pos = type.find_first_of(" ");
			if (pos != std::string::npos && pos < type.length()-1)
			{
				type = type.substr( pos+1 );
			}
		}
		catch ( const std::exception& ) // we catch std::exception here to handle call to typeid failing above
		{
			type = "Exception";
		}

		std::ostringstream message;
		if (assetClass.ReferencesObject())
		{
			message << type << " while building '" << assetClass->GetFullName() << "': " << ex.what() << std::endl;
		}
		else
		{
			message << type << ": " << ex.what() << std::endl;
		}

		Log::Error( "%s", message.str().c_str() );

		if ( m_GenerateReportFlag )
		{
			std::ostringstream subject;
			subject << "Error Report: " << Nocturnal::GetCmdLine();

			//    Windows::SendMail( subject.str(), message.str() );
		}
	}


	///////////////////////////////////////////////////////////////////////////////
	void Report(Asset::AssetClass* assetClass)
	{
		if (m_GenerateReportFlag && (Log::GetErrorCount() || Log::GetWarningCount()))
		{
			std::string line;
			std::fstream file ( std::string( assetClass->GetBuiltDirectory() + "error.txt" ).c_str() );
			if ( !file.fail() )
			{
				Platform::Print( Platform::ConsoleColors::White, stderr, "Warnings and Errors:\n" );
				while ( !file.eof() )
				{
					std::getline( file, line );

					Platform::ConsoleColor color = Platform::ConsoleColors::None;
					if ( strncmp( "Error", line.c_str(), 5 ) == 0 )
					{
						color = Platform::ConsoleColors::Red;
					}
					else if ( strncmp( "Warning", line.c_str(), 7 ) == 0 )
					{
						color = Platform::ConsoleColors::Yellow;
					}

					Platform::Print( color, stderr, "%s\n", line.c_str() );
				}
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	bool Build( Dependencies::DependencyGraph& depGraph, std::set< Nocturnal::Path >& assets, const std::vector< std::string >& options )
	{
		bool success = true;

		for ( std::set< Nocturnal::Path >::const_iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
		{
			const Nocturnal::Path& path = (*itr);

			AssetClassPtr assetClass;

			if (Application::IsDebuggerPresent() && !m_AllFlag )
			{
				assetClass = AssetClass::LoadAssetClass( path );

				if (assetClass.ReferencesObject())
				{
					AssetBuilder::Build( depGraph, assetClass, options );
					Report( assetClass );
				}
				else
				{
					throw Nocturnal::Exception( "Unable to load asset '%s'", path.c_str() );
				}
			}
			else
			{
				try
				{
					assetClass = AssetClass::LoadAssetClass( path );

					if (assetClass.ReferencesObject())
					{
						AssetBuilder::Build( depGraph, assetClass, options );
						Report( assetClass );
					}
					else
					{
						throw Nocturnal::Exception( "Unable to load asset '%s'", path.c_str() );
					}
				}
				catch( const Nocturnal::Exception& ex )
				{
					if ( Nocturnal::GetCmdLineFlag( AssetBuilder::CommandArgs::HaltOnError ) )
					{
						throw;
					}
					success = false;
					Except( ex, assetClass );
				}
			}
		}

		return success;
	}

	///////////////////////////////////////////////////////////////////////////////
	bool Build( Dependencies::DependencyGraph& depGraph, std::set< Nocturnal::Path >& assets, const AssetBuilder::BuilderOptionsPtr& options )
	{
		bool success = true;

		AssetBuilder::V_BuildJob jobs;
		V_AssetClass buildingAssets;

		for ( std::set< Nocturnal::Path >::const_iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
		{
			const Nocturnal::Path& filePath = (*itr);

			AssetClassPtr assetClass;

			if (Application::IsDebuggerPresent() && !m_AllFlag)
			{
				assetClass = AssetClass::LoadAssetClass( filePath );

				if (assetClass.ReferencesObject())
				{
					jobs.push_back( new AssetBuilder::BuildJob( &depGraph, assetClass, options, NULL, true ) );
					buildingAssets.push_back( assetClass );
				}
				else
				{
					throw Nocturnal::Exception( "Unable to load asset '%s'", filePath.c_str() );
				}
			}
			else
			{
				try
				{
					assetClass = AssetClass::LoadAssetClass( filePath );

					if (assetClass.ReferencesObject())
					{
						jobs.push_back( new AssetBuilder::BuildJob( &depGraph, assetClass, options, NULL, true ) );
						buildingAssets.push_back( assetClass );
					}
					else
					{
						throw Nocturnal::Exception( "Unable to locate asset '%s'", filePath.c_str() );
					}
				}
				catch( const Nocturnal::Exception& ex )
				{
					if ( Nocturnal::GetCmdLineFlag( AssetBuilder::CommandArgs::HaltOnError ) )
					{
						throw;
					}
					success = false;
					Except( ex, assetClass );
				}
			}
		}

		if (Application::IsDebuggerPresent())
		{
			AssetBuilder::Build( depGraph, jobs );
		}
		else
		{
			try
			{
				AssetBuilder::Build( depGraph, jobs );
			}
			catch( const Nocturnal::Exception& ex )
			{
				if ( Nocturnal::GetCmdLineFlag( AssetBuilder::CommandArgs::HaltOnError ) )
				{
					throw;
				}
				success = false;
				Except( ex );
			}
		}

		for ( V_AssetClass::const_iterator itr = buildingAssets.begin(), end = buildingAssets.end(); itr != end; ++itr )
		{
			Report( *itr );
		}

		return success;
	}

	///////////////////////////////////////////////////////////////////////////////
	bool RunAsBuildWorker( Dependencies::DependencyGraph& depGraph )
	{
		bool success = true;

		if (!Worker::Client::Initialize())
		{
			return false;
		}

		Log::Print("Waiting for build request...\n");

		do
		{
			IPC::Message* msg = Worker::Client::Receive();

			if (msg)
			{
				std::strstream stream ((char*)msg->GetData(), msg->GetSize());

				AssetBuilder::BuildRequestPtr job = Reflect::ObjectCast<AssetBuilder::BuildRequest> (Reflect::ArchiveBinary::FromStream(stream, Reflect::GetType<AssetBuilder::BuildRequest>()));

				if (!job.ReferencesObject())
				{
#ifdef _DEBUG
					MessageBoxA(NULL, "Unable to decode message from foreground!", "Error", MB_OK);
					return false;
#else
					throw Nocturnal::Exception("Unable to decode message from foreground!");
#endif
				}

				Log::Debug( "Building %d requested assets\n", job->m_Assets.size() );

				success &= Build( depGraph, job->m_Assets, job->m_Options );

				delete msg;

				if (g_ImmortalWorker)
				{
					Worker::Client::Send(0);
				}
			}
			else
			{
				NOC_BREAK();
			}
		}
		while (g_ImmortalWorker);

		return success;
	}

	///////////////////////////////////////////////////////////////////////////////
	virtual bool Initialize( std::string& error )
	{
		bool result = true;

		result &= m_OptionsMap.AddOption( new FlagOption( &m_HelpFlag, "h|help", "print command usage" ), error );
		result &= m_OptionsMap.AddOption( new FlagOption( &m_AllFlag, "all", "build all assets matching the input spec" ), error );
		result &= m_OptionsMap.AddOption( new FlagOption( &m_NoMultipleFlag, "nm|nomultiple", "asset must be unique, selection not necessary" ), error );
		result &= m_OptionsMap.AddOption( new FlagOption( &m_GenerateReportFlag, "report", "report users suspected of causing problems" ), error );
		result &= m_OptionsMap.AddOption( new FlagOption( &m_ForceFlag, "f|force", "force a build even if it's up to date" ), error );
		result &= m_OptionsMap.AddOption( new FlagOption( &m_HaltOnErrorFlag, "halt_on_error", "errors should immediately halt the build" ), error );
		result &= m_OptionsMap.AddOption( new FlagOption( &m_DisableCacheFilesFlag, "disable_cache_files", "disable upload/download via cache file storage" ), error );
		result &= m_OptionsMap.AddOption( new FlagOption( &m_SingleThreadFlag, "single_thread", "disable processing using multiple threads" ), error );
		result &= m_OptionsMap.AddOption( new FlagOption( &m_WorkerFlag, "worker", "disable processing using multiple threads" ), error );
		result &= m_OptionsMap.AddOption( new SimpleOption<std::string>( &m_HackFileSpecOption, "hack_filespec", "<SPEC>", "invalidate the format version of a FileSpec" ), error );
		result &= m_OptionsMap.AddOption( new SimpleOption<std::vector<std::string>>( &m_RegionOption, "region", "<REGION> [<REGION> ...]", "only build the listed regions" ), error );

		return result;
	}

	///////////////////////////////////////////////////////////////////////////////
	virtual bool Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) NOC_OVERRIDE
	{
		if ( !m_OptionsMap.ParseOptions( argsBegin, argsEnd, error ) )
		{
			return false;
		}

		if ( m_HelpFlag )
		{
			Log::Print( Help().c_str() );
			return true;
		}

		if ( argsBegin != argsEnd )
		{
			const std::string& arg = (*argsBegin);
			++argsBegin;

			if ( arg.length() )
			{
				m_SearchQuery = arg;
				argsBegin++;

				Nocturnal::Path::Normalize( m_SearchQuery );
			}
		}

		if ( m_SearchQuery.empty() )
		{
			error = std::string( "Please pass an asset to build." );
			return false;
		}

		if ( !QueryAssetPaths( m_SearchQuery, m_NoMultipleFlag, m_AllFlag, m_AssetPaths ) )
		{
			return true;
		}


		bool success = true;

		if ( Application::IsDebuggerPresent() )
		{
			if ( Nocturnal::GetCmdLineFlag( Worker::Args::Worker ) )
			{
#pragma TODO( "Figure out how to handle dependency graphs with workers" )
				//success = RunAsBuildWorker();
			}
			else
			{
#pragma TODO( "instantiate the proper dependency graph and pass it in here" )
				//success = Build( m_AssetPaths, options );
			}
		}
		else
		{
			try
			{
				if ( Nocturnal::GetCmdLineFlag( Worker::Args::Worker ) )
				{
#pragma TODO( "Figure out how to handle dependency graphs with workers" )
					//success = RunAsBuildWorker();        
				}
				else
				{
#pragma TODO( "instantiate the proper dependency graph and pass it in here" )
					//success = Build( m_AssetPaths, options );
				}
			}
			catch( const Nocturnal::Exception& ex )
			{
				if ( Nocturnal::GetCmdLineFlag( AssetBuilder::CommandArgs::HaltOnError ) )
				{
					throw;
				}
				success = false;
				Except( ex );
			}
		}

		return success;
	}
};



///////////////////////////////////////////////////////////////////////////////
void AssetBuilt( const AssetBuilder::AssetBuiltArgsPtr& args )
{
	if ( Nocturnal::GetCmdLineFlag( Worker::Args::Worker ) )
	{
		std::stringstream stream;

		try
		{
			Reflect::ArchiveBinary::ToStream( args, stream );
		}
		catch ( Nocturnal::Exception& ex )
		{
			Log::Error( "%s\n", ex.what() );
			return;
		}

		Worker::Client::Send(0x1, (u32)stream.str().length(), (const u8*)stream.str().c_str());
	}
}

///////////////////////////////////////////////////////////////////////////////
int Main (int argc, const char** argv)
{
	// print physical memory
	MEMORYSTATUSEX status;
	memset(&status, 0, sizeof(status));
	status.dwLength = sizeof(status);
	::GlobalMemoryStatusEx(&status);
	Log::Print("Physical Memory: %I64u M bytes total, %I64u M bytes available\n", status.ullTotalPhys >> 20, status.ullAvailPhys >> 20);

	Nocturnal::InitializerStack initializerStack( true );
	initializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
	initializerStack.Push( Content::Initialize, Content::Cleanup );
	initializerStack.Push( Asset::Initialize, Asset::Cleanup );
	initializerStack.Push( AssetBuilder::Initialize, AssetBuilder::Cleanup );

	AssetBuilder::AddAssetBuiltListener( AssetBuilder::AssetBuiltSignature::Delegate ( &AssetBuilt ) );

	// fill out the options vector
	std::vector< std::string > options;
	for ( int i = 1; i < argc; ++i )
	{
		options.push_back( argv[ i ] );
	}

	std::string error; 

	BuildCommand buildCommand;
	if ( !buildCommand.Initialize( error ) )
	{
		Log::Print( error.c_str() );
		return 1;
	}

	if ( !buildCommand.Process( options.begin(), options.end(), error ) )
	{
		if ( !error.empty() )
		{
			Log::Print( "Error: \n" );
			Log::Print( error.c_str() );
			Log::Print( "\n" );
		}
		Log::Print( buildCommand.Help().c_str() );

		return 1;
	}

	return 0;
}

class BuildToolApp : public wxApp
{
public:
	virtual bool OnInit() NOC_OVERRIDE
	{
		return true; // skip wxWidgets cmd line processing
	}

	virtual int OnRun() NOC_OVERRIDE
	{
		return Main( argc, (const char**)argv );
	}
};

DECLARE_APP( BuildToolApp );

IMPLEMENT_APP( BuildToolApp );

int main()
{
	return Application::StandardWinMain( &wxEntry );
}
