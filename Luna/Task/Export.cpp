#include "Precompile.h"
#include "Foundation/InitializerStack.h"

#include "Export.h"
#include "TaskOutputWindow.h"
#include "TaskOptionsDialog.h"

#include "Application/Inspect/InspectInit.h"
#include "Application/Inspect/Reflect/InspectReflectInit.h"
#include "Application/Inspect/Reflect/ReflectInterpreter.h"

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Classes/SceneAsset.h"
#include "Foundation/Component/ComponentHandle.h"
#include "Pipeline/Asset/ExporterJob.h"

#include "Foundation/Reflect/Archive.h"

#include "Application/Application.h"
#include "Application/Worker/Process.h"

#include "Foundation/Log.h"
#include "Foundation/IPC/Connection.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Platform/Exception.h"

#include <strstream>

using namespace Luna;

Nocturnal::InitializerStack g_RegisteredTypes;

HANDLE g_ExportThread = NULL;
bool g_ExportInProgress = false;
bool g_ExportCancelling = false;

static Worker::Process* g_WorkerProcess = NULL;
static TaskOutputWindow* g_OutputWindow;

static i32                         g_InitCount = 0;
static Nocturnal::InitializerStack g_InitializerStack;

REFLECT_DEFINE_CLASS( ExportOptions );

void ExportOptions::EnumerateClass( Reflect::Compositor<ExportOptions>& comp )
{
  comp.GetComposite().m_UIName = TXT( "Exporter Options" );

  Reflect::Field* fieldRecursive = comp.AddField( &ExportOptions::m_Recursive, "m_Recursive" );
  fieldRecursive->m_UIName = TXT( "Export Nested Assets" );
  Reflect::Field* fieldStateTrackerFile = comp.AddField( &ExportOptions::m_StateTrackerFile, "m_StateTrackerFile" );
  Reflect::Field* fieldSetupLighting = comp.AddField( &ExportOptions::m_SetupLighting, "m_SetupLighting" );
  fieldStateTrackerFile->m_UIName = TXT( "State Tracker File" );
}


void Export::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    g_InitializerStack.Push( InspectReflect::Initialize, InspectReflect::Cleanup );

    g_RegisteredTypes.Push( Reflect::RegisterClass<ExportOptions>( TXT( "ExportOptions" ) ) );
  }
}

void Export::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    // non-blocking builds dangle thier thread handles
    if ( g_ExportThread )
    {
      CloseHandle( g_ExportThread );
      g_ExportThread = NULL;
    }
    g_RegisteredTypes.Cleanup();
    g_InitializerStack.Cleanup();
  }
}

void ExportSignal(Nocturnal::Void)
{
  if (g_ExportInProgress)
  {
    // halt our build thread
    g_ExportCancelling = true;

    // murder the actual build process
    g_WorkerProcess->Kill();

    // release the process object (handle)
    Worker::Process::Release( g_WorkerProcess );

    // wait for it to complete
    WaitForSingleObject( g_ExportThread, INFINITE );

    // release the thread handle
    CloseHandle( g_ExportThread );
    g_ExportThread = NULL;

    // reset cancel state
    g_ExportCancelling = false;

    // raise finished event
    TaskFinishedArgs args;
    args.m_Result = TaskResults::Cancel;
    RaiseTaskFinished( args );

    // flag as dormant
    g_ExportInProgress = false;
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

bool Export( const S_tuid& assetIDs, bool recurse, const tstring& stateTrackerFile, bool lightingSetup = false )
{
  g_WorkerProcess = Worker::Process::Create( TXT( "ExportTool.exe" ) );

  if (!g_WorkerProcess->Start( 20000 ) )
  {
    return false;
  }

  Asset::ExporterJobPtr job = new Asset::ExporterJob();

  job->m_Recursive = recurse;
  job->m_StateTrackerFile = stateTrackerFile;
  job->m_SetupLighting = lightingSetup;

  S_tuid::const_iterator itr = assetIDs.begin();
  S_tuid::const_iterator end = assetIDs.end();
  for ( ; itr != end; ++itr )
  {
    job->m_AssetIds.insert( *itr );
  }

  std::stringstream stream;
  try
  {
    Reflect::ArchiveBinary::ToStream( job, stream );
  }
  catch ( Nocturnal::Exception& ex )
  {
    Log::Error( TXT( "%s\n" ), ex.What() );
    return false;
  }

  if (!g_WorkerProcess->Send(0x0, (u32)stream.str().length(), (const u8*)stream.str().c_str()))
  {
    return false;
  }

  while (g_WorkerProcess->Running() && !g_ExportCancelling)
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

      Log::Statement statement( output->m_String, output->m_Stream, output->m_Level, output->m_Indent );

      if (g_OutputWindow)
      {
        g_OutputWindow->PrintListener( Log::PrintedArgs ( statement ) );
      }
      else
      {
        Log::PrintStatement( statement );
      }
    }

    delete msg;
  }

  if (g_ExportCancelling)
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

DWORD WINAPI ExportThread( LPVOID lpParam )
{
  bool success = true;

  ExportParams* params = static_cast< ExportParams* >( lpParam );

  success = ::Export( params->m_AssetIds, params->m_Recursive, params->m_StateTrackerFile, params->m_SetupLighting );

  if ( !g_ExportCancelling )
  {
    // notify of successful finish
    TaskFinishedArgs args;
    args.m_Result = success ? TaskResults::Success : TaskResults::Failure;
    RaiseTaskFinished( args );
  }

  g_ExportInProgress = false;

  delete params;

  return success ? 0 : 1;
}

void Luna::ExportAssets( const S_tuid& assetIds, wxWindow* parent, bool showOptions, bool blocking, bool lightingSetup )
{
  if ( g_ExportInProgress )
  {
    wxMessageBox( wxT( "Another build is already in progress!" ), wxT( "Export in Progress" ), wxICON_WARNING | wxOK );
    return;
  }

  if ( g_ExportThread )
  {
    CloseHandle( g_ExportThread );
    g_ExportThread = NULL;
  }

  ExportOptionsPtr exportOptions = new ExportOptions();

  if ( showOptions )
  {
    TaskOptionsDialog dialog( parent, wxID_ANY, TXT( "Export Options" ) );

    Inspect::CanvasWindow* canvasWindow = new Inspect::CanvasWindow( dialog.GetPanel(), wxID_ANY, wxDefaultPosition, dialog.GetPanel()->GetSize(), wxALWAYS_SHOW_SB | wxCLIP_CHILDREN );

    Inspect::Canvas* canvas = new Inspect::Canvas;
    canvas->SetControl( canvasWindow );

    Inspect::ReflectInterpreterPtr interpreter = new Inspect::ReflectInterpreter( canvas );

    std::vector< Reflect::Element* > elems;
    elems.push_back( exportOptions );

    interpreter->Interpret( elems );

    canvas->Layout();
    if ( dialog.ShowModal() != wxID_OK )
    {
      delete canvas;
      return;
    }
  }

  g_ExportInProgress = true;

  ExportParams* params = new ExportParams();

  params->m_AssetIds = assetIds;
  params->m_Recursive = exportOptions->m_Recursive;
  params->m_StateTrackerFile = exportOptions->m_StateTrackerFile;
  params->m_SetupLighting = lightingSetup;

  if ( !g_OutputWindow && !blocking )
  {
    g_OutputWindow = new TaskOutputWindow( NULL, TXT( "Exporter Output" ), 100, 100, 700, 500 );
    g_OutputWindow->AddSignalListener( SignalSignature::Delegate ( &ExportSignal ) );
    g_OutputWindow->CentreOnScreen();
  }

  RaiseTaskStarted( true );

  if ( g_OutputWindow )
  {
    g_OutputWindow->Show();
    g_OutputWindow->Raise();
  }

  g_ExportThread = CreateThread( NULL, 0, ExportThread, (LPVOID)params, 0, NULL );

  if (blocking)
  {
    WaitForSingleObject( g_ExportThread, INFINITE );

    CloseHandle( g_ExportThread );
    g_ExportThread = NULL;
  }
}

void Luna::ExportAsset( const tuid& assetId, wxWindow* parent, bool showOptions, bool blocking )
{
  S_tuid tuids;
  tuids.insert( assetId );
  ExportAssets( tuids, parent, showOptions, blocking );
}
