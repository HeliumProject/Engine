#include "ApplicationPch.h"
#include "RCSFile.h"

#include "Foundation/FilePath.h"
#include "Foundation/Log.h"

#include "Application/RCS.h"

using namespace Helium;
using namespace Helium::RCS;

void RCS::File::GetInfo( const GetInfoFlag flags )
{
	GetProvider()->GetInfo( *this, flags );
}

void RCS::File::Sync( const uint64_t timestamp )
{
	uint64_t syncTime = timestamp ? timestamp : GetSyncTimestamp();

	GetProvider()->Sync( *this, syncTime );
}

void RCS::File::Add( const OpenFlag flags, const uint64_t changesetId )
{
	GetInfo();

	if ( IsCheckedOutByMe() && ( m_Operation == Operations::Add || m_Operation == Operations::Branch ) )
	{
		return;
	}

	if ( ( ( ( flags & OpenFlags::Exclusive ) == OpenFlags::Exclusive ) || IsLocking() ) && IsCheckedOutBySomeoneElse() )
	{
		tstring usernames;
		GetOpenedByUsers( usernames );
		throw FileInUseException( m_LocalPath.c_str(), usernames.c_str() );
	}

	m_ChangesetId = changesetId;

	GetProvider()->Add( *this );
}

void RCS::File::Edit( const OpenFlag flags, const uint64_t changesetId )
{
	GetInfo();

	if ( IsCheckedOutByMe() && m_Operation == Operations::Edit )
	{
		return;
	}

	if ( ( ( ( flags & OpenFlags::Exclusive ) == OpenFlags::Exclusive ) || IsLocking() ) && IsCheckedOutBySomeoneElse() )
	{
		tstring usernames;
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
			throw Exception( TXT( "File '%s' cannot be opened for edit because you do not have the file synced." ), m_LocalPath.c_str() );
		}
	}

	m_ChangesetId = changesetId;

	GetProvider()->Edit( *this );
}

void RCS::File::Delete( const OpenFlag flags, const uint64_t changesetId )
{
	GetInfo();

	if ( !ExistsInDepot() )
	{
		throw Exception( TXT( "File '%s' does not exist in revision control." ), m_LocalPath.c_str() );
	}

	if ( IsCheckedOutByMe() && m_Operation == Operations::Delete )
	{
		return;
	}

	if ( ( ( ( flags & OpenFlags::Exclusive ) == OpenFlags::Exclusive ) || IsLocking() ) && ( IsCheckedOutBySomeoneElse() ) )
	{
		tstring usernames;
		GetOpenedByUsers( usernames );
		throw FileInUseException( m_LocalPath.c_str(), usernames.c_str() );
	}

	m_ChangesetId = changesetId;

	GetProvider()->Delete( *this );
}

void RCS::File::Reopen( const Changeset& changeset, const OpenFlag flags )
{
	// verify we have it checked out
	GetInfo();
	if ( !IsCheckedOutByMe() )
	{
		throw Exception( TXT( "%s is not currently checked out." ), m_LocalPath.c_str() );
	}

	m_ChangesetId = changeset.m_Id;

	GetProvider()->Reopen( *this );
}

void RCS::File::Copy( File& target, const OpenFlag flags, const uint64_t changesetId )
{
	GetInfo();
	target.GetInfo();

	// here, we've already done this operation, effectively
	if ( target.IsCheckedOutByMe() && target.m_Operation == Operations::Branch )
	{
		return;
	}

	if ( ( ( flags & OpenFlags::Exclusive ) == OpenFlags::Exclusive ) && target.IsCheckedOutBySomeoneElse() )
	{
		tstring targetUsernames;
		target.GetOpenedByUsers( targetUsernames );
		throw FileInUseException( target.m_LocalPath.c_str(), targetUsernames.c_str() );
	}

	target.m_ChangesetId = changesetId;

	GetProvider()->Integrate( *this, target );
}

void RCS::File::Rename( File& target, const OpenFlag flags, const uint64_t changesetId )
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
		throw Exception( TXT( "Cannot rename the deleted file '%s'." ), m_LocalPath.c_str() );
	}

	if ( ( flags & OpenFlags::Exclusive ) == OpenFlags::Exclusive )
	{
		if (  IsCheckedOutBySomeoneElse() )
		{
			tstring usernames;
			GetOpenedByUsers( usernames );
			throw FileInUseException( m_LocalPath.c_str(), usernames.c_str() );
		}
		else if ( target.IsCheckedOutBySomeoneElse() )
		{
			tstring targetUsernames;
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

void RCS::File::Revert( const OpenFlag flags )
{
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
static void _EnsureExistence( const tstring &path )
{
	Helium::FilePath file( path );

	// if the file doesn't exist, create a zero length 
	if( !file.Exists() )
	{
		file.Create();
	}
}

void RCS::File::Open( const OpenFlag flags, const uint64_t changesetId )
{
	if ( !PathIsManaged( m_LocalPath ) )
	{
		Log::Warning( Log::Levels::Verbose, TXT( "Attempted to Open unmanaged path (not opening in RCS, but ensuring file existence): %s\n" ), m_LocalPath.c_str() );
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

bool RCS::File::QueryOpen( MessageSignature::Delegate messageHandler, const OpenFlag flags, const uint64_t changesetId )
{
	tstring message;

	GetInfo();

	if ( !ExistsInDepot() )
	{
		message = m_LocalPath;
		message += TXT( " doesn't exist in revision control, do you want to create it?" );

		HELIUM_ASSERT( messageHandler.Valid() );
		if ( messageHandler.Valid() )
		{
			MessageArgs args ( TXT( "Check Out?" ), message, MessagePriorities::Question, MessageAppearances::YesNo );
			messageHandler.Invoke( args );
			if ( args.m_Result == MessageResults::Yes )
			{
				Open( flags, changesetId );
			}
		}

		return true;
	}

	if ( IsCheckedOutByMe() )
	{
		return true;
	}

	if ( ( ( flags & OpenFlags::Exclusive ) == OpenFlags::Exclusive ) && IsCheckedOutBySomeoneElse() )
	{
		tstring usernames;
		GetOpenedByUsers( usernames );
		message = m_LocalPath + TXT( " is already checked out by " ) + usernames + TXT( ", do you still wish to open the file?" );

		HELIUM_ASSERT( messageHandler.Valid() );
		if ( messageHandler.Valid() )
		{
			MessageArgs args ( TXT( "Checked Out" ), message, MessagePriorities::Question, MessageAppearances::YesNo );
			messageHandler.Invoke( args );
			if ( args.m_Result == MessageResults::Yes )
			{
				Open( OpenFlags::Default, changesetId );
				return true;
			}
		}

		return false;
	}

	message = tstring( TXT( "Do you wish to check out " ) ) + m_LocalPath + TXT( "?" );

	HELIUM_ASSERT( messageHandler.Valid() );
	if ( messageHandler.Valid() )
	{
		MessageArgs args ( TXT( "Check Out?" ), message, MessagePriorities::Question, MessageAppearances::YesNo );
		messageHandler.Invoke( args );
		if ( args.m_Result == MessageResults::Yes )
		{
			Open( flags, changesetId );
			return true;
		}
	}

	return false;
}

void RCS::File::Commit( const tstring& description )
{
	Changeset changeset;
	changeset.m_Description = description;
	changeset.Create();

	Reopen( changeset );
	changeset.Commit();
}

// Was: GetLastUser
void RCS::File::GetCreatedByUser( tstring& username )
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

void RCS::File::GetLastModifiedByUser( tstring& username )
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
void RCS::File::GetOpenedByUsers( tstring& usernames )
{
	usernames = TXT( "" );

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
				usernames += TXT( ", " );
			}
		}
	}
}