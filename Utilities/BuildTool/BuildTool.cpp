#include <wx/wx.h>
#include <strstream>
#include <iostream>
#include <fstream>
#include <string>

#include "Common/Config.h"
#include "Common/Version.h"
#include "Common/Exception.h"
#include "Common/CommandLine.h"
#include "Common/String/Utilities.h"

#include "rcs/rcs.h"
#include "Debug/Exception.h"
#include "DebugUI/DebugUI.h"
#include "IPC/Connection.h"
#include "Profile/Profile.h"
#include "Console/Console.h"
#include "Windows/Console.h"
#include "Windows/Process.h"

#include "File/File.h"
#include "Finder/Finder.h"
#include "Finder/DebugSpecs.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "FileSystem/FileSystem.h"

#include "AssetBuilder/AssetBuilder.h"
#include "Asset/AssetInit.h"
#include "Asset/AssetClass.h"

#include "Worker/Client.h"
#include "Worker/Process.h"
#include "AppUtils/AppUtils.h"
#include "Common/InitializerStack.h"
#include "Content/ContentInit.h"

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
    Console::Print( "Usage: buildtool asset [options]\n" );
    Console::Print( "\n" );
    Console::Print( "Basic Options:\n" );
    Console::Print( "    -h[elp]              -- this help\n" );
    Console::Print( "    -view                -- view the asset after building it\n" );
    Console::Print( "\n" );
    Console::Print( "Assets to Build:\n" );
    Console::Print( " -region[s] <region list>     -- only build the listed regions\n" );
    Console::Print( "    -defaults            -- build all the default assets\n" );
    Console::Print( " -nm|nomultiple               -- asset must be unique, selection not necessary\n" );
    Console::Print( " -all                         -- build all assets matching the input spec\n" );
    Console::Print( "\n" );
    Console::Print( "Build System:\n" );
    Console::Print( " -f[orce]|hack_all_filespecs  -- force a build even if it's up to date\n" );
    Console::Print( " -hack_filespec <specname>    -- invalidate the format version of a FileSpec\n" );
    Console::Print( " -disable_cache_files         -- disable upload/download via cache file storage\n" );
    Console::Print( " -single_thread               -- disable processing using multiple threads\n" );
    Console::Print( "\n" );
    Console::Print( "Error Handling:\n" );
    Console::Print( " -halt_on_error               -- errors should immediately halt the build\n" );
    Console::Print( " -report                      -- report users suspected of causing problems\n" );
    Console::Print( "\n" );
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
    FileSystem::CleanName( g_SearchQuery );

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
                    Console::Error( "Invalid filespec: %s\n", specName.c_str() );
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

    Console::Error( "%s", message.str().c_str() );

    if (g_GenerateReport)
    {
        std::string project (NOCTURNAL_PROJECT_NAME);
        toUpper( project );

        std::ostringstream subject;
        subject << project << " Error Report: " << Nocturnal::GetCmdLine();

        //    Windows::SendMail( subject.str(), message.str() );
    }
}

///////////////////////////////////////////////////////////////////////////////
void Report(Asset::AssetClass* assetClass)
{
    if (g_GenerateReport && (Console::GetErrorCount() || Console::GetWarningCount()))
    {
        std::string line;
        std::fstream file ( FinderSpecs::Debug::ERROR_FILE.GetFile( assetClass->GetBuiltDirectory() ).c_str() );
        if ( !file.fail() )
        {
            Windows::Print( Windows::ConsoleColors::White, stderr, "Warnings and Errors:\n" );
            while ( !file.eof() )
            {
                std::getline( file, line );

                Windows::ConsoleColor color = Windows::ConsoleColors::None;
                if ( strncmp( "Error", line.c_str(), 5 ) == 0 )
                {
                    color = Windows::ConsoleColors::Red;
                }
                else if ( strncmp( "Warning", line.c_str(), 7 ) == 0 )
                {
                    color = Windows::ConsoleColors::Yellow;
                }

                Windows::Print( color, stderr, "%s\n", line.c_str() );
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
            Console::Error( "%s\n", ex.what() );
            return;
        }

        Worker::Client::Send(0x1, (u32)stream.str().length(), (const u8*)stream.str().c_str());
    }
}

///////////////////////////////////////////////////////////////////////////////
bool Build( File::S_Reference& assets, const V_string& options )
{
    bool success = true;

    for ( File::S_Reference::iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
    {
        File::ReferencePtr& fileRef = (*itr);
        fileRef->Resolve();

        AssetClassPtr assetClass;

        if (AppUtils::IsDebuggerPresent() && !g_All)
        {
            assetClass = AssetClass::LoadAssetClass( *fileRef );

            if (assetClass.ReferencesObject())
            {
                AssetBuilder::Build( assetClass, options );
                Report( assetClass );
            }
            else
            {
                throw Nocturnal::Exception( "Unable to load asset '%s'", fileRef->GetFile().GetPath().c_str() );
            }
        }
        else
        {
            try
            {
                assetClass = AssetClass::LoadAssetClass( *fileRef );

                if (assetClass.ReferencesObject())
                {
                    AssetBuilder::Build( assetClass, options );
                    Report( assetClass );
                }
                else
                {
                    throw Nocturnal::Exception( "Unable to load asset '%s'", fileRef->GetFile().GetPath().c_str() );
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
bool Build( File::S_Reference& assets, const AssetBuilder::BuilderOptionsPtr& options )
{
    bool success = true;

    AssetBuilder::V_BuildJob jobs;
    V_AssetClass buildingAssets;

    for ( File::S_Reference::iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
    {
        File::ReferencePtr& fileRef = (*itr);

        AssetClassPtr assetClass;

        if (AppUtils::IsDebuggerPresent() && !g_All)
        {
            assetClass = AssetClass::LoadAssetClass( *fileRef );

            if (assetClass.ReferencesObject())
            {
                jobs.push_back( new AssetBuilder::BuildJob( assetClass, options, NULL, true ) );
                buildingAssets.push_back( assetClass );
            }
            else
            {
                throw Nocturnal::Exception( "Unable to load asset '%s'", fileRef->GetFile().GetPath().c_str() );
            }
        }
        else
        {
            try
            {
                assetClass = AssetClass::LoadAssetClass( *fileRef );

                if (assetClass.ReferencesObject())
                {
                    jobs.push_back( new AssetBuilder::BuildJob( assetClass, options, NULL, true ) );
                    buildingAssets.push_back( assetClass );
                }
                else
                {
                    throw Nocturnal::Exception( "Unable to locate asset '%s'", fileRef->GetFile().GetPath().c_str() );
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

    if (AppUtils::IsDebuggerPresent())
    {
        AssetBuilder::Build( jobs );
    }
    else
    {
        try
        {
            AssetBuilder::Build( jobs );
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
    File::S_Reference possibleMatches;
    File::GlobalResolver().Find( g_SearchQuery, possibleMatches );

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

    return Build( possibleMatches, options );
}

///////////////////////////////////////////////////////////////////////////////
bool RunAsBuildWorker()
{
    bool success = true;

    if (!Worker::Client::Initialize())
    {
        return false;
    }

    Console::Print("Waiting for build request...\n");

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

            Console::Debug( "Building %d requested assets\n", job->m_Assets.size() );

            success &= Build( job->m_Assets, job->m_Options );

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
    Console::Print("Physical Memory: %I64u M bytes total, %I64u M bytes available\n", status.ullTotalPhys >> 20, status.ullAvailPhys >> 20);

    // enable heap defragmenting
    bool lowFragHeap = Windows::EnableLowFragmentationHeap();
    Console::Debug("Low Fragmentation Heap is %s\n", lowFragHeap ? "enabled" : "not enabled");

    Nocturnal::InitializerStack initializerStack( true );
    initializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
    initializerStack.Push( Content::Initialize, Content::Cleanup );
    initializerStack.Push( Asset::Initialize, Asset::Cleanup );
    initializerStack.Push( Finder::Initialize, Finder::Cleanup );
    initializerStack.Push( File::Initialize, File::Cleanup );
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
        Console::Bullet bullet ("Building default assets...\n");
        //    success = BuildDefaultAssets(options);
        return success ? 0 : 1;
    }

    if (AppUtils::IsDebuggerPresent())
    {
        if (Nocturnal::GetCmdLineFlag( Worker::Args::Worker ))
        {
            success = RunAsBuildWorker();
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
                success = RunAsBuildWorker();        
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

class Application : public wxApp
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

DECLARE_APP( Application );

IMPLEMENT_APP( Application );

int main()
{
    Nocturnal::InitializerStack initializerStack( true );

    initializerStack.Push( &DebugUI::Initialize, &DebugUI::Cleanup );

    return AppUtils::StandardWinMain( &wxEntry );
}
