#include "Precompile.h"

#include "LiveManager.h"
#include "LiveFrame.h"
#include "UIToolKit/RegistryConfig.h"

#include "Common/String/Utilities.h"
#include "Common/Environment.h"
#include "Common/CommandLine.h"
#include "Console/Console.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Windows/Error.h"
#include "Windows/Mail.h"
#include "Windows/Process.h"

#include <TlHelp32.h>
#include <sstream>
#include <fstream>

using namespace Luna;

#define REGISTRY_VERSION "1"

i32 g_InitCount = 0;

static char* g_AutoOpenFrameKey = "AutoOpenFrame";
static char* g_AutoStartViewerKey = "AutoStartViewer";
static char* g_ForceAutoStartViewerKey = "ForceAutoStartViewer";
static char* g_DefaultTargetKey = "DefaultTarget";

#define CRASH_IGNORE_TIME 10 // ignore crashes that happen within this number of seconds of a previous crash

enum LiveManagerEvents
{
  Event_ViewerStartThreadDone = wxID_HIGHEST + 1,
  Event_CrashInfoThreadDone,
};

Manager* Manager::s_Manager = NULL;

void Live::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    Manager::Initialize();

    if ( Manager::GetInstance()->GetAutoOpenFrame() )
    {
      OpenLiveFrame();
    }
  }
}

void Live::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    Manager::Cleanup();
  }
}

void Luna::OpenLiveFrame()
{
  Manager::GetInstance()->OpenLiveFrame();
}

void Luna::OnTargetEvent( u32 eventId, SNPS3_DBG_EVENT_HDR* dbgHeader, SNPS3_DBG_EVENT_DATA* dbgData, void* pUserData )
{
  TargetManager::Target* target = (TargetManager::Target*)pUserData;
  Manager::GetInstance()->OnTargetEvent( target, eventId, dbgHeader, dbgData );
}

void Manager::Initialize()
{
  NOC_ASSERT( s_Manager == NULL );
  s_Manager = new Manager ();
}

void Manager::Cleanup()
{
  NOC_ASSERT( s_Manager != NULL );
  delete s_Manager;
}

Manager* Manager::GetInstance()
{
  NOC_ASSERT( s_Manager );
  return s_Manager;
}

Manager::Manager()
: m_LiveFrame( NULL )
, m_StartViewerThread( NULL )
, m_CrashInfoThread( NULL )
, m_Crashed( false )
, m_ViewerMode( ViewerModes::Uberview )
{
  SetupDefaults();
  RefreshTargets();
  FindSelectedTarget();

  m_ProcessTimer.Start( 1000 );

  Connect( Event_ViewerStartThreadDone, wxCommandEventHandler( Manager::OnViewerStartThreadFinished ) );
  Connect( Event_CrashInfoThreadDone, wxCommandEventHandler( Manager::OnCrashInfoThreadFinished ) );
}

Manager::~Manager()
{
  m_ProcessTimer.Stop();

  Disconnect( Event_ViewerStartThreadDone, wxCommandEventHandler( Manager::OnViewerStartThreadFinished ) );
  Disconnect( Event_CrashInfoThreadDone, wxCommandEventHandler( Manager::OnCrashInfoThreadFinished ) );

  for each ( const TargetManager::TargetPtr& target in m_Targets )
  {
    target->RemoveTTYListener( TargetManager::TTYSignature::Delegate( this, &Manager::ProcessTTY ) );
  }

  if ( m_StartViewerThread )
  {
    m_StartViewerThread->Wait();
    delete m_StartViewerThread;
    m_StartViewerThread = NULL;
  }

  if ( m_CrashInfoThread )
  {
    m_CrashInfoThread->Wait();
    delete m_CrashInfoThread;
    m_CrashInfoThread = NULL;
  }

  if ( m_LiveFrame )
  {
    m_LiveFrame->Destroy();
  }
}

void Manager::SetupDefaults()
{
  std::string value;
  ReadRegistryValue( g_AutoOpenFrameKey, value );
  if ( value.empty() )
  {
    SetAutoOpenFrame( false );
  }

  ReadRegistryValue( g_AutoStartViewerKey, value );
  if ( value.empty() )
  {
    SetAutoStartViewer( true );
  }

  ReadRegistryValue( g_ForceAutoStartViewerKey, value );
  if ( value.empty() )
  {
    SetForceAutoStartViewer( false );
  }
}

void Manager::RefreshTargets()
{
  try
  {
    if ( m_CurrentTarget.ReferencesObject() )
    {
      m_CurrentTarget->RemoveTTYListener( TargetManager::TTYSignature::Delegate( this, &Manager::ProcessTTY ) );
    }

    m_Targets.clear();
    TargetManager::Target::GetTargets( m_Targets );
    TargetManager::ProcessEvents( 10 );

    FindSelectedTarget();

    if ( m_CurrentTarget.ReferencesObject() )
    {
      m_CurrentTarget->AddTTYListener( TargetManager::TTYSignature::Delegate( this, &Manager::ProcessTTY ) );
    }
  }
  catch( Nocturnal::Exception& e )
  {
    Console::Error( "%s\n", e.what() );
  }
}

const TargetManager::V_Target& Manager::GetTargets()
{
  return m_Targets;
}

const TargetManager::TargetPtr& Manager::GetCurrentTarget()
{
  return m_CurrentTarget;
}

void Manager::FindSelectedTarget()
{
  std::string targetName = GetDefaultTarget();

  u32 index = 0;
  bool found = false;
  for each ( const TargetManager::TargetPtr& target in m_Targets )
  {
    if ( targetName == target->GetName() )
    {
      found = true;
      break;
    }

    ++index;
  }

  if ( found )
  {
    m_CurrentTarget = m_Targets[ index ];
  }
  else if ( !m_Targets.empty() )
  {
    m_CurrentTarget = m_Targets[ 0 ];
  }
  else
  {
    m_CurrentTarget = NULL;
  }
}

void Manager::PerformAutoStart()
{
  try
  {
    TargetManager::Reset();

    RefreshTargets();

    if ( !m_Targets.empty() && m_CurrentTarget.ReferencesObject() )
    {
      if ( !m_CurrentTarget->IsConnected() )
      {
        m_CurrentTarget->ForceDisconnect();
        m_CurrentTarget->Connect();
      }

      if ( m_ViewerMode == ViewerModes::Uberview )
      {
        if ( RuntimeConnection::IsViewerConnected( "uberview" ) && !m_CurrentTarget->IsCrashed() )
        {
          return;
        }
      }
      if ( m_ViewerMode == ViewerModes::Game )
      {
        if ( RuntimeConnection::IsViewerConnected( "game" ) && !m_CurrentTarget->IsCrashed() )
        {
          return;
        }
        else
        {
          wxMessageBox("The game doesn't appear to be running.  The game must be running in order to connect Live to Luna.", "Error", wxOK | wxICON_EXCLAMATION);
          return;
        }
      }

      V_u32 processIds;
      m_CurrentTarget->GetProcessIds( processIds );

      if ( processIds.empty() || !m_CurrentTarget->IsOn() )
      {
        StartViewer();
      }
      else
      {
        if ( GetForceAutoStartViewer() )
        {
          StartViewer();
        }
        else if ( GetAutoStartViewer() )
        {
          if ( wxMessageBox( "A process is already running on your devkit.  Start uberview?", "Start Uberview?", wxYES_NO|wxICON_QUESTION, m_LiveFrame ) == wxYES )
          {
            StartViewer();
          }
        }
      }
    }
  }
  catch ( Nocturnal::Exception& e )
  {
    wxMessageBox( e.what(), "Error", wxOK|wxCENTRE|wxICON_ERROR, NULL );
  }
}

void Manager::SetViewerMode( ViewerMode viewerMode )
{
  m_ViewerMode = viewerMode;
}

ViewerMode Manager::GetViewerMode( )
{
  return m_ViewerMode;
}

void Manager::StartViewer()
{
  if ( !m_StartViewerThread && !m_Targets.empty() && m_CurrentTarget.ReferencesObject() )
  {
    NOC_ASSERT( m_StartViewerThread == NULL );

    m_StartViewerThread = new StartViewerThread( this, m_CurrentTarget, "uberview" );
    m_StartViewerThread->Create();
    m_StartViewerThread->Run();
  }
}

void Manager::OnViewerStartThreadFinished( wxCommandEvent& evt )
{
  m_StartViewerThread->Wait();

  if ( !m_StartViewerThread->Success() )
  {
    wxMessageBox( m_StartViewerThread->GetErrorString(), "Error", wxOK|wxCENTRE|wxICON_ERROR, m_LiveFrame );
  }

  delete m_StartViewerThread;
  m_StartViewerThread = NULL;

  // I hate the target manager api.  Because doing anything from another thread requires Reset to be
  // called, it invalidates all our registered event callbacks.
  TargetManager::Reset();
  for each ( const TargetManager::TargetPtr& target in m_Targets )
  {
    target->RegisterEventCallbacks(); 
  }
}

void Manager::OnCrashInfoThreadFinished( wxCommandEvent& evt )
{
  m_CrashInfoThread->Wait();

  SetLastBacktrace( m_CrashInfoThread->GetBacktrace() );

  delete m_CrashInfoThread;
  m_CrashInfoThread = NULL;
}

void Manager::SetAutoOpenFrame( bool enable )
{
  WriteRegistryValue( g_AutoOpenFrameKey, enable ? "1" : "0" );

  if ( m_LiveFrame )
  {
    m_LiveFrame->SetAutoOpenFrame( enable );
  }
}

void Manager::SetAutoStartViewer( bool enable )
{
  WriteRegistryValue( g_AutoStartViewerKey, enable ? "1" : "0" );

  if ( !enable )
  {
    SetForceAutoStartViewer( false );
  }

  if ( m_LiveFrame )
  {
    m_LiveFrame->SetAutoStartViewer( enable );
  }
}

void Manager::SetForceAutoStartViewer( bool enable )
{
  WriteRegistryValue( g_ForceAutoStartViewerKey, enable ? "1" : "0" );

  if ( enable )
  {
    SetAutoStartViewer( true );
  }

  if ( m_LiveFrame )
  {
    m_LiveFrame->SetForceAutoStartViewer( enable );
  }
}

void Manager::SetDefaultTarget( const std::string& target )
{
  WriteRegistryValue( g_DefaultTargetKey, target );

  FindSelectedTarget();
}

std::string Manager::GetDefaultTarget()
{
  std::string defaultTarget;
  ReadRegistryValue( g_DefaultTargetKey, defaultTarget );

  return defaultTarget;
}

bool Manager::GetForceAutoStartViewer()
{
  std::string val;
  ReadRegistryValue( g_ForceAutoStartViewerKey, val );

  if ( val == "0" )
    return false;

  return true;
}

bool Manager::GetAutoOpenFrame()
{
  std::string val;
  ReadRegistryValue( g_AutoOpenFrameKey, val );

  if ( val == "0" )
    return false;

  return true;
}

bool Manager::GetAutoStartViewer()
{
  std::string val;
  ReadRegistryValue( g_AutoStartViewerKey, val );

  if ( val == "0" )
    return false;

  return true;
}

void Manager::OpenLiveFrame()
{
  if ( !m_LiveFrame )
  {
    m_LiveFrame = new LiveFrame( NULL );
  }

  m_LiveFrame->Show();
  m_LiveFrame->Raise();

  if ( m_CurrentTarget.ReferencesObject() )
  {
    m_LiveFrame->SetLastBackTrace( m_LastBackTrace );
  }
}

void Manager::LiveFrameClosed()
{
  m_LiveFrame = NULL;
}

bool Manager::WriteRegistryValue( const std::string& key, const std::string& value )
{
  std::string realKey = std::string (REGISTRY_VERSION) + "/" + key;
  return UIToolKit::RegistryConfig::GetInstance()->Write( "LiveLink", realKey, value );
}

bool Manager::ReadRegistryValue( const std::string& key, std::string& value )
{
  std::string realKey = std::string (REGISTRY_VERSION) + "/" + key;
  return UIToolKit::RegistryConfig::GetInstance()->Read( "LiveLink", realKey, value );
}

void Manager::StartViewer( TargetManager::TargetPtr& target, const std::string& selfName, const V_string& extraArguments )
{
  // figure out our IP address
  std::string ipAddress;
  if ( !Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"LOCAL_IP_OVERRIDE", ipAddress ) )
  {
    char hostName[128];
    if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR) 
    {
      throw Nocturnal::Exception( "Could not lookup host name!" );
    }

    struct hostent *phe = gethostbyname( hostName );
    if ( phe->h_addr_list == NULL )
    {
      throw Nocturnal::Exception( "This computer does not have an IP address!" );
    }

    // assume we want the first one?  should make this an option too
    struct in_addr addr;
    memcpy( &addr, phe->h_addr_list[ 0 ], sizeof( struct in_addr ) );
    ipAddress = inet_ntoa( addr );
  }

  // figure out the SELF location
  std::string selfPath = Finder::ProjectTools( Finder::ToolsFolders::Bin ) + selfName;

  V_string options;
  options.insert( options.begin(), extraArguments.begin(), extraArguments.end() );

  if (Nocturnal::GetCmdLineFlag( "pipe" ))
  {
    options.push_back( "-pipe" );
    options.push_back( "Luna" );
  }
  else
  {
    options.push_back( "-ip" );
    options.push_back( ipAddress );
    options.push_back( "-port" );
    options.push_back( "31338" );
  }

  options.push_back( "-assetbranch" );
  options.push_back( Finder::ProjectAssetsBranch().substr( 0, Finder::ProjectAssetsBranch().length()-1 ) );

  // Make sure we're connected
  if ( !target->IsConnected() )
  {
    target->ForceDisconnect();
    target->Connect();
  }

  // Reset the target
  target->Reset();

  // Reconnect
  target->Connect();

  // Set the fileserver directory
  target->SetFileServerDir( Finder::ProjectRoot().c_str() );

  // Load the process
  if ( target->LoadProcess( selfPath.c_str(), options, false ) == (u32)-1 )
  {
    throw Nocturnal::Exception( "Failed to load process!" );
  }
}

bool Manager::ParseTTYForCrash( const std::string& tty )
{
  std::string lowercase = tty;
  toLower( lowercase );
  if ( lowercase.find( "interrupt(exception)" ) != std::string::npos )
  {
    return true;
  }

  if ( lowercase.find( "backtrace" ) != std::string::npos )
  {
    return true;
  }

  if ( lowercase.find( "assert failed" ) != std::string::npos )
  {
    return true;
  }



  return false;
}

void Manager::ProcessTTY( const TargetManager::TTYArgs& args )
{
  if ( m_LiveFrame && args.m_Target == m_CurrentTarget )
  {
    //m_LiveFrame->AppendTTY( args.m_Text );
  }

  if ( m_Crashed )
  {
    m_CrashLines++;
  }
  else if ( ParseTTYForCrash( args.m_Text ) )
  {
    m_Crashed = true;
    m_CrashLines = 1;
  }
}

void Manager::PostEventsProcessed()
{
  if ( m_Crashed )
  {
    u32 currentTime = time(NULL);
    if ( currentTime - m_LastCrashTime > CRASH_IGNORE_TIME )
    {
#pragma TODO( "somewhat expensive -- should move this to a periodical check and store" )
      // only send crash reports if the debugger is not running
      PROCESSENTRY32 pe;
      pe.dwSize = sizeof( pe );
      HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
      bool debugger = false;
      BOOL res = Process32First( hSnapshot, &pe );
      for (; res; res = Process32Next( hSnapshot, &pe ) )
      {
        if ( !stricmp( pe.szExeFile, "ps3debugger.exe" ) )
        {
          debugger = true;
          break;
        }
      }

      CloseHandle( hSnapshot );

      // ensure we have all of the TTY
      TargetManager::ProcessEvents( 10 );

      if ( !debugger )
      {
        Console::Print( "Sending PS3 crash report\n" );

        SendPS3DebugInfo();
      }
    }

    m_Crashed = false;
    m_LastCrashTime = currentTime;
  }
}

void Manager::OnTargetEvent( TargetManager::Target* target, u32 eventId, SNPS3_DBG_EVENT_HDR* dbgHeader, SNPS3_DBG_EVENT_DATA* dbgData )
{
  // in the future I would like to be able to handle crashes in here.  currently we check for a stopped
  // main PPU thread after every set of events has been processed
  // this is for two reasons:
  // 1) When a self is loaded with the debuggable flag set, the ps3 will not output a crash report
  //    to the TTY
  // 2) We don't get these events if the self was not loaded with the debuggable flag, so we would
  //    only ever get crash reports from uberview
}

std::string GetBacktrace( const D_string& tty, const std::string& elf, u32 crashLines )
{
#pragma TODO( "Use the target manager api?" )

  std::string traceFile = Finder::ProjectTemp() + "tty.txt";
  std::ofstream out;
  out.open( traceFile.c_str() );
  if ( !out.is_open() )
  {
    return "Could not open tty file for write";
  }

  crashLines = std::min( (u32)tty.size() - 1, crashLines );

  D_string::const_iterator it = tty.end() - crashLines;
  D_string::const_iterator end = tty.end();
  for (; it != end; ++it )
  {
    out << *it;
  }

  out.close();

  std::string outputFile = Finder::ProjectTemp() + "backtrace.txt";

  Windows::Execute( std::string ("perl.exe ") + Finder::ProjectTools() + "scripts/backtrace.pl " 
    + traceFile + " " + outputFile + " -e " + elf, false, true );

  std::ifstream in;
  in.open( outputFile.c_str() );
  if ( !in.is_open() )
  {
    return "Could not open backtrace file for read";
  }

  std::stringstream traceStream;
  while ( !in.eof() )
  {
    char buf[2048];
    in.getline( buf, 2048 );

    traceStream << buf << "\n";
  }

  in.close();

  DeleteFile( outputFile.c_str() );

  return traceStream.str();
}

void Manager::SendPS3DebugInfo()
{
  if ( !m_CrashInfoThread && !m_Targets.empty() && m_CurrentTarget.ReferencesObject() )
  {
    NOC_ASSERT( m_CrashInfoThread == NULL );

    TargetManager::V_ProcessInfo processes;
    m_CurrentTarget->GetProcessInfo( processes );
    std::string elf;
    if ( !processes.empty() )
    {
      elf = processes[ 0 ].m_Path;
    }

    m_CrashInfoThread = new SendCrashInfoThread( this, m_CurrentTarget->GetTTY(), elf, m_CurrentTarget->GetName(), m_CrashLines );
    m_CrashInfoThread->Create();
    m_CrashInfoThread->Run();
  }
}

void Manager::SetLastBacktrace( const std::string& backtrace )
{
  m_LastBackTrace = backtrace;

  if ( m_LiveFrame )
  {
    m_LiveFrame->SetLastBackTrace( m_LastBackTrace );
  }
}

StartViewerThread::StartViewerThread( wxEvtHandler* frame, const TargetManager::TargetPtr& target,
                                     const std::string& viewerName )
: wxThread( wxTHREAD_JOINABLE )
, m_EventHandler( frame )
, m_Target( target )
, m_ViewerName( viewerName )
{
  
}

wxThread::ExitCode StartViewerThread::Entry()
{
  V_string args;

  m_Success = true;

  try
  {
    TargetManager::Reset();
    Manager::GetInstance()->StartViewer( m_Target, m_ViewerName + ".self", args );
  }
  catch ( Nocturnal::Exception& e )
  {
    m_Success = false;
    m_ErrorString = e.what();
  }

  wxPostEvent( m_EventHandler, wxCommandEvent( Event_ViewerStartThreadDone ) );

  return 0;
}

void TargetManagerProcessTimer::Notify()
{
  TargetManager::ProcessEvents( 10 );
  Manager::GetInstance()->PostEventsProcessed();
}

wxThread::ExitCode SendCrashInfoThread::Entry()
{
  try
  {
    std::stringstream errorStream;

    std::string str;
    Nocturnal::GetEnvVar( "USERNAME", str );
    errorStream << "Username:\t" << str << std::endl;

    Nocturnal::GetEnvVar( "COMPUTERNAME", str );
    errorStream << "Computer:\t" << str << std::endl;

    errorStream << "SELF:\t" << m_Elf << std::endl;

    m_Backtrace = ::GetBacktrace( m_TTY, m_Elf, m_CrashLines );

    errorStream << m_Backtrace << std::endl;

    errorStream << "\nRecent TTY:\n\n";

    for each ( const std::string& line in m_TTY )
    {
      errorStream << line;
    }

    std::string selfName = m_Elf;
    FileSystem::CleanName( selfName );
    selfName = FileSystem::GetLeaf( selfName );

    Nocturnal::GetEnvVar( "COMPUTERNAME", str );
    std::string subject = "Crash in " + selfName + " @ " + str + " on " + m_TargetName;
    Windows::SendMail( subject, errorStream.str(), "", "auto_ps3_bugs@insomniacgames.com", "auto_ps3_bugs@insomniacgames.com" );

  }
  catch ( Nocturnal::Exception& e )
  {
    Console::Error( "Error in backtrace thread: %s\n", e.what() );
  }

  wxPostEvent( m_EventHandler, wxCommandEvent( Event_CrashInfoThreadDone ) );

  return 0;
}
