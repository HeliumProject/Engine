#include "Reference.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"

#include "Manager.h"
#include "RCS/RCS.h"

using namespace File;

REFLECT_DEFINE_CLASS( Reference );

void Reference::EnumerateClass( Reflect::Compositor< Reference >& comp )
{
  comp.AddField( &Reference::m_Id, "m_Id", Reflect::FieldFlags::FileID );
  comp.AddField( &Reference::m_CreationTime, "m_CreationTime" );
  comp.AddField( &Reference::m_ModifiedTime, "m_ModifiedTime" );
  comp.AddField( &Reference::m_LastUsername, "m_LastUsername" );
  comp.AddField( &Reference::m_Path, "m_Path" );
  comp.AddField( &Reference::m_Signature, "m_Signature" );
  comp.AddField( &Reference::m_FileModifiedTime, "m_FileModifiedTime" );
  comp.AddField( &Reference::m_FileCreationTime, "m_FileCreationTime" );
  comp.AddField( &Reference::m_FileSize, "m_FileSize" );
}

Reference::Reference( const std::string& path )
: m_Id( TUID::Null )
, m_CreationTime( (u64) _time64( NULL ) )
, m_ModifiedTime( 0 )
, m_LastUsername( getenv( "USERNAME" ) )
, m_Path( "" )
, m_Signature( "" )
, m_FileCreationTime( 0 )
, m_FileModifiedTime( 0 )
, m_FileSize( 0 )
{
  if ( !path.empty() )
  {
    SetPath( path );
  }
}

Reference::~Reference()
{
}

std::string Reference::AsString( bool verbose )
{
  Update();

  char fileString[ 512 ];
  
  if ( verbose )
  {
    sprintf_s( fileString, 512, "id           : "TUID_HEX_FORMAT"\n" \
                                "path         : %s\n" \
                                "user         : %s\n" \
                                "created      : %I64u\n" \
                                "modified     : %I64u\n" \
                                "file created : %I64u\n" \
                                "file modified: %I64u\n" \
                                "signature    : %s\n" \
                                ,
                                m_Id,
                                m_Path.c_str(),
                                m_LastUsername.c_str(),
                                m_CreationTime,
                                m_ModifiedTime,
                                m_FileCreationTime,
                                m_FileModifiedTime,
                                m_Signature.c_str() );
  }
  else
  {
    sprintf_s( fileString, 512, "%s ("TUID_HEX_FORMAT")", m_Path.c_str(), m_Id );
  }

  return fileString;
}

bool Reference::IsValid()
{
  Update();

  if ( m_Id == TUID::Null || m_Path.empty() )
  {
    return false;
  }

  return true;
}

void Reference::Update()
{
  if ( !File::GlobalManager().GetPath( m_Id, m_Path ) )
  {
    if ( !m_Path.empty() )
    {
      File::GlobalManager().Add( m_Path, m_Id );
    }
    else
    {
      return; // can't do anything, no path, no valid id...
    }
  }

  struct _stat64 fileStatus;
  memset( &fileStatus, 0, sizeof( fileStatus ) );
  if ( _stat64( m_Path.c_str(), &fileStatus ) != 0 )
  {
  
    // file doesn't exist on disk, try to use revision control to track it down
    RCS::File rcsFile( m_Path );
    rcsFile.GetInfo( (RCS::GetInfoFlag) ( RCS::GetInfoFlags::GetHistory | RCS::GetInfoFlags::GetIntegrationHistory ) );

    if ( !rcsFile.ExistsInDepot() )
    {
      return;
    }

    bool found = false;

    if ( !rcsFile.m_Revisions.empty() )
    {
      RCS::V_RevisionPtr::reverse_iterator itr = rcsFile.m_Revisions.rbegin();
      RCS::V_RevisionPtr::reverse_iterator end = rcsFile.m_Revisions.rend();
      for( ; itr != end; ++itr )
      {
        if ( !(*itr)->m_IntegrationTargets.empty() )
        {
          RCS::V_FilePtr::iterator infoItr = (*itr)->m_IntegrationTargets.begin();
          RCS::V_FilePtr::iterator infoEnd = (*itr)->m_IntegrationTargets.end();
          for( ; infoItr != infoEnd; ++infoItr )
          {
            // Get the info for the target file
            (*infoItr)->GetInfo();

            // likely match
            if ( (*infoItr)->m_Digest == m_Signature )
            {
              memset( &fileStatus, 0, sizeof( fileStatus ) );
              if ( _stat64( m_Path.c_str(), &fileStatus ) != 0 )
              {
                SetPath( (*infoItr)->m_LocalPath );
                found = true;
              }
            }
          }
        }
      }
    }

    if ( !found )
    {
      return;
    }
  }

  if ( m_FileModifiedTime != (u64) fileStatus.st_mtime )
  {
    FileSystem::File::GenerateMD5( m_Path, m_Signature );

    m_FileModifiedTime = (u64) fileStatus.st_mtime;
    m_FileCreationTime = (u64) fileStatus.st_ctime;
    m_FileSize = (u64) fileStatus.st_size;

    m_ModifiedTime = (u64) _time64( NULL );
  }
}

void Reference::SetPath( const std::string& path )
{
  m_Path = path;
  m_Id = File::GlobalManager().Open( m_Path );

  m_LastUsername = getenv( "USERNAME" );

  m_ModifiedTime = (u64) _time64( NULL );
}