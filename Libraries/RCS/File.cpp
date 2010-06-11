#include "File.h"
#include "RCS.h"

#include "Foundation/File/File.h"
#include "Foundation/Log.h"

#include "Platform/Windows/Windows.h"
#undef GetUserName

using namespace RCS;

void File::GetInfo( const GetInfoFlag flags )
{
  GetProvider()->GetInfo( *this, flags );
}

void File::Sync( const u64 timestamp )
{
  RCS_SCOPE_TIMER( ( "" ) );

  u64 syncTime = timestamp ? timestamp : GetSyncTimestamp();

  GetProvider()->Sync( *this, syncTime );
}

void File::Add( const OpenFlag flags, const u64 changesetId )
{
  RCS_SCOPE_TIMER( ("") );

  GetInfo();

  if ( IsCheckedOutByMe() && ( m_Operation == Operations::Add || m_Operation == Operations::Branch ) )
  {
    return;
  }

  if ( ( ( ( flags & OpenFlags::Exclusive ) == OpenFlags::Exclusive ) || IsLocking() ) && IsCheckedOutBySomeoneElse() )
  {
    std::string usernames;
    GetOpenedByUsers( usernames );
    throw FileInUseException( m_LocalPath.c_str(), usernames.c_str() );
  }

  m_ChangesetId = changesetId;

  GetProvider()->Add( *this );
}

void File::Edit( const OpenFlag flags, const u64 changesetId )
{
  RCS_SCOPE_TIMER( ("") );

  GetInfo();

  if ( IsCheckedOutByMe() && m_Operation == Operations::Edit )
  {
    return;
  }

  if ( ( ( ( flags & OpenFlags::Exclusive ) == OpenFlags::Exclusive ) || IsLocking() ) && IsCheckedOutBySomeoneElse() )
  {
    std::string usernames;
    GetOpenedByUsers( usernames );
    throw FileInUseException( m_LocalPath.c_str(), usernames.c_str() );
  }

  if( (flags & OpenFlags::AllowOutOfDate) == 0)
  {
    if ( IsBinary() && !IsUpToDate() )
    {
      throw FileOutOfDateException( m_LocalPath.c_str(), m_LocalRevision, m_HeadRevision );
    }

    if ( m_LocalRevision <= 0 && m_LocalRevision != m_HeadRevision && !HeadDeleted() )
    {
      throw Exception( "File '%s' cannot be opened for edit because you do not have the file synced.", m_LocalPath.c_str() );
    }
  }

  m_ChangesetId = changesetId;

  GetProvider()->Edit( *this );
}

void File::Delete( const OpenFlag flags, const u64 changesetId )
{
  RCS_SCOPE_TIMER( ("") );

  GetInfo();

  if ( !ExistsInDepot() )
  {
    throw Exception( "File '%s' does not exist in revision control.", m_LocalPath.c_str() );
  }

  if ( IsCheckedOutByMe() && m_Operation == Operations::Delete )
  {
    return;
  }

  if ( ( ( ( flags & OpenFlags::Exclusive ) == OpenFlags::Exclusive ) || IsLocking() ) && ( IsCheckedOutBySomeoneElse() ) )
  {
    std::string usernames;
    GetOpenedByUsers( usernames );
    throw FileInUseException( m_LocalPath.c_str(), usernames.c_str() );
  }

  m_ChangesetId = changesetId;

  GetProvider()->Delete( *this );
}

void File::Reopen( const Changeset& changeset, const OpenFlag flags )
{
  RCS_SCOPE_TIMER( ("") );

  // verify we have it checked out
  GetInfo();
  if ( !IsCheckedOutByMe() )
  {
    throw Exception( "%s is not currently checked out.", m_LocalPath.c_str() );
  }

  m_ChangesetId = changeset.m_Id;

  GetProvider()->Reopen( *this );
}

void File::Copy( File& target, const OpenFlag flags, const u64 changesetId )
{
  RCS_SCOPE_TIMER( ("") );

  GetInfo();
  target.GetInfo();

  // here, we've already done this operation, effectively
  if ( target.IsCheckedOutByMe() && target.m_Operation == Operations::Branch )
  {
    return;
  }

  if ( ( ( flags & OpenFlags::Exclusive ) == OpenFlags::Exclusive ) && target.IsCheckedOutBySomeoneElse() )
  {
    std::string targetUsernames;
    target.GetOpenedByUsers( targetUsernames );
    throw FileInUseException( target.m_LocalPath.c_str(), targetUsernames.c_str() );
  }

  target.m_ChangesetId = changesetId;

  GetProvider()->Integrate( *this, target );
}

void File::Rename( File& target, const OpenFlag flags, const u64 changesetId )
{
  GetInfo();
  target.GetInfo();

  if ( IsCheckedOutByMe() && m_Operation == Operations::Delete )
  {
    // here, we've already done this operation, effectively
    if ( target.IsCheckedOutByMe() && target.m_Operation == Operations::Branch )
    {
      return;
    }

    // else, we have a problem
    throw Exception( "Cannot rename the deleted file '%s'.", m_LocalPath.c_str() );
  }

  if ( ( flags & OpenFlags::Exclusive ) == OpenFlags::Exclusive )
  {
    if (  IsCheckedOutBySomeoneElse() )
    {
      std::string usernames;
      GetOpenedByUsers( usernames );
      throw FileInUseException( m_LocalPath.c_str(), usernames.c_str() );
    }
    else if ( target.IsCheckedOutBySomeoneElse() )
    {
      std::string targetUsernames;
      target.GetOpenedByUsers( targetUsernames );
      throw FileInUseException( target.m_LocalPath.c_str(), targetUsernames.c_str() );
    }
  }

  if ( IsBinary() && !IsUpToDate() )
  {
    throw FileOutOfDateException( m_LocalPath.c_str(), m_LocalRevision, m_HeadRevision );
  }

  if ( !target.IsUpToDate() )
  {
    throw FileOutOfDateException( target.m_LocalPath.c_str(), target.m_LocalRevision, target.m_HeadRevision );
  }

  m_ChangesetId = changesetId;
  target.m_ChangesetId = changesetId;

  GetProvider()->Rename( *this, target );
}

void File::Revert( const OpenFlag flags )
{
  RCS_SCOPE_TIMER( ("") );

  GetInfo();

  if ( !IsCheckedOutByMe() )
  {
    return;
  }

  bool revertUnchangedOnly = ( flags & OpenFlags::UnchangedOnly ) == OpenFlags::UnchangedOnly;

  GetProvider()->Revert( *this, revertUnchangedOnly );
}

//
// if the file isn't there, this will create an empty
// file, if it is there already, this should have no
// effect.  Basically, we want to guarantee that there's
// a file at that location on disk when this returns OK.
//
static void _EnsureExistence( const std::string &path )
{
  RCS_SCOPE_TIMER( ("") );

  Nocturnal::File file( path );

  // if the file doesn't exist, create a zero length 
  if( !file.Exists() )
  {
    file.MakePath();

    FILE *f = fopen( path.c_str(), "w" );

    // if we couldn't open the file for some reason,
    // eg: write protected but not under revision control,
    // fail
    if ( !f )
    {
      throw Exception( "Could not create file on disk: ", path.c_str() );
    }

    fclose(f);
  }
}

void File::Open( const OpenFlag flags, const u64 changesetId )
{
  if ( !PathIsManaged( m_LocalPath ) )
  {
    Log::Warning( Log::Levels::Verbose,  "Attempted to Open unmanaged path (not opening in RCS, but ensuring file existence): %s\n", m_LocalPath.c_str() );
    _EnsureExistence( m_LocalPath );
    return;
  }

  GetInfo();

  if ( ExistsInDepot() )
  {
    if ( IsCheckedOutByMe() && m_Operation == Operations::Delete )
    {
      Revert();
      Edit( flags, changesetId );
    }
    else if ( IsCheckedOutByMe() )
    {
      Changeset changeset;
      changeset.m_Id = changesetId;
      Reopen( changeset, flags );
    }
    else if ( HeadDeleted() )
    {
      _EnsureExistence( m_LocalPath );
      Add( flags, changesetId );
    }
    else
    {
      Edit( flags, changesetId );
    }

    return;
  }

  // file doesn't exist in depot down here...
  _EnsureExistence( m_LocalPath );

  // open the resultant file for add, should show up in
  // their changelist
  Add( flags, changesetId );
}

bool File::QueryOpen( const OpenFlag flags, const u64 changesetId )
{
  std::string message;

  GetInfo();

  if ( !ExistsInDepot() )
  {
    message = m_LocalPath;
    message += " doesn't exist in revision control, do you want to create it?";

    if ( IDYES == ::MessageBox( GetActiveWindow(), message.c_str(), "Check Out?", MB_YESNO | MB_ICONEXCLAMATION ) )
    {
      Open( flags, changesetId );
    }

    return true;
  }

  if ( IsCheckedOutByMe() )
  {
    return true;
  }

  if ( ( ( flags & OpenFlags::Exclusive ) == OpenFlags::Exclusive ) && IsCheckedOutBySomeoneElse() )
  {
    std::string usernames;
    GetOpenedByUsers( usernames );
    message = m_LocalPath + std::string( " is already checked out by " ) + usernames + ", do you still wish to open the file?";

    // here, the user has basically overridden the exlusivity setting, so we just try to Open it
    if ( IDYES == ::MessageBox( GetActiveWindow(), message.c_str(), "Checked Out", MB_YESNO | MB_ICONEXCLAMATION ) )
    {
      Open( OpenFlags::Default, changesetId );
      return true;
    }
    else
    {
      return false;
    }
  }

  message = std::string( "Do you wish to check out " ) + m_LocalPath + std::string( "?" );

  // here, if someone's checked it out in the interim, and they were supposed to be exlusively doing it,
  // we want to let Open() fail, so we give it the exlusive flag
  if ( IDYES == ::MessageBox( GetActiveWindow(), message.c_str(), "Check Out?", MB_YESNO | MB_ICONEXCLAMATION ) )
  {
    Open( flags, changesetId );
    return true;
  }
  else
  {
    return false;
  }

  return false;
}

void File::Commit( const std::string& description )
{
  Changeset changeset;
  changeset.m_Description = description;
  changeset.Create();

  Reopen( changeset );
  changeset.Commit();
}

// Was: GetLastUser
void File::GetCreatedByUser( std::string& username )
{
  if ( m_Revisions.empty() )
  {
    GetInfo( (RCS::GetInfoFlag) ( RCS::GetInfoFlags::GetHistory | RCS::GetInfoFlags::GetIntegrationHistory ) );
  }

  if ( !m_Revisions.empty() )
  {
    username = (*m_Revisions.rbegin())->m_Username;
  }
}

void File::GetLastModifiedByUser( std::string& username )
{
  if ( m_Revisions.empty() )
  {
    GetInfo( (RCS::GetInfoFlag) ( RCS::GetInfoFlags::GetHistory | RCS::GetInfoFlags::GetIntegrationHistory )  );
  }

  if ( !m_Revisions.empty() )
  {
    username = (*m_Revisions.begin())->m_Username;
  }
}

// Was: GetOtherUsers
void File::GetOpenedByUsers( std::string& usernames )
{
  usernames = "";

  if ( m_Actions.empty() )
  {
    GetInfo();
  }

  if ( !m_Actions.empty() )
  {
    for( V_ActionPtr::const_iterator itr = m_Actions.begin(), end = m_Actions.end(); itr != end; ++itr )
    {
      usernames += (*itr)->m_Username;
      if ( itr + 1 != end )
      {
        usernames += ", ";
      }
    }
  }
}