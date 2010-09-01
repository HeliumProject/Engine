#include "Precompile.h"
#include "CommandQueue.h"

#include "Platform/Mutex.h"

using namespace Helium;
using namespace Helium::Editor;

DEFINE_EVENT_TYPE(wxEVT_FLUSH_COMMANDS)

static Helium::Mutex g_CommandQueueMutex;

CommandQueue::CommandQueue( wxWindow* window )
: m_Window( window )
{
    m_Window->Connect( m_Window->GetId(), wxEVT_FLUSH_COMMANDS, wxCommandEventHandler(CommandQueue::HandleEvent), NULL, this );
}

CommandQueue::~CommandQueue()
{
    m_Window->Disconnect( m_Window->GetId(), wxEVT_FLUSH_COMMANDS, wxCommandEventHandler(CommandQueue::HandleEvent), NULL, this );
}

void CommandQueue::Post( VoidSignature::Delegate delegate )
{
    bool flush;

    {
        Helium::TakeMutex taken( g_CommandQueueMutex );

        // the flash flag groups commands into a batch
        //  - the first push on an empty queue schedules a flush via message
        //  - subsequent pushes do not dispatch another message
        //  - ownership of the messages is taken in a lock, so the all the messages queued during a message pump will be flushed together
        //  - after the queue is emptied the next push will schedule the next message
        flush = m_Commands.empty();

        m_Commands.push_back( delegate );
    }

    if ( flush )
    {
        m_Window->GetEventHandler()->AddPendingEvent( wxCommandEvent ( wxEVT_FLUSH_COMMANDS, m_Window->GetId() ) );
    }
}

void CommandQueue::Flush()
{
    std::vector< VoidSignature::Delegate > commands;

    {
        Helium::TakeMutex taken ( g_CommandQueueMutex);
        commands = m_Commands;
        m_Commands.clear();
    }

    std::vector< VoidSignature::Delegate >::const_iterator itr = commands.begin();
    std::vector< VoidSignature::Delegate >::const_iterator end = commands.end();
    for ( ; itr != end; ++itr )
    {
        // perform the work
        (*itr).Invoke( Helium::Void () );
    }
}

void CommandQueue::HandleEvent( wxCommandEvent& event )
{
    Flush();
}