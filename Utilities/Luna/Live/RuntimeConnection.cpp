#include "Precompile.h"
#include "RuntimeConnection.h"
#include "TargetManager/TargetManager.h"
#include "LiveManager.h"

#include "Common/Exception.h"
#include "Common/CommandLine.h"

#include "IPC/TCP.h"
#include "IPC/Pipe.h"
#include "rpc/rpc.h"
#include "rpc/interfaces/rpc_luna_tool.h"
#include "rpc/interfaces/rpc_luna_host.h"
#include "rpc/interfaces/rpc_lunashader_host.h"
#include "rpc/interfaces/rpc_lunaview_host.h"
#include "rpc/interfaces/rpc_lunaanimationevents_host.h"
#include "rpc/interfaces/rpc_lunacinematicevents_host.h"

#include <wx/timer.h>

using namespace Luna;
using namespace RuntimeConnection;

class RuntimeConnectionTimer : public wxTimer
{
  void Notify()
  {
    RuntimeConnection::Process();
  }
};

class LunaTool : RPC::ILunaTool
{
public:

  LOCAL_IMPL(LunaTool, RPC::ILunaTool);

  virtual void SetViewerName( u8* data, u32 size )
  {
    m_ViewerName = (char*)data;
  }

  std::string m_ViewerName;
};

// real connection objects
IPC::TCPConnection g_TCPConnection;
IPC::PipeConnection g_PipeConnection;

// the ipc connection
IPC::Connection* g_Connection = NULL;

// rpc interfaces
RPC::Host* g_Host = NULL;
RPC::ILunaShaderHostRemote* g_RemoteShader = NULL;
RPC::ILunaViewHostRemote* g_RemoteLevel = NULL;
RPC::ILunaAnimationEventsHostRemote* g_RemoteEffects = NULL;
RPC::ILunaCinematicEventsHostRemote* g_RemoteCinematic = NULL;
LunaTool* g_LunaTool = NULL;

bool g_Connected = false;
RuntimeConnectionTimer* g_Timer = NULL;
static u32 g_InitRef = 0;

RuntimeConnectionStatusSignature::Event g_PS3Status;

static const char* s_PipeName = "Luna";
static const u32 s_Port = 31338;

void RuntimeConnection::Initialize()
{
  if( ++g_InitRef > 1 )
  {
    return;
  }

  if (Nocturnal::GetCmdLineFlag( "pipe" ))
  {
    g_PipeConnection.Initialize(true, "Viewer Data Connection", s_PipeName);
    g_Connection = &g_PipeConnection;
  }
  else
  {
    g_TCPConnection.Initialize(true, "Viewer Data Connection", "", s_Port);
    g_Connection = &g_TCPConnection;
  }

  g_Host = new RPC::Host ();
  g_Host->SetConnection(g_Connection);

  g_RemoteShader = new RPC::ILunaShaderHostRemote( g_Host );
  g_RemoteLevel = new RPC::ILunaViewHostRemote( g_Host );
  g_RemoteEffects = new RPC::ILunaAnimationEventsHostRemote( g_Host );
  g_RemoteCinematic = new RPC::ILunaCinematicEventsHostRemote( g_Host );

  g_LunaTool = new LunaTool( g_Host );
  g_Host->SetLocalInterface( RPC::kLunaTool, (RPC::ILunaTool*)g_LunaTool );


#ifdef _DEBUG
  //g_Host->SetTimeout(RPC::TIMEOUT_FOREVER);
#endif

  g_Timer = new RuntimeConnectionTimer();
  g_Timer->Start( 20 );
}

void RuntimeConnection::Cleanup()
{
  if (--g_InitRef == 0)
  {
    g_Connection->Cleanup();

    delete g_Timer;
    delete g_Host;
    delete g_RemoteShader;
    delete g_RemoteLevel;
    delete g_RemoteEffects;
    delete g_RemoteCinematic;
    delete g_LunaTool;

    g_Timer = NULL;
    g_Host = NULL;
    g_Connection = NULL;
    g_RemoteShader = NULL;
    g_RemoteLevel = NULL;
    g_RemoteEffects = NULL;
    g_RemoteCinematic = NULL;
    g_LunaTool = NULL;

    if ( g_Connected )
    {
      g_PS3Status.Raise( RuntimeConnectionStatusArgs( false ) );
    }

    g_Connected = false;
  }
}

void RuntimeConnection::Process()
{
  // processes events

  if ( !g_Connected && g_Host->Connected() )
  { 
    g_LunaTool->m_ViewerName.clear();
    g_PS3Status.Raise( RuntimeConnectionStatusArgs( true ) );
  }
  else if ( g_Connected && !g_Host->Connected() )
  {
    g_LunaTool->m_ViewerName.clear();
    g_PS3Status.Raise( RuntimeConnectionStatusArgs( false ) );
  }

  g_Connected = g_Host->Connected();

  g_Host->Dispatch();
}

RPC::Host* RuntimeConnection::GetHost()
{
  return g_Host;
}

RPC::ILunaShaderHostRemote* RuntimeConnection::GetRemoteShaderView()
{
  return g_RemoteShader;
}

RPC::ILunaViewHostRemote* RuntimeConnection::GetRemoteLevelView()
{
  return g_RemoteLevel;
}

RPC::ILunaAnimationEventsHostRemote* RuntimeConnection::GetRemoteAnimationEventsView()
{
  return g_RemoteEffects;
}

RPC::ILunaCinematicEventsHostRemote* RuntimeConnection::GetRemoteCinematicEventsView()
{
  return g_RemoteCinematic;
}

bool RuntimeConnection::IsConnected()
{
  return ( g_Host != NULL ) && g_Host->Connected();
}

bool RuntimeConnection::IsViewerConnected( const std::string& viewerName )
{
  if ( !IsConnected() )
    return false;

  try
  {
    TargetManager::Reset();

    const TargetManager::TargetPtr& target = Manager::GetInstance()->GetCurrentTarget();
    target->Connect();

    TargetManager::V_ProcessInfo processes;
    target->GetProcessInfo( processes );

    for each ( const TargetManager::ProcessInfo& info in processes )
    {
      if ( info.m_Path.find( viewerName ) != std::string::npos )
      {
        return true;
      }
    }
  }
  catch ( Nocturnal::Exception& e )
  {
    wxMessageBox( e.what(), "Error", wxOK|wxCENTRE|wxICON_ERROR, NULL );
  }

  return false;
}

void RuntimeConnection::AddRuntimeConnectionStatusListener( const RuntimeConnectionStatusSignature::Delegate& listener )
{
  g_PS3Status.Add( listener );
}

void RuntimeConnection::RemoveRuntimeConnectionStatusListener( const RuntimeConnectionStatusSignature::Delegate& listener )
{
  g_PS3Status.Remove( listener );
}

void RuntimeConnection::ResetConnection()
{
  g_Connection->Cleanup();

  if ( g_Connection == &g_PipeConnection )
  {
    g_PipeConnection.Initialize(true, "Viewer Data Connection", s_PipeName);
  }
  else
  {
    g_TCPConnection.Initialize(true, "Viewer Data Connection", "", s_Port);
  }
}