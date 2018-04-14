#include "ApplicationPch.h"
#include "RCSChangeset.h"
#include "RCS.h"

using namespace Helium::RCS;

void Changeset::Open( File& file, const OpenFlag flags ) const
{
  file.Open( flags, m_Id );
}

void Changeset::Edit( File& file, const OpenFlag flags ) const
{
  file.Edit( flags, m_Id );
}

void Changeset::Copy( File& source, File& target, const OpenFlag flags ) const
{
  source.Copy( target, flags, m_Id );
}

void Changeset::Rename( File& source, File& target, const OpenFlag flags ) const
{
    source.Rename( target, flags, m_Id );
}

void Changeset::Delete( File& file, const OpenFlag flags ) const
{
  file.Delete( flags, m_Id );
}

void Changeset::Create()
{
  GetProvider()->CreateChangeset( *this );
}

void Changeset::Commit()
{
  // if they are committing the default changeset, we need to gather
  // up the files that are open in there and move them into the newly
  // created changeset
  if ( m_Id == DefaultChangesetId )
  {
    V_File files;
    GetOpenedFiles( files );

    if ( files.size() > 0 )
    {
      Create();

      V_File::iterator fItr = files.begin();
      V_File::iterator fEnd = files.end();
      for ( ; fItr != fEnd; ++fItr )
      {
        if ( (*fItr).m_ChangesetId == DefaultChangesetId )
        {
          (*fItr).m_ChangesetId = m_Id;
          GetProvider()->Reopen( (*fItr) );
        }
      }
    }
  }

  GetProvider()->Commit( *this );
}

void Changeset::Revert( const OpenFlag flags )
{
  GetProvider()->Revert( *this, ( ( flags & OpenFlags::UnchangedOnly ) == OpenFlags::UnchangedOnly ) );
  m_Id = DefaultChangesetId;
}

void Changeset::Reopen( File& file, const OpenFlag flags ) const
{
  // verify we have it checked out
  file.GetInfo();
  if ( !file.IsCheckedOutByMe() )
  {
    throw Exception( "%s is not currently checked out.", file.m_LocalPath.c_str() );
  }

  file.m_ChangesetId = m_Id;

  GetProvider()->Reopen( file );
}