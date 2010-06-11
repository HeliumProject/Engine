#include "Platform/Windows/Windows.h"
#include "Foundation/Exception.h"

#include "FileIterator.h"
#include "FileSystem.h"

#include "Platform/Assert.h"

using namespace FileSystem;

FileIterator::FileIterator()
: m_Done( false )
, m_Handle ( INVALID_HANDLE_VALUE )
{

}

FileIterator::FileIterator(const std::string &root, const std::string &spec, const FileIteratorFlags flag /* = IteratorFlags::Default */)
: m_Done( false )
, m_Handle ( INVALID_HANDLE_VALUE )
{
  Open( root, spec, flag );
}

FileIterator::~FileIterator()
{
  Close();
}

bool FileIterator::Next()
{
  return Find(false);
}

void FileIterator::Reset()
{
  Close();
  Find(true);
}

bool FileIterator::IsDone()
{
  return m_Done;
}

const std::string& FileIterator::Item()
{
  if ( m_Done )
  {
    throw FileSystem::Exception("The file iterator is invalid!");
  }

  return m_Item;
}

bool FileIterator::Open(const std::string &root, const std::string &spec /* = "" */, const FileIteratorFlags flag /* = IteratorFlags::Default */)
{
  // Clean the root string and append a trailing slash.
  m_Root = root;
  FileSystem::GuaranteeSlash( m_Root );
  FileSystem::Win32Name( m_Root );

  m_Spec = spec;
  m_Flags = flag;
  m_Query = m_Root + m_Spec;

  // check that the input is not larger than allowed
  if ( m_Query.size() > MAX_PATH )
  {
    throw FileSystem::Exception( "Query string is too long (max buffer length is %d): %s", ( int ) MAX_PATH, m_Query.c_str() );
  }

  return Find(true);
}

bool FileIterator::Find(bool first)
{
  DWORD error = 0x0;
  WIN32_FIND_DATA foundFile; 

  if (first)
  {
    if ( (m_Handle = ::FindFirstFile( m_Query.c_str(), &foundFile )) == INVALID_HANDLE_VALUE )
    {
      m_Done = true;

      Close();

      if ( (error = GetLastError()) != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND ) 
      {
        throw Nocturnal::PlatformException( error, "Error calling FindFirstFile" );
      }
    }
  }
  else
  {
    if ( ::FindNextFile( m_Handle, &foundFile ) == 0 )
    {      
      m_Done = true;

      Close();

      if ( (error = GetLastError()) != ERROR_NO_MORE_FILES ) 
      {
        throw Nocturnal::PlatformException( error, "Error calling FindNextFile" );
      }
    }
  }

  // while our current item isn't what we are looking for
  while (!m_Done)
  {
    bool ok = true;

    // skip relative path directories if fileName is "." or ".."
    if ( ( std::strcmp( foundFile.cFileName , "." ) == 0 ) || ( std::strcmp( foundFile.cFileName , ".." ) == 0 ) )
    {
      ok = false;
    }
    else
    {
      // directory...
      if ( foundFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
      {
        // NoDirs: skip directory files
        // also skip hidden/system directories, so we don't try to access "System Volume Information"
        if ( m_Flags & IteratorFlags::NoDirs  
          || ( foundFile.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )
          || ( foundFile.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) )
        {
          m_Item.clear();
          ok = false;
        }
      }
      // file...
      else
      {
        // NoFiles: skip files
        if ( m_Flags & IteratorFlags::NoFiles )
        {
          m_Item.clear();
          ok = false;
        }
      }
    }

    if (ok)
    {
      // It's a keeper! store the data and format the file name
      m_Item = foundFile.cFileName;

      // NoMarkDir: append trailing slash on directory names
      if ( ( foundFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) && !( m_Flags & IteratorFlags::NoMarkDir ) )
      {
        m_Item += "/";
      }

      // NoPrependRoot: add the root path to the fileName
      if ( !( m_Flags & IteratorFlags::NoPrependRoot ) )
      {
        m_Item.insert( 0, m_Root );
      }

      // NoClean clean the file names using StandardName
      if ( !( m_Flags & IteratorFlags::NoClean ) )
      {
        FileSystem::CleanName( m_Item );
      }

      break;
    }
    else
    {
      m_Item.clear();

      // this pumps the windows file iterator to the next element (into foundFile) for our next do...while loop
      if ( ::FindNextFile( m_Handle, &foundFile ) == 0 )
      {      
        m_Done = true;

        Close();

        if ( (error = GetLastError()) != ERROR_NO_MORE_FILES ) 
        {
          throw Nocturnal::PlatformException( error, "Error calling FindNextFile" );
        }
      }
    }
  }

  return !m_Done;
}

void FileIterator::Close()
{
  if ( m_Handle != INVALID_HANDLE_VALUE )
  {
    BOOL result = ::FindClose( m_Handle );
    m_Handle = INVALID_HANDLE_VALUE;
    NOC_ASSERT(result);
  }

  m_Done = true;
  m_Item.clear(); 
}