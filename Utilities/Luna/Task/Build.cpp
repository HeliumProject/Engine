#include "Precompile.h"

#include <algorithm>
#include <strstream>

#include "Build.h"
#include "TaskOutputWindow.h"
#include "TaskOptionsDialog.h"
#include "ZoneSelectorDialog.h"
#include "RegionSelectorDialog.h"

#include "Inspect/InspectInit.h"
#include "InspectReflect/InspectReflectInit.h"
#include "InspectReflect/ReflectInterpreter.h"

#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "File/Manager.h"

#include "Dependencies/Dependencies.h"

#include "Asset/AssetClass.h"
#include "Asset/LevelAsset.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/WorldFileAttribute.h"
#include "AssetBuilder/AssetBuilder.h"
#include "AssetBuilder/BuilderOptions.h"

#include "Content/Zone.h"
#include "Reflect/Archive.h"

#include "AppUtils/AppUtils.h"
#include "Worker/Process.h"
#include "IPC/Connection.h"
#include "Common/InitializerStack.h"
#include "Console/Console.h"
#include "Debug/Exception.h"

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
    g_InitializerStack.Push( Dependencies::Initialize, Dependencies::Cleanup );
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

AssetBuilder::AssetBuiltSignature::Event g_AssetBuiltEvent;
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
  S_tuid m_AssetIds;
  tuid m_ViewAsset;
  AssetBuilder::BuilderOptionsPtr m_BuilderOptions;

  BuildParams()
    : m_BuilderOptions( NULL )
  {

  }
};

bool BuildEntry( const S_tuid& assetIDs, AssetBuilder::BuilderOptionsPtr options, bool view )
{
  g_WorkerProcess = Worker::Process::Create( "BuildTool.exe" );

  if (!g_WorkerProcess->Start())
  {
    return false;
  }

  AssetBuilder::BuildRequestPtr req = new AssetBuilder::BuildRequest ();
  req->m_Assets = assetIDs;
  req->m_Options = options;
  req->m_View = view;

  std::stringstream stream;
  
  try
  {
    Reflect::Archive::ToStream( req, stream, Reflect::ArchiveTypes::Binary );
  }
  catch ( Nocturnal::Exception& ex )
  {
    Console::Error( "%s\n", ex.what() );
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

      Console::Statement statement ( output->m_String, output->m_Stream, output->m_Level, output->m_Indent );

      if (g_OutputWindow)
      {
        g_OutputWindow->PrintListener( Console::PrintedArgs ( statement ) );
      }
      else
      {
        Console::PrintStatement( statement );
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

  S_tuid assets;

  for each ( const tuid assetId in params->m_AssetIds )
  {
    Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( assetId, false );

    if ( assetClass->GetEngineType() == Asset::EngineTypes::Level )
    {
      if ( !params->m_BuilderOptions.ReferencesObject() )
      {
        params->m_BuilderOptions = new AssetBuilder::LevelBuilderOptions;
      }

      if ( params->m_ViewAsset != TUID::Null )
      {
        AssetBuilder::LevelBuilderOptionsPtr options = Reflect::ObjectCast<AssetBuilder::LevelBuilderOptions>( params->m_BuilderOptions );

        options->m_Viewer = true;
      }

      assets.insert( assetId );
    }
    else if ( assetClass->GetEngineType() == Asset::EngineTypes::Cinematic )
    {
      if ( !params->m_BuilderOptions.ReferencesObject() )
      {
        params->m_BuilderOptions = new AssetBuilder::LevelBuilderOptions;
      }

      assets.insert( assetId );
    }
    else
    {
      assets.insert( assetId );
    }
  }

  success = BuildEntry( assets, params->m_BuilderOptions, false );

  if ( !g_BuildCancelling )
  {
    // notify of successful finish
    BuildFinishedArgs args;
    args.m_Result = success ? TaskResults::Success : TaskResults::Failure;
    args.m_Assets = params->m_AssetIds;
    args.m_View = params->m_ViewAsset != TUID::Null;
    args.m_BuilderOptions = params->m_BuilderOptions;
    RaiseTaskFinished( args );
  }

  g_BuildInProgress = false;

  delete params;

  return success ? 0 : 1;
}

AssetBuilder::BuilderOptionsPtr CreateBuilderOptions( const S_tuid& assetIds )
{
  Asset::EngineType engineType = Asset::EngineTypes::Null;
  bool differentClasses = false;

  // if all the assets are the same type, we can use their specific builder options.  Otherwise, use the base builder options
  for each ( const tuid assetId in assetIds )
  {
    Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( assetId, false );

    Asset::EngineType currentType = assetClass->GetEngineType();

    if ( engineType != Asset::EngineTypes::Null && currentType != engineType )
    {
      differentClasses = true;
      engineType = Asset::EngineTypes::Null;
    }

    if ( !differentClasses )
    {
      engineType = currentType;
    }
  }

  AssetBuilder::BuilderOptionsPtr builderOptions = NULL;

  if ( differentClasses )
  {
    builderOptions = new AssetBuilder::BuilderOptions;
  }
  else
  {
    switch ( engineType )
    {
    case Asset::EngineTypes::AnimationSet:
      builderOptions = new AssetBuilder::AnimationBuilderOptions;
      break;

    case Asset::EngineTypes::Tie:
      builderOptions = new AssetBuilder::TieBuilderOptions;
      break;

    case Asset::EngineTypes::Moby:
      builderOptions = new AssetBuilder::MobyBuilderOptions;
      break;

    case Asset::EngineTypes::Level:
      {
        builderOptions = new AssetBuilder::LevelBuilderOptions;
      }

      break;

    case Asset::EngineTypes::Shrub:
      builderOptions = new AssetBuilder::ShrubBuilderOptions;
      break;

    case Asset::EngineTypes::Shader:
      builderOptions = new AssetBuilder::ShaderBuilderOptions;
      break;

    case Asset::EngineTypes::Foliage:
      builderOptions = new AssetBuilder::FoliageBuilderOptions;
      break;

    case Asset::EngineTypes::Sky:
      builderOptions = new AssetBuilder::SkyBuilderOptions;
      break;

    case Asset::EngineTypes::Cinematic:
      builderOptions = new AssetBuilder::CinematicBuilderOptions;
      break;

    case Asset::EngineTypes::Ufrag:
      builderOptions = new AssetBuilder::UfragBuilderOptions;
      break;

    default:
      builderOptions = new AssetBuilder::BuilderOptions;
      break;
    }
  }

  return builderOptions;
}

bool GetBuilderOptions( const S_tuid& assetIds, AssetBuilder::BuilderOptionsPtr& builderOptions, wxWindow* parent )
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

  // if we're a level, check to see if the zones flag has been set.  if so, pop up the zone selector dialog
  AssetBuilder::LevelBuilderOptions* levelBuildOptions = Reflect::ObjectCast< LevelBuilderOptions >( builderOptions );
  if ( success && levelBuildOptions )
  {
    if ( levelBuildOptions->m_SelectZones )
    {
      V_string zoneNames;

      // fill out the zone names from the level asset
      Asset::LevelAssetPtr levelAsset = Asset::AssetClass::GetAssetClass<Asset::LevelAsset>( *assetIds.begin() );
      Attribute::AttributeViewer< Asset::WorldFileAttribute > model( levelAsset );

      Reflect::V_Element elements;     
      try
      {
        Reflect::Archive::FromFile( model->GetFilePath(), elements );
      }
      catch ( Nocturnal::Exception& ex )
      {
        std::ostringstream str;
        str << "Unable to load world from: " << model->GetFilePath() << ": " << ex.what();
        wxMessageBox( str.str(), "Error", wxICON_ERROR | wxOK );
        success = false;
      }

      if ( success )
      {
        V_string zoneNames;

        Reflect::V_Element::iterator itr = elements.begin();
        Reflect::V_Element::iterator end= elements.end();
        for( ; itr != end; ++itr )
        {
          Content::ZonePtr zone = Reflect::ObjectCast< Content::Zone >( (*itr) );
          if ( zone )
          {
            if ( zone->m_Active )
            {
              zoneNames.push_back( zone->GetName() );
            }
          }
        }

        std::sort( zoneNames.begin(), zoneNames.end() );

        // pop up the zone selector dialog
        S_u32 selectedZones;
        ZoneSelectorDialog zoneSelector( parent, "Select Zones", "", zoneNames, selectedZones );

        if ( zoneSelector.ShowModal() != wxID_CANCEL )
        {
          for each ( u32 zoneNum in selectedZones )
          {
            levelBuildOptions->m_ZoneList.push_back( zoneNames[ zoneNum ] );
          }
        }
        else  // dialog was canceled
        {
          success = false;
        }
      }
    }
  }

  return success;
}

void Luna::BuildAssets( const S_tuid& assetIds, wxWindow* parent, AssetBuilder::BuilderOptionsPtr builderOptions, bool showOptions, bool blocking, const tuid& viewAsset )
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
    builderOptions = CreateBuilderOptions( assetIds );
  }

  bool success = true;
  if ( viewAsset != TUID::Null )
  {
    if ( assetIds.size() == 1 )
    {
      LevelBuilderOptionsPtr levelBuilderOptions = Reflect::ObjectCast< LevelBuilderOptions >( builderOptions );
      if ( levelBuilderOptions.ReferencesObject() )
      {
        // fill out the region names from the level asset
        Asset::LevelAssetPtr levelAsset = Asset::AssetClass::GetAssetClass<Asset::LevelAsset>( *assetIds.begin() );
        Attribute::AttributeViewer< Asset::WorldFileAttribute > model( levelAsset );

        Reflect::V_Element elements;
        try
        {
          Reflect::Archive::FromFile( model->GetFilePath(), elements );
        }
        catch ( Nocturnal::Exception& ex )
        {
          std::ostringstream str;
          str << "Unable to load world from: " << model->GetFilePath() << ": " << ex.what();
          wxMessageBox( str.str(), "Error", wxICON_ERROR | wxOK );
          success = false;
        }

        if (success)
        {
          S_string regionNames;

#pragma TODO( "Remove default region as an all-zones region" )
          regionNames.insert( "default" );

          Reflect::V_Element::iterator itr = elements.begin();
          Reflect::V_Element::iterator end= elements.end();
          for( ; itr != end; ++itr )
          {
            Content::ZonePtr zone = Reflect::ObjectCast< Content::Zone >( (*itr) );

            if ( zone )
            {
              if ( zone->m_Active )
              {
                if ( zone->m_Regions.empty() )
                {
                  zone->m_Regions.push_back( "default" );
                }

                for each ( const std::string& region in zone->m_Regions )
                {
                  std::string name = region;
                  if ( name.empty() )
                  {
                    name = "default";
                  }

                  regionNames.insert( region );
                }
              }
            }
          }

          if ( regionNames.empty() )
          {
            regionNames.insert( "default" );
          }

          if ( regionNames.size() == 1 )
          {
            levelBuilderOptions->m_Regions.push_back( *regionNames.begin() );
          }
          else
          {
            // pop up the region selector dialog
            std::string selectedRegion;
            RegionSelectorDialog regionSelector( parent, "Select Region", "", regionNames, selectedRegion );

            if ( regionSelector.ShowModal() != wxID_CANCEL )
            {
              levelBuilderOptions->m_Regions.push_back( selectedRegion );
            }
            else  // dialog was canceled
            {
              success = false;
            }
          }
          
        }
      }
    }
  }

  if ( success && ( !showOptions || GetBuilderOptions( assetIds, builderOptions, parent ) ) )
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
    params->m_AssetIds = assetIds;
    params->m_BuilderOptions = builderOptions;
    params->m_ViewAsset = viewAsset;

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

void Luna::BuildAsset( const tuid& assetId, wxWindow* parent, AssetBuilder::BuilderOptionsPtr builderOptions, bool showOptions, bool blocking, const tuid& viewAsset )
{
  S_tuid tuids;
  tuids.insert( assetId );
  BuildAssets( tuids, parent, builderOptions, showOptions, blocking, viewAsset );
}

void Luna::ViewAsset( const tuid& assetId, wxWindow* parent, AssetBuilder::BuilderOptionsPtr builderOptions, bool showOptions, bool blocking )
{
  BuildAsset( assetId, parent, builderOptions, showOptions, blocking, assetId );
}