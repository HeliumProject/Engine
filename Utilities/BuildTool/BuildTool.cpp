#include <wx/wx.h>
#include <strstream>
#include <iostream>
#include <fstream>
#include <string>

#include "Foundation/Version.h"
#include "Foundation/Exception.h"
#include "Foundation/CommandLine.h"
#include "Foundation/File/Path.h"
#include "Foundation/String/Utilities.h"

#include "Application/RCS/RCS.h"
#include "Debug/Exception.h"
#include "Foundation/Log.h"
#include "Foundation/Profile.h"
#include "Foundation/IPC/Connection.h"
#include "Platform/Windows/Console.h"
#include "Platform/Process.h"

#include "Finder/DebugSpecs.h"

#include "AssetBuilder/AssetBuilder.h"
#include "Asset/AssetInit.h"
#include "Asset/AssetClass.h"

#include "Application/Worker/Client.h"
#include "Application/Worker/Process.h"
#include "Application/Application.h"
#include "Foundation/InitializerStack.h"
#include "Pipeline/Content/ContentInit.h"

using namespace Asset;

#define MAX_MATCHES 20

// globals
std::string       g_SearchQuery;
bool              g_All = false;
bool              g_NoMultiple = false;
bool              g_GenerateReport = false;
bool              g_ImmortalWorker = false;
bool              g_Defaults = false;
V_string          g_Regions;     // only for levels, which need to view a single region

///////////////////////////////////////////////////////////////////////////////
void PrintUsage()
{
    Log::Print( "Usage: buildtool asset [options]\n" );
    Log::Print( "\n" );
    Log::Print( "Basic Options:\n" );
    Log::Print( "    -h[elp]              -- this help\n" );
    Log::Print( "    -view                -- view the asset after building it\n" );
    Log::Print( "\n" );
    Log::Print( "Assets to Build:\n" );
    Log::Print( " -region[s] <region list>     -- only build the listed regions\n" );
    Log::Print( "    -defaults            -- build all the default assets\n" );
    Log::Print( " -nm|nomultiple               -- asset must be unique, selection not necessary\n" );
    Log::Print( " -all                         -- build all assets matching the input spec\n" );
    Log::Print( "\n" );
    Log::Print( "Build System:\n" );
    Log::Print( " -f[orce]|hack_all_filespecs  -- force a build even if it's up to date\n" );
    Log::Print( " -hack_filespec <specname>    -- invalidate the format version of a FileSpec\n" );
    Log::Print( " -disable_cache_files         -- disable upload/download via cache file storage\n" );
    Log::Print( " -single_thread               -- disable processing using multiple threads\n" );
    Log::Print( "\n" );
    Log::Print( "Error Handling:\n" );
    Log::Print( " -halt_on_error               -- errors should immediately halt the build\n" );
    Log::Print( " -report                      -- report users suspected of causing problems\n" );
    Log::Print( "\n" );
}

///////////////////////////////////////////////////////////////////////////////
bool ParseProgramOptions( int argc, const char** argv )
{
    if ( argc < 2 )
    {
        PrintUsage();

        return false;
    }

    if ( !stricmp( argv[1], "-h" ) || !stricmp( argv[1], "-help" ) )
    {
        PrintUsage();

        return false;
    }

    // search query (asset to build) must be the first command line argument
    // it might be cooler if it was the last argument but that fucks up the buildserver and stuff
    // not changing it for this tools release. (10/29/2008)
    //
    g_SearchQuery = argv[ 1 ];
    Nocturnal::Path::Normalize( g_SearchQuery );

    if ( !stricmp( g_SearchQuery.c_str(), "-defaults" ) )
    {
        g_Defaults = true;
    }

    for ( int arg = 2; arg < argc; ++arg )
    {
        if ( !stricmp( argv[ arg ], "-all" ) )
        {
            g_All = true;
        }
        else if ( !stricmp( argv[ arg ], "-nm" ) || !stricmp( argv[ arg ], "-nomultiple" ) )
        {
            g_NoMultiple = true;
        }
        else if ( !stricmp( argv[ arg ], "-defaults" ) )
        {
            g_Defaults = true;
        }
        else if ( !stricmp( argv[ arg ], "-regions" ) || !stricmp( argv[ arg ], "-region" ) )
        {
            // get list of regions
            while ( arg + 1 < argc )
            {
                // stop looking once we get to the optional params
                char c = argv[arg + 1][0];
                if (c == '-')
                    break;

                g_Regions.push_back( argv[ arg + 1 ] );
                arg++;
            }
        }
        else if ( !stricmp( argv[ arg ], "-hack_filespec" ) )
        {
            if ( arg + 1 < argc )
            {
                std::string specName = argv[ ++arg ];

                if ( !Finder::HackSpec( specName ) )
                {
                    Log::Error( "Invalid filespec: %s\n", specName.c_str() );
                }
            }
        }
        else if ( !stricmp( argv[ arg ], "-hack_all_filespecs" ) || !stricmp( argv[ arg ], "-f" ) || !stricmp( argv[ arg ], "-force" ) )
        {
            Finder::HackAllSpecs();
        }
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

    if (g_GenerateReport)
    {
        std::ostringstream subject;
        subject << "Error Report: " << Nocturnal::GetCmdLine();

        //    Windows::SendMail( subject.str(), message.str() );
    }
}

///////////////////////////////////////////////////////////////////////////////
void Report(Asset::AssetClass* assetClass)
{
    if (g_GenerateReport && (Log::GetErrorCount() || Log::GetWarningCount()))
    {
        std::string line;
        std::fstream file ( FinderSpecs::Debug::ERROR_FILE.GetFile( assetClass->GetBuiltDirectory() ).c_str() );
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
void AssetBuilt( const AssetBuilder::AssetBuiltArgsPtr& args )
{
    if ( Nocturnal::GetCmdLineFlag( Worker::Args::Worker ) )
    {
        std::stringstream stream;

        try
        {
            Reflect::Archive::ToStream( args, stream, Reflect::ArchiveTypes::Binary );
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
bool Build( Dependencies::DependencyGraph& depGraph, Nocturnal::S_Path& assets, const V_string& options )
{
    bool success = true;

    for ( Nocturnal::S_Path::const_iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
    {
        const Nocturnal::Path& path = (*itr);

        AssetClassPtr assetClass;

        if (Application::IsDebuggerPresent() && !g_All)
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
bool Build( Dependencies::DependencyGraph& depGraph, Nocturnal::S_Path& assets, const AssetBuilder::BuilderOptionsPtr& options )
{
    bool success = true;

    AssetBuilder::V_BuildJob jobs;
    V_AssetClass buildingAssets;

    for ( Nocturnal::S_Path::const_iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
    {
        const Nocturnal::Path& filePath = (*itr);

        AssetClassPtr assetClass;

        if (Application::IsDebuggerPresent() && !g_All)
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
bool QueryAndBuildAssets(const V_string& options)
{ 
    // get the asset files they want to build
    int maxMatches = g_NoMultiple ? 1 : (g_All ? -1 : MAX_MATCHES);
    Nocturnal::S_Path possibleMatches;
#pragma TODO( "make this search the tracker" )
    //    File::GlobalResolver().Find( g_SearchQuery, possibleMatches );

    if ( possibleMatches.empty() )
    {
        return false;
    }

    if ( !g_All )
    {
#pragma TODO( "implement asset choosing" )
        NOC_BREAK();
        //// determine which asset to build from the list
        //int assetIndex = File::DetermineAssetIndex( possibleMatches, "build" );

        //if ( assetIndex == -1 )
        //{
        //    return false;
        //}

        //return true;
        return false;
    }

#pragma TODO( "instantiate the proper dependency graph and pass it in here" )
//    return Build( possibleMatches, options );
    return false;
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

            AssetBuilder::BuildRequestPtr job = Reflect::ObjectCast<AssetBuilder::BuildRequest> (Reflect::Archive::FromStream(stream, Reflect::ArchiveTypes::Binary, Reflect::GetType<AssetBuilder::BuildRequest>()));

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
            _asm nop;
        }
    }
    while (g_ImmortalWorker);

    return success;
}

///////////////////////////////////////////////////////////////////////////////
int Main (int argc, const char** argv)
{
    bool success = true;
    g_GenerateReport = Nocturnal::GetCmdLineFlag("report");

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
    initializerStack.Push( Finder::Initialize, Finder::Cleanup );
    initializerStack.Push( AssetBuilder::Initialize, AssetBuilder::Cleanup );

    AssetBuilder::AddAssetBuiltListener( AssetBuilder::AssetBuiltSignature::Delegate ( &AssetBuilt ) );

    if ( !ParseProgramOptions( argc, argv ) )
    {
        return 1;
    }

    // fill out the options vector
    V_string options;
    for ( int i = 2; i < argc; ++i )
    {
        options.push_back( argv[ i ] );
    }

    if (g_Defaults)
    {
        Log::Bullet bullet ("Building default assets...\n");
        //    success = BuildDefaultAssets(options);
        return success ? 0 : 1;
    }

    if (Application::IsDebuggerPresent())
    {
        if (Nocturnal::GetCmdLineFlag( Worker::Args::Worker ))
        {
#pragma TODO( "Figure out how to handle dependency graphs with workers" )
            //            success = RunAsBuildWorker();
        }
        else
        {
            success = QueryAndBuildAssets( options );
        }
    }
    else
    {
        try
        {
            if (Nocturnal::GetCmdLineFlag( Worker::Args::Worker ))
            {
#pragma TODO( "Figure out how to handle dependency graphs with workers" )
//                success = RunAsBuildWorker();        
            }
            else
            {
                success = QueryAndBuildAssets( options );
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

    return success ? 0 : 1;
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
