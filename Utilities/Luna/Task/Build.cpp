#include "Precompile.h"

#include <algorithm>
#include <strstream>

#include "Build.h"
#include "TaskOutputWindow.h"
#include "TaskOptionsDialog.h"
#include "ZoneSelectorDialog.h"
#include "RegionSelectorDialog.h"

#include "Application/Inspect/InspectInit.h"
#include "Application/Inspect/Reflect/InspectReflectInit.h"
#include "Application/Inspect/Reflect/ReflectInterpreter.h"

#include "Pipeline/Dependencies/Dependencies.h"

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Classes/SceneAsset.h"
#include "Pipeline/Component/ComponentHandle.h"
#include "Pipeline/AssetBuilder/AssetBuilder.h"
#include "Pipeline/AssetBuilder/BuilderOptions.h"

#include "Pipeline/Content/Nodes/Zone.h"
#include "Foundation/Reflect/Archive.h"

#include "Application/Application.h"
#include "Application/Worker/Process.h"
#include "Foundation/Log.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/IPC/Connection.h"
#include "Foundation/Exception.h"

using namespace AssetBuilder;
using namespace Luna;

HANDLE g_BuildThread = NULL;
bool g_BuildInProgress = false;
bool g_BuildCancelling = false;

static Worker::Process* g_WorkerProcess = NULL;
static TaskOutputWindow* g_OutputWindow;

static i32                         g_InitCount = 0;
static Nocturnal::InitializerStack g_InitializerStack;

void Build::Initialize()
{
    if ( ++g_InitCount == 1 )
    {
        g_InitializerStack.Push( InspectReflect::Initialize, InspectReflect::Cleanup );
        g_InitializerStack.Push( AssetBuilder::Initialize, AssetBuilder::Cleanup );
    }
}

void Build::Cleanup()
{
    if ( --g_InitCount == 0 )
    {
        // non-blocking builds dangle thier thread handles
        if ( g_BuildThread )
        {
            CloseHandle( g_BuildThread );
            g_BuildThread = NULL;
        }
        g_InitializerStack.Cleanup();
    }
}

static AssetBuilder::AssetBuiltSignature::Event g_AssetBuiltEvent;

void Luna::AddAssetBuiltListener( const AssetBuilder::AssetBuiltSignature::Delegate& listener )
{
    g_AssetBuiltEvent.Add( listener );
}

void Luna::RemoveAssetBuiltListener( const AssetBuilder::AssetBuiltSignature::Delegate& listener )
{
    g_AssetBuiltEvent.Remove( listener );
}

void BuildSignal(Nocturnal::Void)
{
    if (g_BuildInProgress)
    {
        // halt our build thread
        g_BuildCancelling = true;

        // murder the actual build process
        g_WorkerProcess->Kill();

        // release the process object (handle)
        Worker::Process::Release( g_WorkerProcess );

        // wait for it to complete
        WaitForSingleObject( g_BuildThread, INFINITE );

        // release the thread handle
        CloseHandle( g_BuildThread );
        g_BuildThread = NULL;

        // reset cancel state
        g_BuildCancelling = false;

        // raise finished event
        TaskFinishedArgs args;
        args.m_Result = TaskResults::Cancel;
        RaiseTaskFinished( args );

        // flag as dormant
        g_BuildInProgress = false;
    }
    else
    {
        if (g_OutputWindow)
        {
            g_OutputWindow->Destroy();
            g_OutputWindow = NULL;
        }
    }
}

struct BuildParams
{
    Nocturnal::S_Path m_Assets;
    AssetBuilder::BuilderOptionsPtr m_BuilderOptions;

    BuildParams()
        : m_BuilderOptions( NULL )
    {

    }
};

bool BuildEntry( const Nocturnal::S_Path& assets, AssetBuilder::BuilderOptionsPtr options, bool view )
{
    g_WorkerProcess = Worker::Process::Create( "BuildTool.exe" );

    if (!g_WorkerProcess->Start())
    {
        return false;
    }

    AssetBuilder::BuildRequestPtr req = new AssetBuilder::BuildRequest ();
    req->m_Assets = assets;
    req->m_Options = options;

    std::stringstream stream;

    try
    {
        Reflect::Archive::ToStream( req, stream, Reflect::ArchiveTypes::Binary );
    }
    catch ( Nocturnal::Exception& ex )
    {
        Log::Error( "%s\n", ex.what() );
        return false;
    }

    if (!g_WorkerProcess->Send(0x0, (u32)stream.str().length(), (const u8*)stream.str().c_str()))
    {
        return false;
    }

    while (g_WorkerProcess->Running() && !g_BuildCancelling)
    {
        IPC::Message* msg = g_WorkerProcess->Receive();

        if (!msg)
        {
            break;
        }

        u32 messageId = msg->GetID();
        if (messageId == Worker::ConsoleOutputMessage)
        {
            Worker::ConsoleOutput* output = (Worker::ConsoleOutput*)msg->GetData();

            Log::Statement statement ( output->m_String, output->m_Stream, output->m_Level, output->m_Indent );

            if (g_OutputWindow)
            {
                g_OutputWindow->PrintListener( Log::PrintedArgs ( statement ) );
            }
            else
            {
                Log::PrintStatement( statement );
            }
        }
        else if ( messageId == 0x1 )
        {
            std::strstream stream ((char*)msg->GetData(), msg->GetSize());

            AssetBuilder::AssetBuiltArgsPtr assetBuiltArgs = Reflect::ObjectCast<AssetBuilder::AssetBuiltArgs> (Reflect::Archive::FromStream(stream, Reflect::ArchiveTypes::Binary, Reflect::GetType<AssetBuilder::AssetBuiltArgs>()));
            if ( assetBuiltArgs.ReferencesObject() )
            {
                g_AssetBuiltEvent.Raise( assetBuiltArgs );
            }
        }

        delete msg;
    }

    if (g_BuildCancelling)
    {
        ExitThread( 0 );
        return 0;
    }
    else
    {
        int result = g_WorkerProcess->Finish();

        Worker::Process::Release( g_WorkerProcess );

        return result == 0;
    }
}

DWORD WINAPI BuildThread( LPVOID lpParam )
{
    bool success = true;

    BuildParams* params = static_cast< BuildParams* >( lpParam );

    Nocturnal::S_Path assets;

    for ( Nocturnal::S_Path::const_iterator itr = params->m_Assets.begin(), end = params->m_Assets.end(); itr != end; ++itr )
    {
        const Nocturnal::Path& assetPath = (*itr);

        Asset::AssetClassPtr assetClass = Asset::AssetClass::LoadAssetClass( assetPath );

        if ( assetClass->GetAssetType() == Asset::AssetTypes::Level )
        {
            if ( !params->m_BuilderOptions.ReferencesObject() )
            {
                params->m_BuilderOptions = new AssetBuilder::LevelBuilderOptions;
            }

            assets.insert( assetPath );
        }
        else if ( assetClass->GetAssetType() == Asset::AssetTypes::Cinematic )
        {
            if ( !params->m_BuilderOptions.ReferencesObject() )
            {
                params->m_BuilderOptions = new AssetBuilder::LevelBuilderOptions;
            }

            assets.insert( assetPath );
        }
        else
        {
            assets.insert( assetPath );
        }
    }

    success = BuildEntry( assets, params->m_BuilderOptions, false );

    if ( !g_BuildCancelling )
    {
        // notify of successful finish
        BuildFinishedArgs args;
        args.m_Result = success ? TaskResults::Success : TaskResults::Failure;
        args.m_Assets = params->m_Assets;
        args.m_BuilderOptions = params->m_BuilderOptions;
        RaiseTaskFinished( args );
    }

    g_BuildInProgress = false;

    delete params;

    return success ? 0 : 1;
}

AssetBuilder::BuilderOptionsPtr CreateBuilderOptions( const Nocturnal::S_Path& assets )
{
    Asset::AssetType assetType = Asset::AssetTypes::Null;
    bool differentClasses = false;

    // if all the assets are the same type, we can use their specific builder options.  Otherwise, use the base builder options
    for ( Nocturnal::S_Path::const_iterator itr = assets.begin(), end = assets.end(); itr != end; ++itr )
    {
        const Nocturnal::Path& assetPath = (*itr);
        Asset::AssetClassPtr assetClass = Asset::AssetClass::LoadAssetClass( assetPath );

        Asset::AssetType currentType = assetClass->GetAssetType();

        if ( assetType != Asset::AssetTypes::Null && currentType != assetType )
        {
            differentClasses = true;
            assetType = Asset::AssetTypes::Null;
        }

        if ( !differentClasses )
        {
            assetType = currentType;
        }
    }

    AssetBuilder::BuilderOptionsPtr builderOptions = NULL;

    if ( differentClasses )
    {
        builderOptions = new AssetBuilder::BuilderOptions;
    }
    else
    {
        switch ( assetType )
        {
        case Asset::AssetTypes::AnimationSet:
            builderOptions = new AssetBuilder::AnimationBuilderOptions;
            break;

        case Asset::AssetTypes::Level:
            {
                builderOptions = new AssetBuilder::LevelBuilderOptions;
            }

            break;

        case Asset::AssetTypes::Shader:
            builderOptions = new AssetBuilder::ShaderBuilderOptions;
            break;

        case Asset::AssetTypes::Sky:
            builderOptions = new AssetBuilder::SkyBuilderOptions;
            break;

        case Asset::AssetTypes::Cinematic:
            builderOptions = new AssetBuilder::CinematicBuilderOptions;
            break;

        default:
            builderOptions = new AssetBuilder::BuilderOptions;
            break;
        }
    }

    return builderOptions;
}

bool GetBuilderOptions( const Nocturnal::S_Path& assets, AssetBuilder::BuilderOptionsPtr& builderOptions, wxWindow* parent )
{
    TaskOptionsDialog dialog( parent, wxID_ANY, "Builder/Packer Options" );

    Inspect::CanvasWindow* canvasWindow = new Inspect::CanvasWindow( dialog.GetPanel(), wxID_ANY, wxDefaultPosition, dialog.GetPanel()->GetSize(), wxALWAYS_SHOW_SB | wxCLIP_CHILDREN );

    Inspect::Canvas* canvas = new Inspect::Canvas;
    canvas->SetControl( canvasWindow );

    Inspect::ReflectInterpreterPtr interpreter = new Inspect::ReflectInterpreter( canvas );

    std::vector< Reflect::Element* > elems;

    if ( builderOptions.ReferencesObject() )
    {
        elems.push_back( builderOptions );

        interpreter->Interpret( elems );
    }

    canvas->Layout();
    canvas->Read();
    bool success = dialog.ShowModal() == wxID_OK;

    delete canvas;
    return success;
}

void Luna::BuildAssets( const Nocturnal::S_Path& assets, wxWindow* parent, AssetBuilder::BuilderOptionsPtr builderOptions, bool showOptions, bool blocking )
{
    if ( g_BuildInProgress )
    {
        wxMessageBox( "Another build is already in progress!", "Build in Progress", wxICON_WARNING | wxOK );
        return;
    }

    if ( g_BuildThread )
    {
        CloseHandle( g_BuildThread );
        g_BuildThread = NULL;
    }

    g_BuildInProgress = true;

    if ( !builderOptions.ReferencesObject() )
    {
        builderOptions = CreateBuilderOptions( assets );
    }

    bool success = true;

    if ( success && ( !showOptions || GetBuilderOptions( assets, builderOptions, parent ) ) )
    {
        if ( !g_OutputWindow && !blocking )
        {
            g_OutputWindow = new TaskOutputWindow( NULL, "Builder Output", 100, 100, 700, 500 );
            g_OutputWindow->AddSignalListener( SignalSignature::Delegate ( &BuildSignal ) );
            g_OutputWindow->CentreOnScreen();
        }

        RaiseTaskStarted( true );

        if ( g_OutputWindow )
        {
            g_OutputWindow->Show();
            g_OutputWindow->Raise();
        }

        BuildParams* params = new BuildParams;
        params->m_Assets = assets;
        params->m_BuilderOptions = builderOptions;

        g_BuildThread = CreateThread( NULL, 0, BuildThread, (LPVOID)params, 0, NULL );

        if (blocking)
        {
            WaitForSingleObject( g_BuildThread, INFINITE );
            CloseHandle( g_BuildThread );
            g_BuildThread = NULL;
        }
    }
    else
    {
        g_BuildInProgress = false;
    }
}

void Luna::BuildAsset( const Nocturnal::Path& asset, wxWindow* parent, AssetBuilder::BuilderOptionsPtr builderOptions, bool showOptions, bool blocking )
{
    Nocturnal::S_Path assets;
    assets.insert( asset );
    BuildAssets( assets, parent, builderOptions, showOptions, blocking );
}
