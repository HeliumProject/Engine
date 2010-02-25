#include "FileHandle.h"

#include "Windows/Windows.h"


using namespace Nocturnal;

u32 GetWin32MoveMethod( FileOffset offsetType )
{
  switch( offsetType )
  {
  case FileOffsets::Beginning:
    return FILE_BEGIN;

  case FileOffsets::Current:
    return FILE_CURRENT;

  case FileOffsets::End:
    return FILE_END;
  }

  return 0;
}

const char* GetWin32MoveMethodStr( FileOffset offsetType )
{
  switch( offsetType )
  {
  case FileOffsets::Beginning:
    return "beginning";

  case FileOffsets::Current:
    return "current";

  case FileOffsets::End:
    return "end";
  }

  return NULL;
}

FileHandle::FileHandle( const std::string& path, const char* mode )
: m_Mode( mode )
, m_FileHandle( NULL )
, m_OpenCount( 0 )
{
  m_Path.Set( path );
}

bool FileHandle::IsValid()
{
  return m_FileHandle != NULL;
}

bool FileHandle::Open()
{
  ++m_OpenCount;

  if ( m_FileHandle == NULL )
  {
    m_FileHandle = fopen( m_Path.c_str(), m_Mode );
  }

  return IsValid();
}

void FileHandle::Close()
{
  if ( --m_OpenCount == 0 && m_FileHandle )
  {
    fclose( m_FileHandle );
    m_FileHandle = NULL;
  }
}

u32 FileHandle::Read( u8* buffer, size_t amount )
{
  DWORD read = 0;

  if ( !::ReadFile( m_FileHandle, buffer, (DWORD)amount, &read, NULL ) )
  {
    throw FileOperationException( "Unable to read %d bytes from %s", amount, m_Path.c_str() );
  }

  return read;
}

void FileHandle::Write( u8* buffer, size_t amount )
{
  DWORD written = 0;

  // NOTE that we throw if we don't succeed writing the specified amount, which is different than read
  if ( !::WriteFile( m_FileHandle, buffer, (DWORD)amount, &written, NULL ) || written != amount )
  {
    throw FileOperationException( "Unable to write %d bytes from %s", written, m_Path.c_str() );
  }
}

void FileHandle::Seek( FileLocation location, FileOffset offsetType )
{
  LARGE_INTEGER loc;
  loc.QuadPart = location;

  LARGE_INTEGER newLoc;
  newLoc.QuadPart = 0;

  if ( !::SetFilePointerEx( m_FileHandle, loc, &newLoc, GetWin32MoveMethod( offsetType ) ) )
  {
    throw FileOperationException( "Unable to seek %I64d bytes from %s location in %s", location, GetWin32MoveMethodStr( offsetType ), m_Path.c_str() );
  }
}

FileLocation FileHandle::Tell()
{
  LARGE_INTEGER loc;
  loc.QuadPart = 0;

  LARGE_INTEGER zero;
  zero.QuadPart = 0;

  if ( !::SetFilePointerEx( m_FileHandle, zero, &loc, FILE_CURRENT ) )
  {
    throw FileOperationException( "Unable to determine location in %s", m_Path.c_str() );
  }

  return loc.QuadPart;
}
