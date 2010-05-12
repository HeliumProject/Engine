#include "Windows/Windows.h"
#include "TempResolver.h"

#include "Common/String/Utilities.h"
#include "Common/Environment.h"

#include <sys/timeb.h>

using namespace File;

/////////////////////////////////////////////////////////////////////////////
TempResolver::TempResolver()
{
  // Username
  DWORD nameLength = MAX_COMPUTERNAME_LENGTH + 1;
  char nameString[MAX_COMPUTERNAME_LENGTH + 1];

  if ( ::GetUserName( nameString, &nameLength ) )
  {
    m_Username = nameString;
  }
  if ( m_Username.empty() )
  {
    if( !Nocturnal::GetEnvVar( "USERNAME", m_Username ) )
    {
      m_Username = "unknown";
    }
  }
  toLower( m_Username );
}

/////////////////////////////////////////////////////////////////////////////
TempResolver::~TempResolver()
{
  ClearCache();
}

/////////////////////////////////////////////////////////////////////////////
void TempResolver::ClearCache()
{
  FILE_SCOPE_TIMER((""));

  m_TuidManagedFiles.clear();
  m_PathManagedFiles.clear();
}

/////////////////////////////////////////////////////////////////////////////
tuid TempResolver::AddEntry( const std::string &filePath, const tuid tryId )
{
  FILE_SCOPE_TIMER((""));

  tuid id = tryId;
  if ( id == TUID::Null )
  {
    id = TUID::Generate();
  }

  __timeb64 now;
  _ftime64_s( &now );
  u64 currentTime = ( now.time * 1000 ) + now.millitm;

  ManagedFilePtr newFile = new ManagedFile();
  newFile->m_Id       = id;
  newFile->m_Created  = currentTime;
  newFile->m_Modified = currentTime;
  newFile->m_Path     = filePath;
  newFile->m_Username = m_Username;

  InsertManagedFile( newFile );

  return newFile->m_Id;
}

/////////////////////////////////////////////////////////////////////////////
tuid TempResolver::AddEntry( ManagedFilePtr& file )
{
  FILE_SCOPE_TIMER((""));

  M_TuidManagedFile::iterator foundFile = m_TuidManagedFiles.find( file->m_Id );
  if ( foundFile != m_TuidManagedFiles.end() )
  {
    if ( foundFile->second->m_Path == file->m_Path )
    {
      // we already have this file cached, early out
      return file->m_Id;
    }
  }

  ManagedFilePtr newFile = new ManagedFile();
  file->CopyTo( newFile );

  InsertManagedFile( newFile );

  return newFile->m_Id;
}

/////////////////////////////////////////////////////////////////////////////
tuid TempResolver::UpdateEntry( const ManagedFilePtr& file, const std::string &newFilePath )
{
  FILE_SCOPE_TIMER((""));

  __timeb64 now;
  _ftime64_s( &now );
  u64 modifiedTime = ( now.time * 1000 ) + now.millitm;

  M_TuidManagedFile::iterator foundFileItr = m_TuidManagedFiles.find( file->m_Id );
  if ( foundFileItr != m_TuidManagedFiles.end() )
  {
    ManagedFilePtr foundFile = foundFileItr->second;

    // Update the path and modified time
    foundFile->m_Modified = modifiedTime;
    foundFile->m_Path = newFilePath;
  }
  else
  {
    // Create a new file
    ManagedFilePtr newFile = new ManagedFile();
    file->CopyTo( newFile );

    // Update the path and modified time
    newFile->m_Modified = modifiedTime;
    newFile->m_Path = newFilePath;

    // Add the new file
    InsertManagedFile( newFile ); 
  }

  return file->m_Id;
}

/////////////////////////////////////////////////////////////////////////////
void TempResolver::DeleteEntry( const tuid id )
{
  FILE_SCOPE_TIMER((""));

  ManagedFilePtr file = GetFile( id );

  if ( file )
  {
    m_TuidManagedFiles.erase( file->m_Id );
    m_PathManagedFiles.erase( file->m_Path );
  }
}

/////////////////////////////////////////////////////////////////////////////
ManagedFilePtr TempResolver::GetFile( const std::string& filePath )
{
  FILE_SCOPE_TIMER((""));

  ManagedFilePtr file = NULL;

  M_PathManagedFile::iterator foundFile = m_PathManagedFiles.find( filePath );
  if ( foundFile != m_PathManagedFiles.end() )
  {
    file = new ManagedFile();
    foundFile->second->CopyTo( file );
  }

  return file;
}

/////////////////////////////////////////////////////////////////////////////
ManagedFilePtr TempResolver::GetFile( const tuid id )
{
  FILE_SCOPE_TIMER((""));

  ManagedFilePtr file = NULL;

  M_TuidManagedFile::iterator foundFile = m_TuidManagedFiles.find( id );
  if ( foundFile != m_TuidManagedFiles.end() )
  {
    file = new ManagedFile();
    foundFile->second->CopyTo( file );
  }

  return file;
}

/////////////////////////////////////////////////////////////////////////////
tuid TempResolver::GetFileID( const std::string &filePath )
{
  FILE_SCOPE_TIMER((""));

  ManagedFilePtr file = GetFile( filePath );
  if ( file )
  {
    return file->m_Id;
  }

  return TUID::Null;
}

/////////////////////////////////////////////////////////////////////////////
void TempResolver::GetFilePath( const tuid id, std::string &filePath )
{
  FILE_SCOPE_TIMER((""));

  filePath = "";

  ManagedFilePtr file = GetFile( id );
  filePath = file->m_Path;
}

/////////////////////////////////////////////////////////////////////////////
void TempResolver::InsertManagedFile( ManagedFilePtr& file )
{
  M_TuidManagedFile::iterator foundFile = m_TuidManagedFiles.find( file->m_Id );
  if ( foundFile != m_TuidManagedFiles.end() )
  {
    if ( foundFile->second->m_Path == file->m_Path )
    {
      // we already have this file cached, early out
      return;
    }
    else
    {
      // erase the old file and the new one (if it's there)
      m_PathManagedFiles.erase( foundFile->second->m_Path );
      m_TuidManagedFiles.erase( foundFile->second->m_Id );

      // insert it
      m_TuidManagedFiles[file->m_Id] = file;
      m_PathManagedFiles[file->m_Path] = file;
    }
  }
  else
  {
    m_TuidManagedFiles[file->m_Id] = file;
    m_PathManagedFiles[file->m_Path] = file;
  }
}
