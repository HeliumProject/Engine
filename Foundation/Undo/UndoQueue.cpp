#include "FoundationPch.h"
#include "UndoQueue.h"

#include "Platform/Assert.h"
#include "Foundation/Log.h"
#include "Foundation/Exception.h"

#ifdef _DEBUG
//#define DEBUG_UNDO
#endif

using namespace Helium;

UndoQueue::UndoQueue()
: m_MaxLength (0)
{
    Reset();
}

UndoQueue::~UndoQueue()
{
    m_Destroyed.Raise( UndoQueueChangeArgs( this, NULL ) );
}

void UndoQueue::Reset()
{
    m_Undo.clear();
    m_Redo.clear();
    m_Active = false;
    m_BatchState = 0;
    m_Reset.Raise( UndoQueueChangeArgs( this, NULL ) );
}

void UndoQueue::Print() const
{
    Log::Print( TXT( "Max: %d\tUndo Length:\t%d\tRedo Length:\t%d\n" ), GetMaxLength(), m_Undo.size(), m_Redo.size() );
}

bool UndoQueue::IsActive() const
{
    return m_Active;
}

int UndoQueue::GetLength() const
{
    return static_cast< int >( m_Undo.size() + m_Redo.size() );
}

int UndoQueue::GetMaxLength() const
{
    return m_MaxLength;
}

void UndoQueue::SetMaxLength( int value )
{
    m_MaxLength = value;
}

bool UndoQueue::IsBatching() const
{
    return m_BatchState > 0;
}

void UndoQueue::BeginBatch()
{
    if ( ++m_BatchState == 1 )
    {
        m_Batch = new BatchCommand();
    }
}

void UndoQueue::EndBatch()
{
    if ( --m_BatchState == 0 && !m_Batch->IsEmpty() )
    {
        Push( m_Batch );
    }

    m_Batch = NULL;
}

void UndoQueue::Push( const UndoCommandPtr& c )
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
    m_UndoCommandPushed.Raise( UndoQueueChangeArgs( this, c ) );

#ifdef DEBUG_UNDO
    Print();
#endif
}

bool UndoQueue::CanUndo() const
{
    return m_Undo.size() > 0;
}

bool UndoQueue::CanRedo() const
{
    return m_Redo.size() > 0;
}

void UndoQueue::Undo()
{
    m_Active = true;

    // if the undo stack is not empty
    if ( m_Undo.size() > 0 )
    {
        UndoQueueChangingArgs args ( this, m_Undo.back().Ptr() );
        m_Undoing.Raise( args );
        if ( !args.m_Veto )
        {
            // get the command at the current position
            UndoCommandPtr c = m_Undo.back();
            m_Undo.pop_back();

            try
            {
                // undo command
                c->Undo();

                // Put command in the redo list.  If Undo throws an exception, the command will
                // not make it into the redo queue and the smart pointer will cause it to be
                // deleted.
                m_Redo.push_back( c );

                m_Undone.Raise( UndoQueueChangeArgs( this, c.Ptr() ) );
            }
            catch ( const Exception& e )
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

void UndoQueue::Redo()
{
    m_Active = true;

    // if the redo staick is not empty
    if ( m_Redo.size() > 0 )
    {
        UndoQueueChangingArgs args ( this, m_Redo.back().Ptr() );
        m_Redoing.Raise( args );
        if ( !args.m_Veto )
        {
            // get the command at the next position
            UndoCommandPtr c = m_Redo.back();
            m_Redo.pop_back();

            try
            {
                // redo it
                c->Redo();

                // Put command in the undo list.  If Redo throws an exception, the command will
                // not make it into the redo queue and the smart pointer will cause it to be
                // deleted.
                m_Undo.push_back( c );

                m_Redone.Raise( UndoQueueChangeArgs( this, c.Ptr() ) );
            }
            catch ( const Exception& e )
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
