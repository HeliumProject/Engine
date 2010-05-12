#include "Precompile.h"
#include "SharedFileManager.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor - PRIVATE - Use GetInstance.
// 
SharedFileManager::SharedFileManager()
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SharedFileManager::~SharedFileManager()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the one and only instance of this class.
// 
SharedFileManager* SharedFileManager::GetInstance()
{
  static SharedFileManager theSharedFiles;
  return &theSharedFiles;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a listener for a file event on a specific file.
// 
void SharedFileManager::AddFileListener( const tuid& fileID, const SharedFileChangedSignature::Delegate& listener )
{
  if ( fileID != TUID::Null )
  {
    SharedFileChangedSignature::Event& evt = m_Events.insert( M_SharedFileChangedEvent::value_type( fileID, SharedFileChangedSignature::Event() ) ).first->second;
    evt.Add( listener );
  }
  else
  {
    // Why are you listening for events on a non-existant file?
    NOC_BREAK();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified listener from the list.
// 
void SharedFileManager::RemoveFileListener( const tuid& fileID, const SharedFileChangedSignature::Delegate& listener )
{
  M_SharedFileChangedEvent::iterator found = m_Events.find( fileID );
  if ( found != m_Events.end() )
  {
    SharedFileChangedSignature::Event& evt = found->second;
    evt.Remove( listener );
    if ( evt.Count() == 0 )
    {
      m_Events.erase( found );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified listener from all the files.
// 
void SharedFileManager::RemoveListenerFromAllFiles( const SharedFileChangedSignature::Delegate& listener )
{
  M_SharedFileChangedEvent::iterator itr = m_Events.begin();
  M_SharedFileChangedEvent::iterator end = m_Events.end();
  while ( itr != end )
  {
    SharedFileChangedSignature::Event& evt = itr->second;
    evt.Remove( listener );
    if ( evt.Count() == 0 )
    {
      itr = m_Events.erase( itr );
    }
    else
    {
      ++itr;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Raises an event on the specified file.
// 
void SharedFileManager::FireEvent( const tuid& fileID, FileActions::FileAction action, const SharedFileChangedSignature::Delegate& emitter )
{
  M_SharedFileChangedEvent::iterator found = m_Events.find( fileID );
  if ( found != m_Events.end() )
  {
    SharedFileChangedSignature::Event& evt = found->second;
    evt.Raise( SharedFileChangeArgs( fileID, action ), emitter );
  }
}
