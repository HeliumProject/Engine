#include <map>
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/timeb.h>
#include <time.h>
#include <vector>

#include "RCS.h"
#include "RCSExceptions.h"

#include "Foundation/File/Path.h"
#include "Foundation/Log.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::RCS;

Provider*     g_Provider = NULL;
Changeset     g_DefaultChangeset;
std::vector< tstring >      g_ManagedPaths;
std::vector< tstring >      g_IgnoredPaths;
u64           g_SyncTimestamp = 0;

///////////////////////////////////////////////////////////////////
// Utility methods

// 
// prepends projectRoot onto each element of paths 
// checks to see if the resulting prefix is found in the query path
// 
static bool _IsSubdir( const tstring& query, std::vector< tstring >& paths )
{
  Helium::Path queryPath( query );

  for( std::vector< tstring >::const_iterator it = paths.begin(), end = paths.end(); it != end; ++it )
  {
    Helium::Path path( (*it) );

    if ( queryPath.Normalized().compare( 0, path.length(), path.Normalized() ) == 0 )
    {
      return true; 
    }
  }

  return false; 
}

Changeset& RCS::DefaultChangeset()
{
  return g_DefaultChangeset;
}

void RCS::SetProvider( Provider* provider )
{
  if ( provider != NULL && g_Provider != NULL )
  {
    throw RCS::Exception( TXT( "Attempt to re-set provider.  Current provider is: %s\n" ), g_Provider->GetName() );
  }

  g_Provider = provider;
}

Provider* RCS::GetProvider()
{
  return g_Provider;
}

///////////////////////////////////////////////////////////////////
// Implementation

void RCS::SetManagedPaths( const std::vector< tstring >& paths )
{
  g_ManagedPaths = paths;
}

void RCS::SetIgnoredPaths( const std::vector< tstring >& paths )
{
  g_IgnoredPaths = paths;
}

bool RCS::PathIsManaged( const tstring &path )
{
  // check to see if we should be ignoring this path first
  if( !g_IgnoredPaths.empty() && _IsSubdir( path, g_IgnoredPaths ) )
  {
    return false;
  }

  // check to see if it's in our list of paths to pay attention to
  return _IsSubdir( path, g_ManagedPaths );
}

void RCS::SetSyncTimestamp( const u64 timestamp )
{
  if ( timestamp == SyncTimes::Now )
  {
    __time64_t t;
    _time64( &t );
    g_SyncTimestamp = t;
  }
  else
  {
    g_SyncTimestamp = timestamp;
  }
}

u64 RCS::GetSyncTimestamp()
{
  return g_SyncTimestamp;
}

// Changelist functions

void RCS::GetChangesets( RCS::V_Changeset& changesets )
{
  g_Provider->GetChangesets( changesets );

  V_File files;
  GetOpenedFiles( files );

  V_Changeset::iterator changeItr = changesets.begin();
  V_Changeset::iterator changeEnd = changesets.end();

  V_File::const_iterator itr = files.begin();
  V_File::const_iterator end = files.end();
  for( ; itr != end; ++itr )
  {
    changeItr = changesets.begin();

    for( ; changeItr != changeEnd; ++changeItr )
    {
      if ( (*changeItr).m_Id == (*itr).m_ChangesetId )
      {
        (*changeItr).m_Files.push_back( (*itr) );
        break;
      }
    }
  }
}

bool RCS::IsValidChangeset( const RCS::Changeset& changeset )
{
  if ( changeset.m_Id == DefaultChangesetId )
  {
    return true;
  }

  RCS::V_Changeset currentChangesets;
  g_Provider->GetChangesets( currentChangesets );

  RCS::V_Changeset::const_iterator itr = currentChangesets.begin();
  RCS::V_Changeset::const_iterator end = currentChangesets.end();
  for ( ; itr != end; ++itr )
  {
    if ( (*itr).m_Id == changeset.m_Id )
    {
      return true;
    }
  }
  
  return false;
}

void RCS::GetInfo( V_File& files, GetInfoFlag flags )
{
  for ( V_File::iterator itr = files.begin(), end = files.end();
    itr != end;
    ++itr )
  {
    (*itr).GetInfo( flags );
  }
}

void RCS::GetInfo( const tstring& folder, V_File& files, bool recursive, u32 fileData, u32 actionData )
{
  g_Provider->GetInfo( folder, files, recursive, fileData, actionData );
}

void RCS::GetOpenedFiles( V_File &files )
{
  g_Provider->GetOpenedFiles( files );
}
