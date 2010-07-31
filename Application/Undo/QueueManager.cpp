#include "QueueManager.h"
#include "Queue.h"

using namespace Helium::Undo;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
QueueManager::QueueManager()
: m_MaxLength( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
QueueManager::~QueueManager()
{
}

///////////////////////////////////////////////////////////////////////////////
// Clear the queue manager.
// 
void QueueManager::Reset()
{
  m_Undo.clear();
  m_Redo.clear();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the total number of items that can be redone or undone.
// 
size_t QueueManager::GetLength() const
{
  return m_Undo.size() + m_Redo.size();
}

///////////////////////////////////////////////////////////////////////////////
// Sets the maximum number of commands that can be undone or redone.  Set length
// to zero for infinite undo queue.
// 
void QueueManager::SetMaxLength( size_t length )
{
  m_MaxLength = length;

  Resize();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the maximum number of commands that can be held.
// 
size_t QueueManager::GetMaxLenth() const
{
  return m_MaxLength;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if there is a command available to be undone.
// 
bool QueueManager::CanUndo() const
{
  return m_Undo.size() > 0;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if there is a command available to be redone.
// 
bool QueueManager::CanRedo() const
{
  return m_Redo.size() > 0;
}

///////////////////////////////////////////////////////////////////////////////
// Pushes the specified queue into the undo stack and clears the redo stack.
// 
void QueueManager::Push( Queue* queue )
{
  queue->AddDestroyListener( QueueChangeSignature::Delegate ( this, &QueueManager::QueueDestroyed ) );
  m_Redo.clear();
  m_Undo.push_back( queue );
  Resize();
}

///////////////////////////////////////////////////////////////////////////////
// Undo the last command on the last queue that was pushed.
// 
void QueueManager::Undo()
{
  if ( CanUndo() )
  {
    Queue* queue = m_Undo.back();
    m_Undo.pop_back();
    queue->Undo();
    m_Redo.push_back( queue );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Redo the next command on the next queue in the list.
// 
void QueueManager::Redo()
{
  if ( CanRedo() )
  {
    Queue* queue = m_Redo.back();
    m_Redo.pop_back();
    queue->Redo();
    m_Undo.push_back( queue );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes undo and redo queues until the number of commands is less then the
// max length.
// 
void QueueManager::Resize()
{
  if ( m_MaxLength > 0 )
  {
    while ( GetLength() > m_MaxLength )
    {
      if ( m_Undo.size() > 0 )
      {
        m_Undo.pop_front();
      }
      else
      {
        m_Redo.pop_front();
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a queue is being destroyed.  Removes that queue from
// the undo and redo stacks.
// 
void QueueManager::QueueDestroyed( const QueueChangeArgs& args )
{
  D_QueueDumbPtr::iterator undoItr = m_Undo.begin();
  while ( undoItr != m_Undo.end() )
  {
    if ( *undoItr == args.m_Queue )
    {
      undoItr = m_Undo.erase( undoItr );
    }
    else
    {
      ++undoItr;
    }
  }

  D_QueueDumbPtr::iterator redoItr = m_Redo.begin();
  while ( redoItr != m_Redo.end() )
  {
    if ( *redoItr == args.m_Queue )
    {
      redoItr = m_Redo.erase( redoItr );
    }
    else
    {
      ++redoItr;
    }
  }
}
