#include "Precompile.h"
#include "CommandQueue.h"

using namespace Helium;

CommandQueue::CommandQueue()
{

}

CommandQueue::~CommandQueue()
{

}

void CommandQueue::Post( VoidSignature::Delegate delegate )
{
    bool flush;

    {
        Helium::MutexScopeLock taken( m_Mutex );

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
        EnqueueFlush();
    }
}

void CommandQueue::Flush()
{
    std::vector< VoidSignature::Delegate > commands;

    {
        Helium::MutexScopeLock taken ( m_Mutex);
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

void CommandQueue::EnqueueFlush()
{
    // override this to manually defer the Flush() call (instead of calling it explicitly in an update loop or timer)
}