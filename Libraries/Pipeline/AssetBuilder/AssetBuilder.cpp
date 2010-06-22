#include "Platform/Windows/Windows.h"
#include "AssetBuilder.h"

#include "AssetBuilderExceptions.h"
#include "BuilderInterface.h"
#include "BuilderOptions.h"
#include "BuilderStats.h"

#include <map>
#include <set>
#include <queue>
#include <stack>
#include <algorithm>

#include "Platform/Thread.h"
#include "Platform/Mutex.h"
#include "Platform/Windows/Windows.h"

#include "Foundation/Version.h"
#include "Foundation/CommandLine.h"
#include "Foundation/Environment.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Log.h"

#include "Pipeline/Asset/AssetInit.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/AssetBuilder/CacheFiles.h"
#include "Pipeline/Dependencies/Dependencies.h"

#include "Application/Application.h"
#include "Application/Exception.h"

using namespace Asset;
using namespace AssetBuilder;

static Nocturnal::InitializerStack g_InitializerStack;
static i32                         g_InitCount = 0;
static u32                         g_BuildStack = 0;
static u32                         g_NiceCount = 0;
static u32                         g_ThreadCount = 1;
static u32                         g_ProcessorCount = 1;
static bool                        g_InConcurrentBuild = false;
static S_tuid                      g_FailedAssets;

#ifdef PROFILE_ACCUMULATION
static Platform::Mutex g_ProfileMutex;

typedef std::map< std::string, Profile::Accumulator > M_StringToAccum;
static M_StringToAccum g_BuilderAccumulators;
static Profile::Accumulator g_InitializeAccum          ( "AssetBuilder Initialization" );
static Profile::Accumulator g_DependencyCheckAccum     ( "AssetBuilder Dependency Checking" );
static Profile::Accumulator g_GatherDependentJobsAccum ( "AssetBuilder Gather Jobs" );
static Profile::Accumulator g_DownloadAccum            ( "AssetBuilder Data Download" );
static Profile::Accumulator g_BuildAccum               ( "AssetBuilder Build" );
#endif

static f32 g_DownloadTime = 0.0f;
static f32 g_UploadTime = 0.0f;
static f32 g_BuildTime = 0.0f;
static f32 g_DependencyCheckTime = 0.0f;
static f32 g_InitializationTime = 0.0f;
static f32 g_SignatureCreationTime = 0.0f;
static f32 g_JobGatheringTime = 0.0f;
static f32 g_TotalTime = 0.0f;

const char* AssetBuilder::CommandArgs::HaltOnError = "halt_on_error";

static void ResetTimers()
{
    g_DownloadTime = 0.0f;
    g_UploadTime = 0.0f;
    g_BuildTime = 0.0f;
    g_DependencyCheckTime = 0.0f;
    g_InitializationTime = 0.0f;
    g_SignatureCreationTime = 0.0f;
    g_JobGatheringTime = 0.0f;
    g_TotalTime = 0.0f;
}

REFLECT_DEFINE_CLASS( AssetBuiltArgs );

void AssetBuiltArgs::EnumerateClass( Reflect::Compositor<AssetBuiltArgs>& comp )
{
    Reflect::Field* fieldAssetId = comp.AddField( &AssetBuiltArgs::m_AssetId, "m_AssetId" );
    Reflect::EnumerationField* enumBuildResult = comp.AddEnumerationField( &AssetBuiltArgs::m_JobResult, "m_JobResult" );
}

void AssetBuilder::Initialize()
{
    if ( ++g_InitCount == 1 )
    {
        g_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );
        g_InitializerStack.Push( CacheFiles::Initialize, CacheFiles::Cleanup );

        g_InitializerStack.Push( Reflect::RegisterClass<BuilderOptions>( "BuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<BuildRequest>( "BuildRequest" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<ZoneBuilderOptions>( "ZoneBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<LevelBuilderOptions>( "LevelBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<RegionBuilderOptions>( "RegionBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<CubeMapBuilderOptions>( "CubeMapBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<ShrubBuilderOptions>( "ShrubBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<TieBuilderOptions>( "TieBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<UfragBuilderOptions>( "UfragBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<ShaderBuilderOptions>( "ShaderBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<FoliageBuilderOptions>( "FoliageBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<FontBuilderOptions>( "FontBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<MobyBuilderOptions>( "MobyBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<MovieBuilderOptions>( "MovieBuilderOptions" ) ); 
        g_InitializerStack.Push( Reflect::RegisterClass<AnimationBuilderOptions>( "AnimationBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<SkyBuilderOptions>( "SkyBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<CinematicBuilderOptions>( "CinematicBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<TexturePackBuilderOptions>( "TexturePackBuilderOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<SymbolEnumAutoFixupOptions>( "SymbolEnumAutoFixupOptions" ) );
        g_InitializerStack.Push( Reflect::RegisterEnumeration<JobResult>( &JobResults::JobResultEnumerateEnumeration, "JobResult" ) );
        g_InitializerStack.Push( Reflect::RegisterClass<AssetBuiltArgs>( "AssetBuiltArgs" ) );

        g_InitializerStack.Push( BuilderInterface::Initialize, BuilderInterface::Cleanup );
        g_InitializerStack.Push( BuilderStats::Initialize, BuilderStats::Cleanup );

        SYSTEM_INFO info;
        GetSystemInfo( &info );
        g_ProcessorCount = info.dwNumberOfProcessors;
        Log::Print( "Computer has %d processors\n", g_ProcessorCount );

        // by default we use a thread per processor
        g_ThreadCount = g_ProcessorCount;

        // read the number of processors to nice (leave for other processes)
        Nocturnal::GetCmdLineArg( "nice", g_NiceCount );

        // resort to single thread if we get a flag from env or cmd line
        if ( Nocturnal::GetEnvFlag( "IG_ASSET_BUILDER_SINGLE_THREAD" ) || Nocturnal::GetCmdLineFlag( "single_thread" ) )
        {
            g_NiceCount = 0;
            g_ThreadCount = 1;
        }
    }
}

void AssetBuilder::Cleanup()
{
    if ( --g_InitCount == 0 )
    {
        g_FailedAssets.clear();
        g_InitializerStack.Cleanup();
    }
}

static AssetBuiltSignature::Event g_AssetBuiltEvent;

void AssetBuilder::AddAssetBuiltListener( const AssetBuiltSignature::Delegate& listener )
{
    g_AssetBuiltEvent.Add( listener );
}

void AssetBuilder::RemoveAssetBuiltListener( const AssetBuiltSignature::Delegate& listener )
{
    g_AssetBuiltEvent.Remove( listener );
}

class PrintListener
{
public:
    PrintListener( Log::V_Statement& consoleOutput, u32& warningCount, u32& errorCount, bool throttle )
        : m_LogOutput( consoleOutput )
        , m_WarningCount( warningCount )
        , m_ErrorCount( errorCount )
        , m_Throttle( throttle )
        , m_Thread( GetCurrentThreadId() )
    {
        Start();
    }

    ~PrintListener()
    {
        Stop();
    }

    void Start()
    {
        Log::AddPrintingListener( Log::PrintingSignature::Delegate( this, &PrintListener::Listener ) );
    }

    void Stop()
    {
        Log::RemovePrintingListener( Log::PrintingSignature::Delegate( this, &PrintListener::Listener ) );
    }

    void Dump()
    {
        Stop();

        Log::PrintStatements( m_LogOutput );
    }

    u32 GetWarningCount()
    {
        return m_WarningCount;
    }

    u32 GetErrorCount()
    {
        return m_ErrorCount;
    }

private:
    void Listener( Log::PrintingArgs& args )
    {
        if ( m_Thread == GetCurrentThreadId() )
        {
            if ( args.m_Statement.m_Stream == Log::Streams::Warning )
            {
                ++m_WarningCount;
            }

            if ( args.m_Statement.m_Stream == Log::Streams::Error )
            {
                ++m_ErrorCount;
            }

            if ( m_Throttle )
            {
                m_LogOutput.push_back( args.m_Statement );
                args.m_Skip = true;
            }
        }
    }

private:
    Log::V_Statement& m_LogOutput;
    u32&                  m_WarningCount;
    u32&                  m_ErrorCount;
    bool                  m_Throttle;
    u32                   m_Thread;
};

void PrintJobList( const V_BuildJob& jobs )
{
    int i = 0;
    for( V_BuildJob::const_iterator itr = jobs.begin(); itr != jobs.end(); ++itr )
    {
        Log::Print( "%d: %x / %s / %s / "TUID_HEX_FORMAT"\n", i++, *(u32*)( (void*)(*itr)->m_Builder ), (*itr)->m_Asset->GetShortName().c_str(), (*itr)->m_Builder->GetBuildString().c_str(), (*itr)->m_Asset->GetPath().Hash() );
    }
}

void TrimJobList( V_BuildJob& jobs )
{
    std::set< std::string > jobSignatures;

    //  Log::Print( "Pre-trim:\n" );
    //  PrintJobList( jobs );

    for( int i = 0; i < (int) jobs.size(); )
    {
        BuildJob* job = jobs[ i ];

#pragma TODO( "We're using the vtable of the builder here to generate a unique ID per builder/asset pair.  We should do this differently in the future." )
        std::stringstream buildSignature;
        buildSignature << job->m_Asset->GetPath().Hash() << job->m_Builder->GetBuildString() << std::hex << *(u32*)( (void*)job->m_Builder );

        if ( jobSignatures.find( buildSignature.str() ) != jobSignatures.end() )
        {
            jobs.erase( jobs.begin() + i );
        }
        else
        {
            jobSignatures.insert( buildSignature.str() );
            ++i;
        }
    }

    //  Log::Print( "\n\nPost-trim:\n" );
    //  PrintJobList( jobs );
}

JobResult InvokeBuild( BuildJob* job, bool throttle )
{
    IBuilder* builder = job->m_Builder;

    JobResult result = JobResults::Clean;
    const AssetClassPtr& assetClass = builder->GetAssetClass();

    const type_info& builderInfo = typeid( *builder );
    std::string builderName = builderInfo.name();

    // rtti name comes out as "class AssetBuilder::...", grab just the last part
    size_t pos = builderName.rfind( ':' );
    if ( pos != std::string::npos )
    {
        builderName = builderName.substr( pos + 1 );
    }

    // if the builder is not inside a namespace, strip off the beginning "class"
    pos = builderName.find( "class" );
    if ( pos != std::string::npos )
    {
        // 6 is the length of the word "class" plus the space after it
        builderName = builderName.substr( pos + 6 );
    }

#ifdef PROFILE_ACCUMULATION
    {
        Platform::TakeMutex mutex ( g_ProfileMutex );
        g_BuilderAccumulators[ builderName ].Init(  (builderName + std::string( " Build" ) ).c_str() );
    }

    PROFILE_SCOPE_ACCUM( g_BuilderAccumulators[ builderName ] );
#endif

    Nocturnal::Path outputDirectory( builder->GetOutputDirectory() );
    outputDirectory.MakePath();

    // add traces
    std::string traceString = builder->GetBuildString();
    while ( ( pos = traceString.find( '/' ) ) != std::string::npos )
    {
        traceString.replace( pos, 1, "$" );
    }
    while ( ( pos = traceString.find( '\'' ) ) != std::string::npos )
    {
        traceString.erase( pos, 1 );
    }

    std::string traceFile = outputDirectory.Get() + traceString + "_" + "trace.txt";
    std::string warningFile = outputDirectory.Get() + traceString + "_" + "warning.txt";
    std::string errorFile = outputDirectory.Get() + traceString + "_" + "error.txt";

    Log::TraceFileHandle trace ( traceFile, Application::GetTraceStreams(), GetCurrentThreadId() );
    Log::TraceFileHandle warning ( warningFile, Log::Streams::Warning, GetCurrentThreadId() );
    Log::TraceFileHandle error ( errorFile, Log::Streams::Error, GetCurrentThreadId() );

    std::string failureReason = "Unknown failure";
    try
    {
        Profile::Timer timer;

        // print some state
        Log::Print( "Building %s\n", builder->GetBuildString().c_str() );
        Log::Print( Log::Levels::Verbose,  " TUID: "TUID_HEX_FORMAT"\n", assetClass->GetPath().Hash() );

        // capture this thread's console output
        PrintListener printListener (job->m_LogOutput, job->m_WarningCount, job->m_ErrorCount, throttle);

        result = builder->Build() ? JobResults::Clean : JobResults::Dirty;

        if ( printListener.GetErrorCount() > 0 || printListener.GetWarningCount() > 0 )
        {
            printListener.Dump();
        }

        if ( printListener.GetErrorCount() > 0 )
        {
            result = JobResults::Failure;
            failureReason = "Errors occurred within the builder";
        }

        if ( result == JobResults::Clean )
        {
            Profile::Timer dependencyTimer;

            if ( job->m_DependencyGraph )
            {
                job->m_DependencyGraph->UpdateOutputs( job->m_OutputFiles );
            }

            g_DependencyCheckTime += dependencyTimer.Elapsed();
        }

        f32 elapsed = timer.Elapsed();

        // do some validation
        elapsed = Math::IsValid( elapsed ) ? elapsed : 0.f;

        g_BuildTime += elapsed;

        BuilderStats::AddBuild( assetClass->GetPath(), assetClass->GetAssetType(), builderName, elapsed );
    }
    catch( const Nocturnal::Exception& e )
    {
        result = JobResults::Failure;
        failureReason = e.what();

        if ( Nocturnal::GetCmdLineFlag( CommandArgs::HaltOnError ) )
        {
            throw;
        }

        Debug::ProcessException( e );
    }

    if ( result == JobResults::Failure )
    {
        g_FailedAssets.insert( assetClass->GetPath().Hash() );
        Log::Error( "Failure Building %s: %s\n", builder->GetBuildString().c_str(), failureReason.c_str() );
    }

    // free our reference to this builder.  In most cases this will entirely free the builder
    job->m_Builder = NULL; 

    return result;
}

// Process jobs created by "job", adding them to jobList
void ProcessNewJobs( const BuildJobPtr& job, V_BuildJob& newJobs, V_BuildJob& jobList )
{
    for ( u32 i = 0; i < (u32)newJobs.size(); ++i )
    {
        const BuildJobPtr newJob = newJobs[ i ];

        if ( newJob->m_Builder )
        {
            if ( !newJob->m_Options.ReferencesObject() )
            {
                newJob->m_Options = new BuilderOptions;
            }

            // if the new job is required but its parent is not, then the new job is also not required
            if ( !(job->m_Flags & JobFlags::Required) )
            {
                newJob->m_Flags &= ~JobFlags::Required;
            }

            // if the new job is required only if this is a top level build, and we're at the top level
            // of the build, set it as required
            if ( newJob->m_Flags & JobFlags::RequiredOnlyInTopLevelBuild )
            {
                newJob->m_Flags &= ~JobFlags::RequiredOnlyInTopLevelBuild;

                if ( g_BuildStack == 1 )
                {
                    newJob->m_Flags &= JobFlags::Required;
                }
            }

            jobList.push_back( newJob );
        }
        else
        {
            // If a builder has not been allocated, we need to create new jobs for as many builders as the
            // builder DLL allocates.  We then push new jobs, with the builder set onto the back of the
            // current list

            V_IBuilder builders;
            BuilderInterface::AllocateBuilders( newJob->m_Asset->GetAssetType(), builders );

            V_IBuilder::const_iterator builderItr = builders.begin();
            V_IBuilder::const_iterator builderEnd = builders.end();
            for ( ; builderItr != builderEnd; ++builderItr )
            {
                newJobs.push_back( new BuildJob( newJob->m_DependencyGraph, newJob->m_Asset, newJob->m_Options, *builderItr, newJob->m_Flags ) );
            }
        }
    }
}

bool BuildDependentJobs(V_BuildJob& allJobs, Dependencies::DependencyGraph& graph, u32 pass)
{
    // gather post-dependent jobs
    std::stringstream message; 
    if (pass == 0)
    {
        message << "Gathering dependent jobs" << std::endl; 
    }
    else
    {
        message << "Gathering dependent jobs, pass " << pass << std::endl; 
    }

    V_BuildJob dependentJobs; 

    {
        Log::Bullet bullet( message.str().c_str() ); 

        V_BuildJob::const_iterator jobItr = allJobs.begin();
        V_BuildJob::const_iterator jobEnd = allJobs.end();
        for ( ; jobItr != jobEnd; ++jobItr )
        {
            BuildJob* job = *jobItr;

            if ( job->m_Result == JobResults::Skip )
            {
                continue;
            }

            try
            {
                PROFILE_SCOPE_ACCUM( g_GatherDependentJobsAccum );

                Profile::Timer timer;

                V_BuildJob newJobs; 
                V_BuildJob allNewJobs; 

                job->m_Builder->GatherDependentJobs( newJobs, pass ); 

                ProcessNewJobs( job, newJobs, allNewJobs ); 
                dependentJobs.insert( dependentJobs.end(), allNewJobs.begin(), allNewJobs.end()); 

                g_JobGatheringTime += timer.Elapsed();
            }
            catch( const Nocturnal::Exception& e )
            {
                std::stringstream error;
                error << "Error gathering post-dependent jobs for job '" << job->m_Asset->GetFullName().c_str() << "': " << e.what();

                job->m_Result = JobResults::Failure;
                g_FailedAssets.insert( job->m_Asset->GetPath().Hash() );

                if ( job->m_Flags & JobFlags::Required || Nocturnal::GetCmdLineFlag( CommandArgs::HaltOnError ) )
                {
                    throw Nocturnal::Exception( error.str().c_str() );
                }
                else
                {
                    Log::Error( "%s\n", error.str().c_str() );
                }

                Debug::ProcessException( e );
            }
        }
    }

    if ( !dependentJobs.empty() )
    {
        std::stringstream message; 
        if(pass == 0)
        {
            message << "Building dependent jobs" << std::endl; 
        }
        else
        {
            message << "Building dependent jobs, pass " << pass << std::endl; 
        }

        Log::Bullet bullet( message.str().c_str() ); 

        // note that dependentJobs will be empty after this call, because Build
        // clears out its passed in job list ... hmmm. 
        Build( graph, dependentJobs );

        return true; 
    }

    return false; 
}

typedef std::queue< BuildJob* > Q_BuildJob;

Platform::Thread::Return BuildThread( Platform::Thread::Param param )
{
    Platform::Locker<Q_BuildJob>* queue = (Platform::Locker<Q_BuildJob>*)param;

    while ( 1 )
    {
        BuildJob* job = NULL;

        {
            Platform::Locker<Q_BuildJob>::Handle jobs = queue->Lock();
            if ( !jobs->empty() )
            {
                job = jobs->front();
                jobs->pop();
            }
        }

        if ( !job )
        {
            break;
        }

        job->m_Result = JobResults::Failure;
        job->m_Result = InvokeBuild( job, true );
    }

    return Platform::Thread::Exit();
}

Platform::Thread::Return BuildThreadEntry( Platform::Thread::Param param )
{
    return Application::StandardThread( &BuildThread, param );
}

void InvokeBuilds( V_BuildJob& jobs, i32 nice )
{
    u32 threadCount = g_ThreadCount;

    if ( nice < 0 )
    {
        nice = g_NiceCount;
    }

    if ( nice > 0 && g_ProcessorCount - nice > 1 )
    {
        threadCount = g_ProcessorCount - nice;
    }

    if ( threadCount > 1 && !g_InConcurrentBuild )
    {
        g_InConcurrentBuild = true;

        typedef std::map< std::string, u32 > M_Affinity;
        M_Affinity threadAffinity;

        V_BuildJob remainingJobs;
        Platform::Thread* threads = new Platform::Thread[ threadCount ];

        Q_BuildJob foregroundJobs;
        Platform::Locker<Q_BuildJob> backgroundJobs;

        // lock and populate the foreground and background jobs
        {
            Platform::Locker<Q_BuildJob>::Handle backgroundJobsHandle = backgroundJobs.Lock();

            // first find and add any jobs that need to be in a single thread
            V_BuildJob::iterator jobItr = jobs.begin();
            V_BuildJob::iterator jobEnd = jobs.end();
            while ( jobItr != jobEnd )
            {
                BuildJobPtr& job = *jobItr;

                if ( job->m_Builder->NeedsThreadAffinity() || jobs.size() == 1 )
                {
                    foregroundJobs.push( job );
                }
                else
                {
                    backgroundJobsHandle->push( job );
                }

                ++jobItr;
            }
        }

        size_t backgroundJobCount = jobs.size() - foregroundJobs.size();
        Log::Print("Foreground jobs: %d, Background jobs: %d, Threads: %d\n", foregroundJobs.size(), backgroundJobCount, threadCount);

        // create the build threads for the background jobs
        for ( u32 i = 0;i < threadCount; ++i )
        {
            threads[ i ].Create( BuildThreadEntry, &backgroundJobs, "AssetBuilder Build Thread" );
        }

        // run foreground jobs
        size_t size = foregroundJobs.size();
        while ( !foregroundJobs.empty() )
        {
            BuildJob* job = foregroundJobs.front();
            foregroundJobs.pop();

            PROFILE_SCOPE_ACCUM( g_BuildAccum );
            job->m_Result = InvokeBuild( job, backgroundJobCount > 0 );
        }

        // wait for background jobs to finish
        for ( u32 i = 0;i < threadCount; ++i )
        {
            threads[ i ].Wait();
        }

        delete [] threads;

        g_InConcurrentBuild = false;
    }
    else
    {
        u32 count = 0;
        V_BuildJob::iterator jobItr = jobs.begin();
        V_BuildJob::iterator jobEnd = jobs.end();
        for ( ; jobItr != jobEnd; ++jobItr )
        {
            BuildJob* job = *jobItr;

            PROFILE_SCOPE_ACCUM( g_BuildAccum );
            job->m_Result = InvokeBuild( job, jobs.size() > 1 );
        }
    }
}

/*
See http://wiki/index.php/AssetBuilder
*/
void AssetBuilder::Build( Dependencies::DependencyGraph& graph, V_BuildJob& jobs, i32 nice )
{
    g_BuildStack++;

    std::ostringstream message;
    if ( jobs.size() == 1 )
    {
        message << jobs.front()->m_Asset->GetFullName();
    }
    else
    {
        message << jobs.size() << " jobs";
    }

    Log::Bullet bullet( "Build Level %d (%s)\n", g_BuildStack, message.str().c_str() );

    V_BuildJob allJobs;
    allJobs.reserve( jobs.size() );

    {
        PROFILE_SCOPE_ACCUM( g_InitializeAccum );

        //
        // Don't use iterators, because this jobs list gets appended to
        //  Always check the size of the list rather than storing it out,
        //  because we need to process any appended items
        //

        for ( u32 i = 0; i < (u32)jobs.size(); ++i )
        {
            Profile::Timer timer;

            BuildJob* job = jobs[ i ];

            if ( !job->m_Options )
            {
                job->m_Options = new BuilderOptions ();
            }

            if ( !job->m_Builder )
            {
                // If a builder has not been allocated, we need to create new jobs for as many builders as the
                // builder DLL allocates.  We then push new jobs, with the builder set onto the back of the
                // current list

                V_IBuilder builders;
                BuilderInterface::AllocateBuilders( job->m_Asset->GetAssetType(), builders );

                // if only one builder was allocated, set it in the current job
                if ( builders.size() == 1 )
                {
                    job->m_Builder = builders.front();
                }
                else
                {
                    // if multiple builders are necessary, allocate new jobs for them, and skip the current one
                    V_IBuilder::const_iterator builderItr = builders.begin();
                    V_IBuilder::const_iterator builderEnd = builders.end();
                    for ( ; builderItr != builderEnd; ++builderItr )
                    {
                        jobs.push_back( new BuildJob( job->m_DependencyGraph, job->m_Asset, job->m_Options, *builderItr, job->m_Flags ) );
                    }

                    continue;
                }
            }

            try
            {
                job->m_Builder->Initialize( job->m_Asset, job->m_Options );
                job->m_BuildString = job->m_Builder->GetBuildString();
            }
            catch( const Nocturnal::Exception& e )
            {
                std::stringstream error;
                error << "Error initializing builder for job '" << job->m_Asset->GetFullName().c_str() << "': " << e.what();

                job->m_Result = JobResults::Failure;
                g_FailedAssets.insert( job->m_Asset->GetPath().Hash() );

                if ( job->m_Flags & JobFlags::Required || Nocturnal::GetCmdLineFlag( CommandArgs::HaltOnError ) )
                {
                    g_BuildStack--;
                    throw Nocturnal::Exception( error.str().c_str() );
                }
                else
                {
                    Log::Error( "%s\n", error.str().c_str() );
                }

                Debug::ProcessException( e );
                continue;
            }

            g_InitializationTime += timer.Elapsed();

            Log::Bullet bullet ( Log::Streams::Normal, Log::Levels::Verbose, "Initialized %s\n", job->m_BuildString.c_str() );

            V_BuildJob newJobs;
            try
            {
                PROFILE_SCOPE_ACCUM( g_GatherDependentJobsAccum );

                Profile::Timer timer;

                job->m_Builder->GatherJobs( newJobs );

                ProcessNewJobs( job, newJobs, job->m_DependentJobs );
                jobs.insert( jobs.end(), job->m_DependentJobs.begin(), job->m_DependentJobs.end() );

                g_JobGatheringTime += timer.Elapsed();
            }
            catch( const Nocturnal::Exception& e )
            {
                std::stringstream error;
                error << "Error gathering jobs for job '" << job->m_Asset->GetFullName().c_str() << "': " << e.what();

                job->m_Result = JobResults::Failure;
                g_FailedAssets.insert( job->m_Asset->GetPath().Hash() );

                if ( job->m_Flags & JobFlags::Required || Nocturnal::GetCmdLineFlag( CommandArgs::HaltOnError )  )
                {
                    g_BuildStack--;
                    throw Nocturnal::Exception( error.str().c_str() );
                }
                else
                {
                    Log::Error( "%s\n", error.str().c_str() );
                }

                Debug::ProcessException( e );
                continue;
            }

            allJobs.push_back( job );
        }
    }

    jobs.clear();

    TrimJobList( allJobs );

    // Build up a list of out-of-date (required) builds, and a mapping from builder to the output files
    // associated with it.
    Dependencies::V_DependencyInfo allOutputFiles;
    Dependencies::V_DependencyInfo downloadOutputFiles;
    bool allUpToDate = true;
    {
        PROFILE_SCOPE_ACCUM( g_DependencyCheckAccum );

        Log::Bullet bullet( "Checking dependencies\n" );

        V_BuildJob::const_iterator jobItr = allJobs.begin();
        V_BuildJob::const_iterator jobEnd = allJobs.end();
        for ( ; jobItr != jobEnd; ++jobItr )
        {
            BuildJob* job = *jobItr;

            try
            {
                bool needsPregisterInputs = job->m_Builder->NeedsPreRegisterInputs();
                if ( needsPregisterInputs )
                {
                    Profile::Timer timer;
                    job->m_Builder->RegisterInputs( job->m_OutputFiles );
                    g_DependencyCheckTime += timer.Elapsed();
                }

                Profile::Timer timer;
                bool upToDate = job->m_Builder->IsUpToDate();
                g_DependencyCheckTime += timer.Elapsed();

                if ( !upToDate )
                {
                    allUpToDate = false;

                    Log::Bullet bullet ( Log::Streams::Normal, Log::Levels::Verbose, "Out-of-date: %s\n", job->m_BuildString.c_str() );

                    if ( !needsPregisterInputs )
                    {
                        Profile::Timer timer;
                        job->m_Builder->RegisterInputs( job->m_OutputFiles  );
                        g_DependencyCheckTime += timer.Elapsed();
                    }

                    allOutputFiles.insert( allOutputFiles.end(), job->m_OutputFiles.begin(), job->m_OutputFiles.end() );

                    downloadOutputFiles.insert( downloadOutputFiles.end(), job->m_OutputFiles.begin(), job->m_OutputFiles.end() );
                }
                else
                {
                    job->m_Result = JobResults::Skip;
                }
            }
            catch( Nocturnal::Exception& e )
            {
                std::stringstream error;
                error << "Error registering inputs for job '" << job->m_Asset->GetFullName().c_str() << "': " << e.what();
                e.Set( error.str() );

                job->m_Result = JobResults::Failure;
                g_FailedAssets.insert( job->m_Asset->GetPath().Hash() );

                if ( job->m_Flags & JobFlags::Required || Nocturnal::GetCmdLineFlag( CommandArgs::HaltOnError ) )
                {
                    g_BuildStack--;
                    throw;
                }
                else
                {
                    Log::Error( "%s\n", error.str().c_str() );
                }

                Debug::ProcessException( e );
            }
        }
    }

    // build the dependent jobs, all long as the builders report any
    //
    u32 dependentJobsPass = 0; 
    bool buildDependentJobs = true; 
    while (buildDependentJobs)
    {
        TrimJobList( allJobs );
        buildDependentJobs = BuildDependentJobs(allJobs, graph, dependentJobsPass); 
        dependentJobsPass++; 
    }

    if ( !allOutputFiles.empty() )
    {
        Profile::Timer timer;
        Log::Bullet bullet( "Creating signatures\n" );

        // Create signatures on ALL the output files, since we will need them for uploading even if we're forcing the build
        graph.CreateSignatures( allOutputFiles, true );

        g_SignatureCreationTime += timer.Elapsed();
    }

    V_BuildJob::const_iterator jobItr = allJobs.begin();
    V_BuildJob::const_iterator jobEnd = allJobs.end();
    for ( ; jobItr != jobEnd; ++jobItr )
    {
        BuildJob* job = *jobItr;
        job->m_Builder->GatherPostJobs( job->m_PostJobs ); 
    }

    // do we have real builds to run on this tier? 
    // 
    if ( !allUpToDate )
    {
        if ( !downloadOutputFiles.empty() )
        {
            PROFILE_SCOPE_ACCUM( g_DownloadAccum );

            Profile::Timer timer;
            Log::Bullet bullet ( "Downloading data\n" );

            CacheFiles::Get( graph, downloadOutputFiles );

            g_DownloadTime += timer.Elapsed();
        }

        // Find the builders whose data could not all be downloaded
        V_BuildJob requiredJobs;
        requiredJobs.reserve( allJobs.size() );

        Dependencies::V_DependencyInfo outputFilesToUpdate;
        V_BuildJob::const_iterator jobItr = allJobs.begin();
        V_BuildJob::const_iterator jobEnd = allJobs.end();
        for ( ; jobItr != jobEnd; ++jobItr )
        {
            BuildJob* job = *jobItr;
            IBuilder* builder = job->m_Builder;

            if ( job->m_Result == JobResults::Skip )
            {
                continue;
            }

            const Dependencies::V_DependencyInfo& files = job->m_OutputFiles;

            bool downloaded = true;
            // special case -- builder returned no output files, so is not downloadable
            if ( files.empty() )
            {
                downloaded = false;
            }
            else
            {
                Dependencies::V_DependencyInfo::const_iterator fileItr = files.begin();
                Dependencies::V_DependencyInfo::const_iterator fileEnd = files.end();
                for ( ; fileItr != fileEnd; ++fileItr )
                {

                    const Dependencies::DependencyInfo* file =  &(**fileItr);
                    if ( !file->m_Downloaded )
                    {
                        downloaded = false;
                        break;
                    }
                }
            }

            // if we successfully downloaded, add the output files to the list of files to call
            // UpdateOutputs on, and raise the event notifying that this build is done
            // otherwise, add the builder to the list of required builders
            if ( downloaded )
            {
                outputFilesToUpdate.insert( outputFilesToUpdate.end(), files.begin(), files.end() );

                job->m_Result = JobResults::Download;

                // now raise the event notifying that this asset has been downloaded
                AssetBuiltArgsPtr builtArgs = new AssetBuiltArgs( builder->GetAssetClass()->GetPath().Hash(), JobResults::Download );
                g_AssetBuiltEvent.Raise( builtArgs );
            }
            else
            {
                requiredJobs.push_back( job );
            }
        }

        if ( !outputFilesToUpdate.empty() )
        {
            Profile::Timer timer;

            Log::Bullet bullet( "Updating %d outputs\n", outputFilesToUpdate.size() );
            graph.UpdateOutputs( outputFilesToUpdate );

            g_DependencyCheckTime += timer.Elapsed();

            outputFilesToUpdate.clear();
        }

        {
            // Build each of the required builders
            TrimJobList( requiredJobs );
            Log::Bullet bullet ("Running %d jobs\n", requiredJobs.size() );
            InvokeBuilds( requiredJobs, nice );
        }

        Dependencies::V_DependencyInfo filesToUpload;

        {
            Log::Bullet bullet ("Checking %d results\n", requiredJobs.size() );

            // If the build failed, add it to our list of failed builds
            u32 count = 0;
            V_BuildJob::const_iterator reqJobItr = requiredJobs.begin();
            V_BuildJob::const_iterator reqJobEnd = requiredJobs.end();
            for ( ; reqJobItr != reqJobEnd; ++reqJobItr )
            {
                BuildJob* job = *reqJobItr;

                if ( job->m_WarningCount || job->m_ErrorCount )
                {
                    Log::Print( "%s:\n", job->m_BuildString.c_str() );
                    Log::PrintStatements( job->m_LogOutput, Log::Streams::Warning | Log::Streams::Error );
                }

                if ( job->m_Result == JobResults::Clean )
                {
                    outputFilesToUpdate.insert( outputFilesToUpdate.end(), job->m_OutputFiles.begin(), job->m_OutputFiles.end() );

                    // now raise the event notifying that this asset has been built
                    AssetBuiltArgsPtr builtArgs = new AssetBuiltArgs( job->m_Asset->GetPath().Hash(), JobResults::Clean );
                    g_AssetBuiltEvent.Raise( builtArgs );

                    filesToUpload.insert( filesToUpload.end(), job->m_OutputFiles.begin(), job->m_OutputFiles.end() );
                }
                else
                {
                    g_FailedAssets.insert( job->m_Asset->GetPath().Hash() );

                    if ( job->m_Result == JobResults::Failure && job->m_Flags & JobFlags::Required )
                    {
                        g_BuildStack--;
                        throw Nocturnal::Exception( "Required job '%s' failed to build.  See above for more errors\n", job->m_BuildString.c_str() );
                    }
                }
            }
        }

        requiredJobs.clear();

        // now upload any output files that were built
        if ( !filesToUpload.empty() )
        {
            Log::Bullet bullet( "Uploading new data\n" );

            Profile::Timer timer;

            CacheFiles::Put( graph, filesToUpload );

            g_UploadTime += timer.Elapsed();
        }

        // validate and fail jobs whose required jobs failed (even if the current job succeeded)
        jobItr = allJobs.begin();
        jobEnd = allJobs.end();
        for ( ; jobItr != jobEnd; ++jobItr )
        {
            BuildJob* job = *jobItr;

            V_BuildJob::const_iterator nestedItr = job->m_DependentJobs.begin();
            V_BuildJob::const_iterator nestedEnd = job->m_DependentJobs.end();
            for ( ; nestedItr != nestedEnd; ++nestedItr )
            {
                BuildJob* nestedJob = *nestedItr;
                if ( ( nestedJob->m_OriginalFlags & JobFlags::Required ) && DidBuildFail( nestedJob->m_Asset->GetPath().Hash() ) )
                {
                    job->m_Result = JobResults::Failure;
                    g_FailedAssets.insert( job->m_Asset->GetPath().Hash() );
                    Log::Print( Log::Levels::Verbose, "Job '%s' marked as failed, because required job '%s' failed\n", job->m_BuildString.c_str(), nestedJob->m_BuildString.c_str() );
                    break;
                }
            }
        }
    }
    else
    {
        Log::Bullet bullet( "All files up-to-date.\n" );
    }

    // post jobs
    V_BuildJob allPostJobs; 
    jobItr = allJobs.begin();
    jobEnd = allJobs.end();
    for ( ; jobItr != jobEnd; ++jobItr )
    {
        BuildJob* job = *jobItr;

        if(job->m_PostJobs.size() == 0)
        {
            continue; 
        }

        ProcessNewJobs( job, job->m_PostJobs, allPostJobs ); 
    }

    // Build all the post jobs
    if(!allPostJobs.empty())
    {
        Log::Bullet enter( "Building %d post jobs\n", allPostJobs.size() ); 

        Build( graph, allPostJobs ); 
    }

    // fail any jobs on this level if their post jobs failed
    jobItr = allJobs.begin();
    jobEnd = allJobs.end();
    for ( ; jobItr != jobEnd; ++jobItr )
    {
        BuildJob* job = *jobItr;

        V_BuildJob::const_iterator nestedItr = job->m_DependentJobs.begin();
        V_BuildJob::const_iterator nestedEnd = job->m_DependentJobs.end();
        for ( ; nestedItr != nestedEnd; ++nestedItr )
        {
            BuildJob* nestedJob = *nestedItr;
            if ( ( nestedJob->m_OriginalFlags & JobFlags::Required ) && DidBuildFail( nestedJob->m_Asset->GetPath().Hash() ) )
            {
                job->m_Result = JobResults::Failure;
                g_FailedAssets.insert( job->m_Asset->GetPath().Hash() );
                Log::Print( Log::Levels::Verbose, "Job '%s' marked as failed, because required job '%s' failed\n", job->m_BuildString.c_str(), nestedJob->m_BuildString.c_str() );
                break;
            }
        }
    }

    g_BuildStack--;

    if ( g_BuildStack == 0 )
    {
        g_FailedAssets.clear();
    }
}

void AssetBuilder::Build( Dependencies::DependencyGraph& graph, const BuildJobPtr& job )
{
    V_BuildJob jobs;
    jobs.push_back( job );

    Build( graph, jobs );
}

void SendTopLevelBuild( const AssetClassPtr& assetClass )
{
    f32 unaccounted = g_TotalTime;
    unaccounted -= g_DependencyCheckTime;
    unaccounted -= g_DownloadTime;
    unaccounted -= g_UploadTime;
    unaccounted -= g_BuildTime;
    unaccounted -= g_InitializationTime;
    unaccounted -= g_JobGatheringTime;
    unaccounted -= g_SignatureCreationTime;

    BuilderStats::TopLevelBuild topLevelBuild;
    topLevelBuild.m_DependencyCheckTime = g_DependencyCheckTime;
    topLevelBuild.m_DownloadTime = g_DownloadTime;
    topLevelBuild.m_UploadTime = g_UploadTime;
    topLevelBuild.m_BuildTime = g_BuildTime;
    topLevelBuild.m_InitializationTime = g_InitializationTime;
    topLevelBuild.m_JobGatheringTime = g_JobGatheringTime;
    topLevelBuild.m_SignatureCreationTime = g_SignatureCreationTime;
    topLevelBuild.m_TotalTime = g_TotalTime;
    topLevelBuild.m_UnaccountedTime = unaccounted;
    BuilderStats::AddTopLevelBuild( assetClass->GetPath(), assetClass->GetAssetType(), topLevelBuild );

    Log::Profile( "Top level build breakdown:\n" );
    Log::Profile( "\tDependency Checking:   %fs\n", ( g_DependencyCheckTime / 1000.0f ) );
    Log::Profile( "\tDownloading:           %fs\n", ( g_DownloadTime / 1000.0f ) );
    Log::Profile( "\tUploading:             %fs\n", ( g_UploadTime / 1000.0f ) );
    Log::Profile( "\tBuilding:              %fs\n", ( g_BuildTime / 1000.0f ) );
    Log::Profile( "\tInitialization:        %fs\n", ( g_InitializationTime / 1000.0f ) );
    Log::Profile( "\tJob Gathering:         %fs\n", ( g_JobGatheringTime / 1000.0f ) );
    Log::Profile( "\tSignature Creation:    %fs\n", ( g_SignatureCreationTime / 1000.0f ) );
    Log::Profile( "\tTotal:                 %fs\n", ( g_TotalTime / 1000.0f ) );
    Log::Profile( "\tUnaccounted for:       %fs\n", ( unaccounted / 1000.0f ) );
}

void AssetBuilder::Build( Dependencies::DependencyGraph& graph, const AssetClassPtr& assetClass, const BuilderOptionsPtr& options )
{
    Log::TraceFileHandle trace ( assetClass->GetBuiltDirectory().Get() + "trace.txt", Application::GetTraceStreams() );
    Log::TraceFileHandle warning ( assetClass->GetBuiltDirectory().Get() + "warning.txt", Log::Streams::Warning );
    Log::TraceFileHandle error ( assetClass->GetBuiltDirectory().Get() + "error.txt", Log::Streams::Error );

    Profile::Timer timer;

    if ( g_BuildStack == 0 )
    {
        ResetTimers();
    }

    V_IBuilder builders;
    BuilderInterface::AllocateBuilders( assetClass->GetAssetType(), builders );

    if ( builders.empty() )
    {
        throw AssetBuilder::Exception ("Unable allocate asset builder");
    }

    V_BuildJob jobs;
    V_IBuilder::const_iterator builderItr = builders.begin();
    V_IBuilder::const_iterator builderEnd = builders.end();
    for ( ; builderItr != builderEnd; ++builderItr )
    {
        jobs.push_back( new BuildJob( &graph, assetClass, options, *builderItr, JobFlags::Required ) );
    }

    Build( graph, jobs );

    if ( g_BuildStack == 0 )
    {
        g_TotalTime = timer.Elapsed();

        SendTopLevelBuild( assetClass );
    }
}

void AssetBuilder::Build( Dependencies::DependencyGraph& graph, const AssetClassPtr& assetClass, const std::vector< std::string >& options )
{
    Log::TraceFileHandle trace ( assetClass->GetBuiltDirectory().Get() + "trace.txt", Application::GetTraceStreams());
    Log::TraceFileHandle warning ( assetClass->GetBuiltDirectory().Get() + "warning.txt", Log::Streams::Warning );
    Log::TraceFileHandle error ( assetClass->GetBuiltDirectory().Get() + "error.txt", Log::Streams::Error );

    Profile::Timer timer;

    if ( g_BuildStack == 0 )
    {
        ResetTimers();
    }

    V_IBuilder builders;
    BuilderInterface::AllocateBuilders( assetClass->GetAssetType(), builders );

    if ( builders.empty() )
    {
        throw AssetBuilder::Exception ("Unable allocate asset builder");
    }

    V_BuildJob jobs;
    V_IBuilder::const_iterator builderItr = builders.begin();
    V_IBuilder::const_iterator builderEnd = builders.end();
    for ( ; builderItr != builderEnd; ++builderItr )
    {
        IBuilder* builder = *builderItr;

        BuilderOptionsPtr builderOptions = builder->ParseOptions( options );

        jobs.push_back( new BuildJob( &graph, assetClass, builderOptions, builder, JobFlags::Required ) );
    }

    builders.clear();

    Build( graph, jobs );

    if ( g_BuildStack == 0 )
    {
        g_TotalTime = timer.Elapsed();

        SendTopLevelBuild( assetClass );
    }
}

void AssetBuilder::Build( Dependencies::DependencyGraph& graph, Nocturnal::Path& path, const BuilderOptionsPtr& builderOptions )
{
    AssetClassPtr assetClass = AssetClass::LoadAssetClass( path );

    Build( graph, assetClass, builderOptions );
}

void AssetBuilder::Build( Dependencies::DependencyGraph& graph, Nocturnal::Path& path, const std::vector< std::string >& options )
{
    AssetClassPtr assetClass = AssetClass::LoadAssetClass( path );

    Build( graph, assetClass, options );
}

bool AssetBuilder::DidBuildFail( const tuid assetId )
{
    return ( g_FailedAssets.find( assetId ) != g_FailedAssets.end() );
}
