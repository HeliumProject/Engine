#include "Platform/Windows/Windows.h"

#include "File.h"
#include "FileSystem.h"

#include "Foundation/Checksum/MD5.h"
#include "Foundation/Checksum/Crc32.h"
#include "Foundation/String/Utilities.h"

#include <sstream>

using namespace FileSystem;


/////////////////////////////////////////////////////////////////////////////
u32 GetWin32Access( FileMode mode )
{
  u32 result = 0;

  if ( mode & FileModes::Read )
    result |= FILE_READ_DATA;

  if ( mode & FileModes::Write )
    result |= FILE_WRITE_DATA;

  return result;
}


/////////////////////////////////////////////////////////////////////////////
u32 GetWin32Disposition( FileMode mode )
{
  if ( mode & FileModes::Append )
    return OPEN_EXISTING;

  if ( mode & FileModes::Truncate )
    return TRUNCATE_EXISTING;

  if ( mode & FileModes::Write )
    return OPEN_ALWAYS;

  return OPEN_EXISTING;
}


/////////////////////////////////////////////////////////////////////////////
u32 GetWin32AttributeFlags( FileMode mode )
{
  u32 flags = FILE_ATTRIBUTE_NORMAL;

  if ( mode & FileModes::SequentialAccess )
  {
    flags |= FILE_FLAG_SEQUENTIAL_SCAN;
  }
  else if ( mode & FileModes::RandomAccess )
  {
    flags |= FILE_FLAG_RANDOM_ACCESS;
  }

  return flags;
}


/////////////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////////////
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


/////////////////////////////////////////////////////////////////////////////
File::File()
  : m_Handle( INVALID_HANDLE_VALUE )
  , m_Path( "" )
{

}

/////////////////////////////////////////////////////////////////////////////
File::File( const std::string& path )
  : m_Handle( INVALID_HANDLE_VALUE )
  , m_Path( path )
{

}

/////////////////////////////////////////////////////////////////////////////
File::~File()
{
  Close();
}

/////////////////////////////////////////////////////////////////////////////
const std::string& File::GetPath()
{
  return m_Path;
}

/////////////////////////////////////////////////////////////////////////////
void File::SetPath( const std::string& path )
{
  if ( m_Path == path )
  {
    return;
  }

  if ( IsOpen() )
  {
    throw Exception ( "Cannot open file %s because %s is still open", path.c_str(), m_Path.c_str() );
  }

  m_Path = path;
}

  
bool File::IsOpen()
{
  return m_Handle != INVALID_HANDLE_VALUE;
}


/////////////////////////////////////////////////////////////////////////////

void File::VerifyOpen()
{
  if ( m_Handle == INVALID_HANDLE_VALUE )
  {
    if ( m_Path.empty() )
    {
      throw Exception( "There is no file specified" );
    }
    else
    {
      throw Exception( "The file %s is not opened", m_Path.c_str() );
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
void File::Open( FileMode mode )
{
  Close();

  if ( m_Path.empty() )
  {
    throw Exception ( "There is no file specified" );
  }

  m_Handle = ::CreateFile(
    m_Path.c_str(),
    GetWin32Access( mode ),
    0,                            // not shared in any way
    NULL,                         // who cares about security?
    GetWin32Disposition(mode),
    GetWin32AttributeFlags(mode), // we are NORMAL, baby
    NULL );

  if ( m_Handle == INVALID_HANDLE_VALUE )
  {
    throw FileOperationException ( "Unable to open %s for %s", m_Path.c_str(), mode & FileModes::Write ? "write" : "read" );
  }
}

/////////////////////////////////////////////////////////////////////////////
const u32 File::GetSize()
{
  DWORD file_size = 0;

  if(m_Handle != INVALID_HANDLE_VALUE)
  {
    file_size = ::GetFileSize(m_Handle, NULL);
    if(file_size == INVALID_FILE_SIZE)
    {
      return 0;
    }
  }
  return file_size;
}


/////////////////////////////////////////////////////////////////////////////
void File::Open( const std::string& path, FileMode mode )
{
  SetPath( path );

  Open( mode );
}


/////////////////////////////////////////////////////////////////////////////
void File::Close()
{
  if (m_Handle != INVALID_HANDLE_VALUE)
  {
    if (m_Path.empty())
    {
      throw Exception ("There is no file specified");
    }

    if (!::CloseHandle(m_Handle))
    {
      throw FileOperationException ("Unable to close %s", m_Path.c_str());
    }

    m_Handle = INVALID_HANDLE_VALUE;
  }
}


/////////////////////////////////////////////////////////////////////////////
u32 File::Read(u8* buffer, size_t amount)
{
  VerifyOpen();

  DWORD read = 0;

  if (!::ReadFile(m_Handle, buffer, (DWORD)amount, &read, NULL))
  {
    throw FileOperationException ("Unable to read %d bytes from %s", amount, m_Path.c_str());
  }

  return read;
}


/////////////////////////////////////////////////////////////////////////////
void File::Write(u8* buffer, size_t amount)
{
  VerifyOpen();

  DWORD written = 0;

  // NOTE that we throw if we don't succeed writing the specified amount, which is different than read
  if (!::WriteFile(m_Handle, buffer, (DWORD)amount, &written, NULL) || written != amount)
  {
    throw FileOperationException ("Unable to write %d bytes from %s", written, m_Path.c_str());
  }
}


/////////////////////////////////////////////////////////////////////////////
void File::Seek(FileLocation location, FileOffset offsetType)
{
  VerifyOpen();

  LARGE_INTEGER loc;
  loc.QuadPart = location;

  LARGE_INTEGER newLoc;
  newLoc.QuadPart = 0;

  if (!::SetFilePointerEx(m_Handle, loc, &newLoc, GetWin32MoveMethod(offsetType)))
  {
    throw FileOperationException ("Unable to seek %I64d bytes from %s location in %s", location, GetWin32MoveMethodStr(offsetType), m_Path.c_str());
  }
}


/////////////////////////////////////////////////////////////////////////////
FileLocation File::Tell()
{
  VerifyOpen();

  LARGE_INTEGER loc;
  loc.QuadPart = 0;

  LARGE_INTEGER zero;
  zero.QuadPart = 0;

  if (!::SetFilePointerEx(m_Handle, zero, &loc, FILE_CURRENT))
  {
    throw FileOperationException ("Unable to determine location in %s", m_Path.c_str());
  }

  return loc.QuadPart;
}


/////////////////////////////////////////////////////////////////////////////
void File::GenerateCRC( const std::string& filePath, std::string &hashString )
{
  u32 crc = Nocturnal::FileCrc32( filePath );

  std::stringstream str;
  str << std::hex << std::uppercase << crc;
  hashString = str.str();
}


/////////////////////////////////////////////////////////////////////////////
bool File::VerifyCRC( const std::string& filePath, const std::string &hashString )
{
  std::string curHashString;
  GenerateCRC( filePath, curHashString );

  return curHashString.compare( hashString ) == 0;
}


/////////////////////////////////////////////////////////////////////////////
void File::GenerateMD5( const std::string& filePath, std::string &hashString )
{
  hashString = Nocturnal::FileMD5( filePath );
}

/////////////////////////////////////////////////////////////////////////////
bool File::VerifyMD5( const std::string& filePath, const std::string &hashString )
{
  std::string curHashString;
  GenerateMD5( filePath, curHashString );

  return ( curHashString.compare( hashString ) == 0 ) ? true : false ;
}
