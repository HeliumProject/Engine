#include "Precompile.h"

#include "BuildCommand.h"

#include <strstream>
#include <iostream>
#include <fstream>
#include <string>

#include "Platform/Debug.h"
#include "Platform/Process.h"
#include "Platform/Windows/Console.h"
#include "Platform/Exception.h"

#include "Foundation/Log.h"
#include "Foundation/Profile.h"
#include "Foundation/Exception.h"
#include "Foundation/Startup.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/CommandLine/Option.h"
#include "Foundation/CommandLine/Command.h"
#include "Foundation/CommandLine/Utilities.h"
#include "Foundation/File/Path.h"
#include "Foundation/IPC/Connection.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/Worker/Client.h"
#include "Foundation/Worker/Process.h"

#include "Pipeline/Asset/AssetInit.h"
#include "Pipeline/Asset/AssetClass.h"

using namespace Helium::Asset;
using namespace Helium::CommandLine;

#define MAX_MATCHES 20

/*

// globals
bool g_ImmortalWorker = false;



BuildCommand::BuildCommand()
: Command( "build", "<ASSET> [<ASSET> ...]", "Build assets" )
{
}

BuildCommand::~BuildCommand()
{
    m_InitializerStack.Cleanup();
}

///////////////////////////////////////////////////////////////////////////////
bool BuildCommand::Initialize( tstring& error )
{
    bool result = true;
    
    result &= AddOption( new FlagOption( &m_AllFlag, "all", "build all assets matching the input spec" ), error );
    result &= AddOption( new FlagOption( &m_ForceFlag, "f|force", "force a build even if it's up to date" ), error );
    result &= AddOption( new FlagOption( &m_NoMultipleFlag, "nm|nomultiple", "asset must be unique, selection not necessary" ), error );
    result &= AddOption( new FlagOption( &m_GenerateReportFlag, "report", "report users suspected of causing problems" ), error );
    result &= AddOption( new FlagOption( &m_HaltOnErrorFlag, "halt_on_error", "errors should immediately halt the build" ), error );
    result &= AddOption( new FlagOption( &m_DisableCacheFilesFlag, "disable_cache_files", "disable upload/download via cache file storage" ), error );
    //DISABLED: result &= AddOption( new FlagOption( &m_SingleThreadFlag, "single_thread", "disable processing using multiple threads" ), error );
    result &= AddOption( new FlagOption( &m_WorkerFlag, "worker", "disable processing using multiple threads" ), error );
    result &= AddOption( new SimpleOption<tstring>( &m_HackFileSpecOption, "hack_filespec", "<SPEC>", "invalidate the format version of a FileSpec" ), error );
    result &= AddOption( new SimpleOption<std::vector<tstring>>( &m_RegionOption, "region", "<REGION> [<REGION> ...]", "only build the listed regions" ), error );
    result &= AddOption( new FlagOption( &m_HelpFlag, "h|help", "print command usage" ), error );

    return result;
}

void BuildCommand::Cleanup()
{
    m_InitializerStack.Cleanup();
}

///////////////////////////////////////////////////////////////////////////////
bool BuildCommand::Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error )
{
    if ( !ParseOptions( argsBegin, argsEnd, error ) )
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
        const tstring& arg = (*argsBegin);
        ++argsBegin;

        if ( arg.length() )
        {
            m_SearchQuery = arg;
        }
    }

    if ( m_SearchQuery.empty() )
    {
        error = tstring( "Please pass an asset to build." );
        return false;
    }

    if ( !QueryAssetPaths( m_SearchQuery, m_NoMultipleFlag, m_AllFlag, m_AssetPaths ) )
    {
        return true;
    }


    bool success = true;

	m_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
	m_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );
	m_InitializerStack.Push( AssetBuilder::Initialize, AssetBuilder::Cleanup );

	AssetBuilder::AddAssetBuiltListener( AssetBuilder::AssetBuiltSignature::Delegate ( &AssetBuilt ) );


    if ( Helium::IsDebuggerPresent() )
    {
        if ( m_WorkerFlag )
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
            if ( m_WorkerFlag )
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
        catch( const Helium::Exception& ex )
        {
            if ( m_HaltOnErrorFlag )
            {
                throw;
            }
            success = false;
            Except( ex );
        }
    }

    return success;
}


///////////////////////////////////////////////////////////////////////////////
void BuildCommand::AssetBuilt( const AssetBuilder::AssetBuiltArgsPtr& args )
{
#pragma TODO ("Re-enable worker stuff")
    //if ( m_WorkerFlag )
    //{
    //    tstringstream stream;

    //    try
    //    {
    //        Reflect::Archive::ToStream( args, stream, Reflect::ArchiveTypes::Binary );
    //    }
    //    catch ( Helium::Exception& ex )
    //    {
    //        Log::Error( "%s\n", ex.What() );
    //        return;
    //    }

    //    Worker::Client::Send(0x1, (uint32_t)stream.str().length(), (const uint8_t*)stream.str().c_str());
    //}
}

///////////////////////////////////////////////////////////////////////////
bool BuildCommand::QueryAssetPaths( const tstring& searchQuery, bool noMultiple, bool all, std::set< Helium::Path >& assetPaths )
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
        HELIUM_BREAK();
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
void BuildCommand::Except( const Helium::Exception& ex, const Asset::AssetClassPtr& assetClass )
{
    // Log all exceptions (they will be non-fatal) for debugging purposes.  If 
    // you want to disable this functionality, look for Debug::ProcessException 
    // in all of the builder code and comment it out.  We could also add a flag
    // for this option if it becomes useful.
    Debug::ProcessException( ex );

    tostringstream message;
    if (assetClass.ReferencesObject())
    {
        message << "Exception while building '" << assetClass->GetFullName() << "': " << ex.What() << std::endl;
    }
    else
    {
        message << "Exception: " << ex.What() << std::endl;
    }

    Log::Error( "%s", message.str().c_str() );

    if ( m_GenerateReportFlag )
    {
        tostringstream subject;
        subject << "Error Report: " << Helium::GetCmdLine();

        //    Windows::SendMail( subject.str(), message.str() );
    }
}


///////////////////////////////////////////////////////////////////////////////
void BuildCommand::Report(Asset::AssetClass* assetClass)
{
    if (m_GenerateReportFlag && (Log::GetErrorCount() || Log::GetWarningCount()))
    {
        tstring line;
        std::fstream file ( tstring( assetClass->GetBuiltDirectory() + "error.txt" ).c_str() );
        if ( !file.fail() )
        {
            Helium::Print( Helium::ConsoleColors::White, stderr, "Warnings and Errors:\n" );
            while ( !file.eof() )
            {
                std::getline( file, line );

                Helium::ConsoleColor color = Helium::ConsoleColors::None;
                if ( strncmp( "Error", line.c_str(), 5 ) == 0 )
                {
                    color = Helium::ConsoleColors::Red;
                }
                else if ( strncmp( "Warning", line.c_str(), 7 ) == 0 )
                {
                    color = Helium::ConsoleColors::Yellow;
                }

                Helium::Print( color, stderr, "%s\n", line.c_str() );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
bool BuildCommand::Build( Dependencies::DependencyGraph& depGraph, std::set< Helium::Path >& assets, const std::vector< tstring >& options )
{
    bool success = true;

    for ( std::set< Helium::Path >::const_iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
    {
        const Helium::Path& path = (*itr);

        AssetClassPtr assetClass;

        if (Helium::IsDebuggerPresent() && !m_AllFlag )
        {
            assetClass = AssetClass::LoadAssetClass( path );

            if (assetClass.ReferencesObject())
            {
                AssetBuilder::Build( depGraph, assetClass, options );
                Report( assetClass );
            }
            else
            {
                throw Helium::Exception( "Unable to load asset '%s'", path.c_str() );
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
                    throw Helium::Exception( "Unable to load asset '%s'", path.c_str() );
                }
            }
            catch( const Helium::Exception& ex )
            {
                if ( m_HaltOnErrorFlag )
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
bool BuildCommand::Build( Dependencies::DependencyGraph& depGraph, std::set< Helium::Path >& assets, const AssetBuilder::BuilderOptionsPtr& options )
{
    bool success = true;

    AssetBuilder::V_BuildJob jobs;
    V_AssetClass buildingAssets;

    for ( std::set< Helium::Path >::const_iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
    {
        const Helium::Path& filePath = (*itr);

        AssetClassPtr assetClass;

        if (Helium::IsDebuggerPresent() && !m_AllFlag)
        {
            assetClass = AssetClass::LoadAssetClass( filePath );

            if (assetClass.ReferencesObject())
            {
                jobs.push_back( new AssetBuilder::BuildJob( &depGraph, assetClass, options, NULL, true ) );
                buildingAssets.push_back( assetClass );
            }
            else
            {
                throw Helium::Exception( "Unable to load asset '%s'", filePath.c_str() );
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
                    throw Helium::Exception( "Unable to locate asset '%s'", filePath.c_str() );
                }
            }
            catch( const Helium::Exception& ex )
            {
                if ( m_HaltOnErrorFlag )
                {
                    throw;
                }
                success = false;
                Except( ex, assetClass );
            }
        }
    }

    if (Helium::IsDebuggerPresent())
    {
        AssetBuilder::Build( depGraph, jobs );
    }
    else
    {
        try
        {
            AssetBuilder::Build( depGraph, jobs );
        }
        catch( const Helium::Exception& ex )
        {
            if ( m_HaltOnErrorFlag )
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
bool BuildCommand::RunAsBuildWorker( Dependencies::DependencyGraph& depGraph, bool debug, bool wait )
{
    bool success = true;

    if (!Worker::Client::Initialize( debug, wait ))
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

            AssetBuilder::BuildRequestPtr job = Reflect::SafeCast<AssetBuilder::BuildRequest> (Reflect::Archive::FromStream(stream, Reflect::ArchiveTypes::Binary, Reflect::GetClass<AssetBuilder::BuildRequest>()));

            if (!job.ReferencesObject())
            {
#ifdef _DEBUG
                MessageBoxA(NULL, "Unable to decode message from foreground!", "Error", MB_OK);
                return false;
#else
                throw Helium::Exception("Unable to decode message from foreground!");
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
            HELIUM_BREAK();
        }
    }
    while (g_ImmortalWorker);

    return success;
}

*/