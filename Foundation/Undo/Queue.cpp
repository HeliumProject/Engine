#include "Queue.h"

#include "Exceptions.h"
#include "Platform/Assert.h"
#include "Foundation/Log.h"

#ifdef _DEBUG
//#define DEBUG_UNDO
#endif

using namespace Helium::Undo;

Queue::Queue()
: m_MaxLength (0)
{
    Reset();
}

Queue::~Queue()
{
    m_Destroyed.Raise( QueueChangeArgs( this, NULL ) );
}

void Queue::Reset()
{
    m_Undo.clear();
    m_Redo.clear();
    m_Active = false;
    m_BatchState = 0;
    m_Reset.Raise( QueueChangeArgs( this, NULL ) );
}

void Queue::Print() const
{
    Log::Print( TXT( "Max: %d\tUndo Length:\t%d\tRedo Length:\t%d\n" ), GetMaxLength(), m_Undo.size(), m_Redo.size() );
}

bool Queue::IsActive() const
{
    return m_Active;
}

int Queue::GetLength() const
{
    return static_cast< int >( m_Undo.size() + m_Redo.size() );
}

int Queue::GetMaxLength() const
{
    return m_MaxLength;
}

void Queue::SetMaxLength( int value )
{
    m_MaxLength = value;
}

bool Queue::IsBatching() const
{
    return m_BatchState > 0;
}

void Queue::BeginBatch()
{
    if ( ++m_BatchState == 1 )
    {
        m_Batch = new BatchCommand();
    }
}

void Queue::EndBatch()
{
    if ( --m_BatchState == 0 && !m_Batch->IsEmpty() )
    {
        Push( m_Batch );
    }

    m_Batch = NULL;
}

void Queue::Push( const CommandPtr& c )
{
    if ( m_Active || !c.ReferencesObject() )
    {
        return;
    }

    if ( m_BatchState > 0 )
    {
        m_Batch->Push( c );
        return;
    }

    // we should always have a command here
    HELIUM_ASSERT( c.ReferencesObject() );

    // we have a new command, so delete all subsequent commands from our current position
    m_Redo.clear();

    // if we have a finite length and we are full, remove the oldest command
    while ( m_MaxLength > 0 && GetLength() >= m_MaxLength )
    {
        m_Undo.erase( m_Undo.begin() );
    }

    // append our command to the queue
    m_Undo.push_back( c );

    // fire an event to interested listeners
    m_CommandPushed.Raise( QueueChangeArgs( this, c ) );

#ifdef DEBUG_UNDO
    Print();
#endif
}

bool Queue::CanUndo() const
{
    return m_Undo.size() > 0;
}

bool Queue::CanRedo() const
{
    return m_Redo.size() > 0;
}

void Queue::Undo()
{
    m_Active = true;

    // if the undo stack is not empty
    if ( m_Undo.size() > 0 )
    {
        QueueChangingArgs args ( this, m_Undo.back().Ptr() );
        m_Undoing.Raise( args );
        if ( !args.m_Veto )
        {
            // get the command at the current position
            CommandPtr c = m_Undo.back();
            m_Undo.pop_back();

            try
            {
                // undo command
                c->Undo();

                // Put command in the redo list.  If Undo throws an exception, the command will
                // not make it into the redo queue and the smart pointer will cause it to be
                // deleted.
                m_Redo.push_back( c );

                m_Undone.Raise( QueueChangeArgs( this, c.Ptr() ) );
            }
            catch ( const Undo::Exception& e )
            {
                Log::Warning( TXT( "Invalid undo command has been removed from the stack.\n" ) );
                Log::Warning( TXT( "%s\n" ), e.What() );
            }
        }
    }

    m_Active = false;

#ifdef DEBUG_UNDO
    Print();
#endif
}

void Queue::Redo()
{
    m_Active = true;

    // if the redo staick is not empty
    if ( m_Redo.size() > 0 )
    {
        QueueChangingArgs args ( this, m_Redo.back().Ptr() );
        m_Redoing.Raise( args );
        if ( !args.m_Veto )
        {
            // get the command at the next position
            CommandPtr c = m_Redo.back();
            m_Redo.pop_back();

            try
            {
                // redo it
                c->Redo();

                // Put command in the undo list.  If Redo throws an exception, the command will
                // not make it into the redo queue and the smart pointer will cause it to be
                // deleted.
                m_Undo.push_back( c );

                m_Redone.Raise( QueueChangeArgs( this, c.Ptr() ) );
            }
            catch ( const Undo::Exception& e )
            {
                Log::Warning( TXT( "Removing invalid command from undo stack.\n" ) );
                Log::Warning( TXT( "%s\n" ), e.What() );
            }
        }
    }

    m_Active = false;

#ifdef DEBUG_UNDO
    Print();
#endif
}
