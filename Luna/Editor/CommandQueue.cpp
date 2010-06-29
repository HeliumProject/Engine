#include "Precompile.h"
#include "CommandQueue.h"

#include "Platform/Mutex.h"

using namespace Luna;

DEFINE_EVENT_TYPE(wxEVT_FLUSH_COMMANDS)

static Platform::Mutex g_CommandQueueMutex;

CommandQueue::CommandQueue( wxWindow* window )
: m_Window( window )
{
  m_Window->Connect( m_Window->GetId(), wxEVT_FLUSH_COMMANDS, wxCommandEventHandler(CommandQueue::HandleEvent), NULL, this );
}

CommandQueue::~CommandQueue()
{
  m_Window->Disconnect( m_Window->GetId(), wxEVT_FLUSH_COMMANDS, wxCommandEventHandler(CommandQueue::HandleEvent), NULL, this );
}

void CommandQueue::HandleEvent( wxCommandEvent& event )
{
  Flush();
}

void CommandQueue::Push( const Undo::CommandPtr& command )
{
  bool flush = m_Commands.empty();

  {
    Platform::TakeMutex taken ( g_CommandQueueMutex);
    m_Commands.push_back( command );
  }

  if ( flush )
  {
    m_Window->AddPendingEvent( wxCommandEvent ( wxEVT_FLUSH_COMMANDS, m_Window->GetId() ) );
  }
}

void CommandQueue::Flush()
{
  Undo::V_CommandSmartPtr commands;

  {
    Platform::TakeMutex taken ( g_CommandQueueMutex);
    
    commands = m_Commands;
    m_Commands.clear();
  }
  
  Undo::V_CommandSmartPtr::const_iterator itr = commands.begin();
  Undo::V_CommandSmartPtr::const_iterator end = commands.end();
  for ( ; itr != end; ++itr )
  {
    // perform the work
    (*itr)->Redo();
  }

  // allow event handlers to push these commands on which ever undo queue they please
  m_PushCommand.Raise( commands );
}
