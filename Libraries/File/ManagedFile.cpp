#include "ManagedFile.h"
#include "History.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"

using namespace File;

/////////////////////////////////////////////////////////////////////////////
//
// ManagedFile
//
/////////////////////////////////////////////////////////////////////////////

REFLECT_DEFINE_CLASS( ManagedFile );
void ManagedFile::EnumerateClass( Reflect::Compositor<ManagedFile>& comp )
{
  Reflect::Field* fieldId = comp.AddField( &ManagedFile::m_Id, "m_Id", Reflect::FieldFlags::FileID );
  Reflect::Field* fieldCreated = comp.AddField( &ManagedFile::m_Created, "m_Created" );
  Reflect::Field* fieldModified = comp.AddField( &ManagedFile::m_Modified, "m_Modified" );
  Reflect::Field* fieldPath = comp.AddField( &ManagedFile::m_Path, "m_Path" );
  Reflect::Field* fieldUserId = comp.AddField( &ManagedFile::m_UserId, "m_UserId" );
  Reflect::Field* fieldUsername = comp.AddField( &ManagedFile::m_Username, "m_Username" );
  Reflect::Field* fieldWasDeleted = comp.AddField( &ManagedFile::m_WasDeleted, "m_WasDeleted" );
}

/////////////////////////////////////////////////////////////////////////////
// File constructor
//
ManagedFile::ManagedFile()
: m_Id( TUID::Null )
, m_Created( 0 )
, m_Modified( 0 )
, m_Path( "" )
, m_UserId( 0 )
, m_Username( "" )
, m_WasDeleted( false )
{
}

ManagedFile::~ManagedFile()
{
}

/////////////////////////////////////////////////////////////////////////////
// Overloaded < operator to support correct sorting in sets by file path
//
bool ManagedFile::operator<( const ManagedFile& rhs ) const
{
  return ( _stricmp( m_Path.c_str(), rhs.m_Path.c_str() ) < 0 );
}

/////////////////////////////////////////////////////////////////////////////
// Overloaded == operator: Determines if two Files are equal
// returns true if the lhs was created before the rhs
//
bool ManagedFile::operator==( const ManagedFile& rhs ) const
{
  return ( ( m_Id == rhs.m_Id )
    && ( m_Created == rhs.m_Created )
    && ( m_Modified == rhs.m_Modified )
    && ( m_Path.compare( rhs.m_Path ) == 0 )
    && ( m_WasDeleted == rhs.m_WasDeleted )
    );
}

/////////////////////////////////////////////////////////////////////////////
//
// ManagedFileStash
//
/////////////////////////////////////////////////////////////////////////////
REFLECT_DEFINE_CLASS( ManagedFileStash );
void ManagedFileStash::EnumerateClass( Reflect::Compositor<ManagedFileStash>& comp )
{
  Reflect::Field* fieldFiles = comp.AddField( &ManagedFileStash::m_Files, "m_Files" );
}