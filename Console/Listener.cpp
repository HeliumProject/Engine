#include "Listener.h"

#include "Windows/Windows.h"

using namespace Console;

Listener::Listener( u32 throttle, u32* errorCount, u32* warningCount, Console::V_Statement* consoleOutput )
: m_Thread( GetCurrentThreadId() )
, m_Throttle( throttle )
, m_WarningCount( warningCount )
, m_ErrorCount( errorCount )
, m_ConsoleOutput( consoleOutput )
{
  Start();
}

Listener::~Listener()
{
  Stop();
}

void Listener::Start()
{
  Console::AddPrintingListener( Console::PrintingSignature::Delegate( this, &Listener::Print ) );
}

void Listener::Stop()
{
  Console::RemovePrintingListener( Console::PrintingSignature::Delegate( this, &Listener::Print ) );
}

void Listener::Dump(bool stop)
{
  if ( stop )
  {
    Stop();
  }

  if ( m_ConsoleOutput )
  {
    Console::PrintStatements( *m_ConsoleOutput );
  }
}

u32 Listener::GetWarningCount()
{
  return *m_WarningCount;
}

u32 Listener::GetErrorCount()
{
  return *m_ErrorCount;
}

void Listener::Print( Console::PrintingArgs& args )
{
  if ( m_Thread == GetCurrentThreadId() )
  {
    if ( args.m_Statement.m_Stream == Console::Streams::Warning && m_WarningCount )
    {
      (*m_WarningCount)++;
    }

    if ( args.m_Statement.m_Stream == Console::Streams::Error && m_ErrorCount )
    {
      (*m_ErrorCount)++;
    }

    if ( m_ConsoleOutput )
    {
      m_ConsoleOutput->push_back( args.m_Statement );
    }

    if ( m_Throttle & args.m_Statement.m_Stream )
    {
      args.m_Skip = true;
    }
  }
}