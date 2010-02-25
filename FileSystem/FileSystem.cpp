#include "Windows/Windows.h"
#include "Windows/Error.h"

#pragma comment(lib, "version")

#include "FileSystem.h"
#include "FileIterator.h"

#include "Common/Assert.h" 
#include "Common/String/Utilities.h"
#include "Common/String/Natural.h"
#include "Common/Boost/Regex.h" 
#include "Common/String/Tokenize.h"

#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <sys/timeb.h>
#include <time.h>
#include <fstream>
#include <iostream>

// The FileSystem library contains convenience functions for manipulating
//  paths, files and directories through heavy use of the boost::filesystem
//  library, docs found here:
//   http://www.boost.org/libs/filesystem/doc/index.htm

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/convenience.hpp>
namespace BoostFS = boost::filesystem;



//*************************************************************************//
//
// CLEAN NAME FUNCTIONS
//
//*************************************************************************//


/////////////////////////////////////////////////////////////////////////////
// Cleans back slashes and tolowers string path
//
// Allows any character except 0x0-0x1F, '<', '>', ':', '"', '/', '\', and '|'.
// Furthermore, names must not end with a trailing space or period.
//
const std::string StandardName(const std::string &dirtyName, const bool slashes, const bool lower)
{
#pragma TODO("Consider search and replace of GetInvalidFileNameChars() and GetInvalidPathChars(), this would help validate the paths are viable native paths (eg. don't contain &, \\, etc..) which is a throw-able use case")

  // In regex, if you are looking for the backslash ('\') character,
  // you need to escape it ("\\"); and since this is stored in a c++ string,
  // you need to escape each slash. Hense "[\\\\/]+" evaluates to this regex "[\\/]+",
  // searching for one or more '\' OR '/' characters and replacing them with a 
  // single forward slash ('/').


  // matches a leading group of one or more back/forward slashes
  const boost::regex s_MatchDirectoryHead( "^[\\\\/]{2,}" ); 
  const boost::regex s_MatchSlashes( "[\\\\/]+" ); 

  FILESYSTEM_SCOPE_TIMER((""));

  // Make sure server path prefix gets normalized, special cased because otherwise
  // we would replace 2 slashes with just 1 slash (below) .. 
  // 
  std::string serverPrefix = ""; 
  boost::smatch matchResult;
  if ( boost::regex_search(dirtyName, matchResult, s_MatchDirectoryHead))
  {
    // the entire server prefix, when we are done, will be // or \\, but
    // we will need to prepend exactly one of those separators to fix up
    // our removal of repeated slashes in a bit... 
    // 
    serverPrefix = slashes ? "/" : "\\"; 
  }

  std::string cleanName; 

  // normalize the slashes throughout the string, combines adjacent slashes
  const char* outputFormat = slashes ? "/" : "\\\\"; 
  cleanName = boost::regex_replace(dirtyName, s_MatchSlashes, outputFormat); 
  cleanName = serverPrefix + cleanName; 

  // lowercase the name
  if ( lower )
  {
    ::toLower( cleanName );
  }

  return cleanName;
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::CleanName( const std::string &dirtyName, std::string &cleanName, const bool lower )
{
  cleanName = StandardName( dirtyName, true, lower );
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::CleanName( std::string &fileName, const bool lower )
{
  fileName = StandardName( fileName, true, lower );
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::Win32Name( const std::string &dirtyName, std::string &win32Name, const bool lower )
{
  win32Name = StandardName( dirtyName, false, lower );
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::Win32Name( std::string &fileName, const bool lower )
{
  fileName = StandardName( fileName, false, lower );
}



//*************************************************************************//
//
// MODIFY PATH FUNCTIONS
//
//*************************************************************************//


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::MakeAbsolute( const std::string &filePath, std::string &absFilePath )
{
  BoostFS::path absPath = BoostFS::system_complete( filePath );
  absFilePath = absPath.string();
  CleanName( absFilePath ); // boost returns paths that don't conform to our CleanName standards
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::MakeAbsolute( std::string &filePath )
{
  MakeAbsolute( ( const std::string ) filePath, filePath );
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::MakeRelative( const std::string& filePath, const std::string& dirPath, std::string& relative )
{
  std::string cleanFileName = filePath;
  MakeAbsolute( filePath, cleanFileName );
  BoostFS::path cleanFilePath = cleanFileName;
  
  std::string cleanDirName = dirPath;
  MakeAbsolute( dirPath, cleanDirName );
  BoostFS::path cleanDirPath = cleanDirName;

  BoostFS::path relativePath = relative;

  // This should only work for paths with drive letters in common
  if ( ( cleanFilePath.root_name().find_first_of( ":" ) == std::string::npos ) ||
       ( cleanDirPath.root_name().find_first_of( ":" ) == std::string::npos ) ||
       ( cleanFilePath.root_name().compare( cleanDirPath.root_name() ) != 0 ) )
  {
    relative = filePath;
    return;
  }

  // walk the paths and count the number of directories from teh root to replace with ".."
  BoostFS::path::iterator itFile = cleanFilePath.begin();
  BoostFS::path::iterator endItFile = cleanFilePath.end();

  BoostFS::path::iterator itDir = cleanDirPath.begin();
  BoostFS::path::iterator endItDir = cleanDirPath.end();

  // advance the iterator while they are equal directories
  while ( ( itDir != endItDir ) && ( itFile != endItFile ) &&
    ( (*itFile).compare( (*itDir) ) == 0 ) )
  {
    ++itFile;
    ++itDir;
    continue;
  }

  // Count the remaining dirs in itrDir and add ".." for each
  for ( ; itDir != endItDir; ++itDir )
  {
    if ( ( (*itDir).compare( "." ) == 0 )
      || ( (*itDir).compare( ".." ) == 0 ) )
    {
      break;
    }

    relativePath /= "..";
  }

  // Copy the rest of itrFile to the path
  for ( ; itFile != endItFile; ++itFile )
  {
    relativePath /= (*itFile);
  }

  relative = relativePath.string();
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::MakeRelative( std::string &filePath, const std::string &dirPath )
{
  std::string relative;
  MakeRelative( filePath, dirPath, relative );
  filePath = relative;
}


/////////////////////////////////////////////////////////////////////////////
//
bool FileSystem::IsRelative( const std::string& filePath )
{
  // matches a leading group of one or more back/forward slashes
  const boost::regex s_MatchDirectoryHead( "^(([a-zA-Z]{1}:)|([/]{2}))" );

  std::string serverPrefix = ""; 
  boost::smatch matchResult;
  if ( boost::regex_search( filePath, matchResult, s_MatchDirectoryHead ) )
  {
    const boost::regex s_MatchRelativeDir( "/[\\.]{1,2}/" );
    if ( boost::regex_search( filePath, matchResult, s_MatchRelativeDir ) )
    {
      return true;
    }

    return false;
  }

  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::GuaranteeSlash( std::string &path )
{
  if ( !path.empty() && *path.rbegin() != '/' )
  {
    path += "/";
  }
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::SplitPath( const std::string &filePath, V_string &decomposed )
{
  bool unc = false;
  if ( filePath.size() > 2 && filePath[0] == '/' && filePath[1] == '/' )
  {
    unc = true;
  }

  Tokenize( filePath, decomposed, "/" );

  if (unc)
  {
    decomposed.front().insert(0, "//");
  }
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::ComposePath( const V_string &decomposed, std::string &filePath )
{
  filePath.clear();

  V_string::const_iterator itr = decomposed.begin();
  V_string::const_iterator end = decomposed.end();
  for ( ; itr != end; ++itr )
  {
    if ( !filePath.empty() && *filePath.rbegin() != '/' && !itr->empty() && *itr->begin() != '/' )
    {
     filePath += "/";
    }

    filePath += *itr;
  }
}


/////////////////////////////////////////////////////////////////////////////
//

void FileSystem::AppendPath( std::string &filePath, const std::string &addPath )
{
  GuaranteeSlash( filePath );
  filePath += addPath;
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::GetLeaf( const std::string &filePath, std::string &leaf )
{
  leaf = filePath;
  const boost::regex s_MatchLeafPattern( ".*?[\\\\/]+([^\\\\/]+)[\\\\/]*$", boost::match_default|boost::regex::icase );
  leaf = boost::regex_replace(leaf, s_MatchLeafPattern, "$1");
}


/////////////////////////////////////////////////////////////////////////////
//
std::string FileSystem::GetLeaf( const std::string &filePath )
{
  std::string leaf;
  GetLeaf( filePath, leaf );
  return leaf;
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::StripLeaf( std::string &filePath )
{
  const boost::regex s_StripLeafPattern( "([\\\\/]+)([^\\\\/]+[\\\\/]*)$", boost::match_default|boost::regex::icase );
  filePath = boost::regex_replace(filePath, s_StripLeafPattern, "$1");
}


/////////////////////////////////////////////////////////////////////////////
//
bool FileSystem::HasPrefix( const std::string& prefix, const std::string& path )
{
  return ( path.find( prefix ) == 0 );
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::StripPrefix( const std::string& prefix, std::string& path )
{
  if ( path.find( prefix ) != path.npos )
    path.erase( 0, prefix.length() );
}


/////////////////////////////////////////////////////////////////////////////
//
std::string FileSystem::StripPrefix( const std::string& prefix, const char *path )
{
  std::string newPath = path;
  StripPrefix( prefix, newPath );
  return newPath;
}



//*************************************************************************//
//
// FILE FUNCTIONS
//
//*************************************************************************//


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::Copy( const std::string& source, const std::string& destination, bool overwrite )
{
  if (FALSE == ::CopyFile( source.c_str(), destination.c_str(), overwrite ? FALSE : TRUE ))
  {
    throw Windows::Exception ( "Unable to copy file '%s' to '%s'", source.c_str(), destination.c_str() ); // will capture GetLastError();
  }
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::Move( const std::string& source, const std::string& destination )
{
  if (FALSE == ::MoveFile( source.c_str(), destination.c_str() ))
  {
    throw Windows::Exception ( "Unable to rename file '%s' to '%s'", source.c_str(), destination.c_str() ); // will capture GetLastError();
  }
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::Delete( const std::string& fileName )
{
  std::string clean( fileName ); 
  Win32Name( clean ); 

  if (Exists(fileName))
  {
    if (FALSE == ::DeleteFileA( clean.c_str() ))
    {
      throw Windows::Exception ( "Unable to delete file '%s'", clean.c_str() ); // will capture GetLastError();
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
//

void FileSystem::AppendFile( std::ofstream& dst, std::ifstream& src )
{
  if( !dst.is_open() || !src.is_open() )
  {
    throw Windows::Exception( "Unable to append file. Src or Dst stream is not valid" );
  }

  const int MAX_FILE_BUFFER_SIZE = 690;

  char  buffer[MAX_FILE_BUFFER_SIZE];
  int   bytes;

  src.seekg( 0, std::ios_base::end );
  bytes = src.tellg();
  src.seekg( 0, std::ios_base::beg );

  while( (bytes > 0) && !src.eof() )
  {
    int readSize = std::min( bytes, MAX_FILE_BUFFER_SIZE );

    memset( buffer, 0, MAX_FILE_BUFFER_SIZE );

    src.read( buffer,  readSize );
    dst.write( buffer, readSize );

    bytes -= readSize;
  }
}


//*************************************************************************//
//
// DIRECTORY PATH FUNCTIONS
//
//*************************************************************************//


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::MakePath( const std::string& path, const bool stripFileName )
{
  FILESYSTEM_SCOPE_TIMER((""));

  std::string stripped = path;

  if ( stripFileName == true )
  {
    std::string::size_type lastSlash = stripped.rfind( '/' ); 
    if( lastSlash == std::string::npos )
    {
      // this handles bare file names: 'file.txt'
      return; 
    }

    StripLeaf( stripped );
  }


  if ( !stripped.empty() )
  {
    if ( !FileSystem::Exists( stripped ) )
    {
      bool createdDir = false;
      BoostFS::path dirPath = stripped;
      try 
      {
        createdDir = BoostFS::create_directories( dirPath );
      }
      catch ( const boost::filesystem::filesystem_error& e )
      {
        throw FileSystem::Exception( "MakePath failed: %s", e.what() );
      }
      if ( !createdDir )
      {
        throw FileSystem::Exception( "MakePath failed to create directory path: %s", dirPath.string().c_str() );
      }
    }
    else if ( !FileSystem::IsFolder( stripped ) )
    {
      throw FileSystem::Exception( "MakePath failed to create directory path, file already exists: %s", stripped.c_str() );
    }
  }
}


/////////////////////////////////////////////////////////////////////////////
//
int FileSystem::Find
(
 const std::string &dirPath,
 V_string &listOfFiles,
 const std::string &spec,
 const FileFindFlags flags,
 const FileIteratorFlags itrFlags,
 const i32 recurseDepth,
 const i32 curDepth
 )
{
  FILESYSTEM_SCOPE_TIMER((""));
  int numFilesFound = 0;

  try
  {
    FileIterator fileIter( dirPath, spec, itrFlags );

    while ( !fileIter.IsDone() )
    {
      listOfFiles.push_back( fileIter.Item() );
      ++numFilesFound;
      fileIter.Next();
    }

    // Recursive: Search sub-directories recursively
    if ( ( flags & FindFlags::Recursive )
      && ( recurseDepth < 0 || recurseDepth > curDepth ) )
    {
      FileIterator dirIter( dirPath, "*.*", IteratorFlags::NoFiles );

      while ( !dirIter.IsDone() )
      {
        numFilesFound += Find( dirIter.Item(), listOfFiles, spec, flags, itrFlags, recurseDepth, curDepth+1 );
        dirIter.Next();
      }
    }
  }
  catch( ... )
  {
    // NoThrowEx: don't throw ANY exceptions
    if ( !( flags & FindFlags::NoThrowEx ) )
    {
      throw;
    }
  }

  // Note: The sort and return are AFTER the try-catch so that: if an exception
  // is thrown by FileIterator but caught and skipped by the catch block,
  // we still sort the files that got pushed before the exception was thrown.

  // sort the result vector unless NoSort is specified.
  if ( curDepth == 0 && !( flags & FindFlags::NoSort ) )
  {
    //sort( listOfFiles.begin(), listOfFiles.end() );
    SortFiles( listOfFiles );
  }

  return numFilesFound;
}


/////////////////////////////////////////////////////////////////////////////
//
int FileSystem::GetFiles
(
 const std::string &dirPath,
 V_string &listOfFiles,
 const std::string &spec,
 const FileFindFlags flags,
 const FileIteratorFlags itrFlags,
 const i32 recurseDepth
 )
{
  FILESYSTEM_SCOPE_TIMER((""));
  return Find( dirPath, listOfFiles, spec, flags, ( FileIteratorFlags ) ( ( itrFlags & ~IteratorFlags::NoFiles ) | IteratorFlags::NoDirs ), recurseDepth );
}


/////////////////////////////////////////////////////////////////////////////
//
int FileSystem::GetDirectories
(
 const std::string &dirPath,
 V_string &listOfFiles,
 const std::string &spec,
 const FileFindFlags flags,
 const FileIteratorFlags itrFlags,
 const i32 recurseDepth
 )
{
  FILESYSTEM_SCOPE_TIMER((""));
  return Find( dirPath, listOfFiles, spec, flags, ( FileIteratorFlags ) ( ( itrFlags & ~IteratorFlags::NoDirs ) | IteratorFlags::NoFiles ), recurseDepth );
}


/////////////////////////////////////////////////////////////////////////////
// return true if the lhs comes before rhs
bool FileSystem::CompareFilePath( const std::string& lhs, const std::string& rhs )
{
  V_string lhsTokens, rhsTokens;
  Tokenize( lhs, lhsTokens, "/" );
  Tokenize( rhs, rhsTokens, "/" );

  int lhsSize = (int) lhsTokens.size() - 1;
  int rhsSize = (int) rhsTokens.size() - 1;
  
  if ( rhsSize == 0 )
    return false;

  if ( lhsSize == 0 )
    return true;

  if ( lhsSize == rhsSize )
  {
    return ( strinatcmp( lhs.c_str(), rhs.c_str() ) < 0 );
  }

  int lhsIndex = 0;
  int rhsIndex = 0;
  for ( ; lhsIndex <= lhsSize ; ++lhsIndex, ++rhsIndex )
  {
    // if either are on their leaf node, return true if lhs is shorter
    if ( lhsIndex >= lhsSize )
    {
      return true;
    }
    // we ran out of rhsTokens, it is shorter than the lhs
    else if ( rhsIndex >= rhsSize )
    {
      return false;
    }

    // while the tokens are the same proceed down the path, otherwise compare the nodes
    int compare = strinatcmp( lhsTokens[lhsIndex].c_str(), rhsTokens[rhsIndex].c_str() );
    if ( compare != 0 )
    {
      return ( compare < 0 );
    }
  }

  // we ran out of lhsTokens
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
bool FileSystem::ReverseCompareFilePath( const std::string& lhs, const std::string& rhs )
{
  return !CompareFilePath( lhs, rhs );
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::SortFiles( V_string &listOfFiles, bool reverse )
{
  if ( !reverse )
  {
    std::sort( listOfFiles.begin(), listOfFiles.end(), CompareFilePath );
  }
  else
  {
    std::sort( listOfFiles.begin(), listOfFiles.end(), ReverseCompareFilePath );
  }
}



//*************************************************************************//
//
// FILE ATTRIBUTE FUNCTIONS
//
//*************************************************************************//

std::string MakeStatSafePath( const std::string& filePath )
{
  std::string clean( filePath );
  FileSystem::Win32Name( clean );
  while ( !clean.empty() && *clean.rbegin() == '\\' )
  {
    clean.erase( clean.length() - 1 );
  }
  if ( !clean.empty() && *clean.rbegin() == ':' )
  {
    clean += '\\';
  }
  return clean;
}



/////////////////////////////////////////////////////////////////////////////
//
bool FileSystem::GetStats( const std::string& filePath, struct _stat &fileStats )
{
  FILESYSTEM_SCOPE_TIMER((""));
  return _stat( filePath.c_str(), &fileStats ) == 0;
}


/////////////////////////////////////////////////////////////////////////////
//
bool FileSystem::GetStats64( const std::string& filePath, struct _stati64 &fileStats )
{
  FILESYSTEM_SCOPE_TIMER((""));
  return _stati64( filePath.c_str(), &fileStats ) == 0;
}


/////////////////////////////////////////////////////////////////////////////
// Returns zero if the file does not exist (or is empty).  Otherwise returns
// the file size in bytes.
// 
u64 FileSystem::GetSize( const std::string& fileName )
{
  struct _stati64 fileStats;
  if ( GetStats64( fileName, fileStats ) )
  {
    return fileStats.st_size;
  }
  return 0;
}


/////////////////////////////////////////////////////////////////////////////
//
bool FileSystem::Exists( const std::string& path )
{
  return ( GetFileAttributes( path.c_str() ) != INVALID_FILE_ATTRIBUTES );
}


/////////////////////////////////////////////////////////////////////////////
// Returns true if the file is not readonly.
// 
bool FileSystem::IsWritable( const std::string& path )
{
  u32 attributes;
  GetAttributes( path, attributes );

  return !( attributes & FILE_ATTRIBUTE_READONLY );
}


/////////////////////////////////////////////////////////////////////////////
// Returns true if the specified path is a file.
// 
bool FileSystem::IsFile( const std::string& path )
{
  const u32 attribs = GetFileAttributes( path.c_str() );
  return ( attribs != INVALID_FILE_ATTRIBUTES ) && ( !( attribs & FILE_ATTRIBUTE_DIRECTORY ) );
}


/////////////////////////////////////////////////////////////////////////////
// Returns true if the specified path is a folder.
// 
bool FileSystem::IsFolder( const std::string& path )
{
  const u32 attribs = GetFileAttributes( path.c_str() );
  return ( attribs != INVALID_FILE_ATTRIBUTES ) && ( attribs & FILE_ATTRIBUTE_DIRECTORY );
}


/////////////////////////////////////////////////////////////////////////////
// used internally for getting file info
//
bool GetTranslationId(LPVOID lpData, UINT unBlockSize, WORD wLangId, DWORD &dwId, bool bPrimaryEnough/*= FALSE*/)
{
  LPWORD lpwData;
  for (lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData)+unBlockSize; lpwData+=2)
  {
    if (*lpwData == wLangId)
    {
      dwId = *((DWORD*)lpwData);
      return TRUE;
    }
  }

  if (!bPrimaryEnough)
    return FALSE;

  for (lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData)+unBlockSize; lpwData+=2)
  {
    if (((*lpwData)&0x00FF) == (wLangId&0x00FF))
    {
      dwId = *((DWORD*)lpwData);
      return TRUE;
    }
  }

  return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// Determine if OS can retrieve version info, and get the version info size
// used internally for getting file info
//
DWORD GetFileDataSize( const std::string &fileName )
{
  DWORD	dwHandle;
  DWORD fileDataSize = GetFileVersionInfoSize( ( LPTSTR ) fileName.c_str(), ( LPDWORD ) &dwHandle );
  return fileDataSize;
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::GetWinVersionInfo( const std::string& filePath, std::string &winVersion )
{
  DWORD fileDataSize = GetFileDataSize( filePath );

  // file has no version info in this case
  if ( fileDataSize <= 0 )
    return;

  LPVOID fileData = ( LPVOID ) malloc ( fileDataSize );
  if ( fileData == NULL )
  {
    throw FileSystem::Exception( "Call to malloc failed, insufficient memory available." );
  }

  // get the version info
  DWORD	dwHandle = 0;
  if ( GetFileVersionInfo( ( LPTSTR ) filePath.c_str(), dwHandle, fileDataSize, fileData ) )
  {
    // catch default information
    LPVOID lpInfo;
    UINT unInfoLen;
    VerQueryValue( fileData, "\\", &lpInfo, &unInfoLen );

    // find best matching language and codepage
    VerQueryValue( fileData, "\\VarFile\\Translation", &lpInfo, &unInfoLen );

    DWORD dwLangCode = 0;
    if ( !GetTranslationId(lpInfo, unInfoLen, GetUserDefaultLangID(), dwLangCode, FALSE ) )
    {
      if ( !GetTranslationId(lpInfo, unInfoLen, GetUserDefaultLangID(), dwLangCode, TRUE ) )
      {
        if ( !GetTranslationId(lpInfo, unInfoLen, MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL ), dwLangCode, TRUE ) )
        {
          if ( !GetTranslationId(lpInfo, unInfoLen, MAKELANGID( LANG_ENGLISH, SUBLANG_NEUTRAL ), dwLangCode, TRUE ) )
          {
            // use the first one we can get
            dwLangCode = *( ( DWORD* ) lpInfo );
          }
        }
      }
    }

    char key[64];
    sprintf_s(
      key,
      64,
      "\\StringFile\\%04X%04X\\FileVersion",
      ( dwLangCode & 0x0000FFFF ),
      ( dwLangCode & 0xFFFF0000 ) >> 16 );

    if ( VerQueryValue( fileData, ( LPTSTR ) key, &lpInfo, &unInfoLen ) )
    {
      winVersion = ( LPCTSTR ) lpInfo;
    }
  }

  free( fileData );
}


/////////////////////////////////////////////////////////////////////////////
//
void FileSystem::GetAttributes( const std::string& path, u32& attributes )
{
  FILESYSTEM_SCOPE_TIMER((""));

  DWORD dwFileAttrs = GetFileAttributes( path.c_str() ); 
  if ( dwFileAttrs == INVALID_FILE_ATTRIBUTES )
  {
    throw Windows::Exception(
      "GetAttributesFast failed, GetFileAttributes returned INVALID_FILE_ATTRIBUTES for path '%s'!",
      path.c_str() );
  }

  attributes = dwFileAttrs;
}


/////////////////////////////////////////////////////////////////////////////
// Checks to see that the file has the given attribute(s)
//
// Note: Use this function to check for the presance of a given attribute
// (e.g., FILE_ATTRIBUTE_READONLY ); but NOT to check for the absence of an
// attribute (e.g., ~FILE_ATTRIBUTE_READONLY) because it is not valid binary
// logic: ( X & ~Y ) is NOT [always] the same as !( X & Y )
//
bool FileSystem::HasAttribute( const std::string& filePath, const u32 attributes )
{
  u32 dwFileAttrs;
  GetAttributes( filePath, dwFileAttrs );

  // *See note above
  if ( dwFileAttrs & attributes )
  {
    return true;
  }
  else
  {
    return false;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
bool FileSystem::UpdatedSince( const std::string& filePath, const __time64_t updateTime )
{
  struct _stati64 fileStats;
  if ( GetStats64( filePath, fileStats ) )
  {
    return ( fileStats.st_mtime > updateTime ) ? true : false ;
  }

  // File is newer because it didn't exist before
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
int FileSystem::CompareModified( const std::string& lhsFileName, const std::string& rhsFileName )
{
  struct _stati64 lhsFileStats;
  bool lhsValid = GetStats64( lhsFileName, lhsFileStats );

  struct _stati64 rhsFileStats;
  bool rhsValid = GetStats64( rhsFileName, rhsFileStats );

  if ( rhsValid && lhsValid )
  {
    if ( lhsFileStats.st_mtime == rhsFileStats.st_mtime )
    {
      // mod times are equal
      return 0;
    }
    else if ( lhsFileStats.st_mtime > rhsFileStats.st_mtime )
    {
      // lhs is newer
      return 1;
    }
    else
    {
      // rhs is newer
      return -1;
    }
  }
  else if ( lhsValid )
  {
    // lhs is newer because rhs doesn't exist
    return 2;
  }
  else if ( rhsValid )
  {
    // rhs is newer because lhs doesn't exist
    return -2;
  }
  else
  {
    // neither file exists
    return -3;
  }
}


/////////////////////////////////////////////////////////////////////////////
//
bool FileSystem::ArePathsEquivalent( const std::string& lhsPath, const std::string& rhsPath )
{
  return BoostFS::equivalent( lhsPath, rhsPath );
}



//*************************************************************************//
//
// FILE EXTENSION FUNCTIONS
//
//*************************************************************************//


/////////////////////////////////////////////////////////////////////////////
// ensure that the extension has a leading '.'
void FileSystem::GuaranteeDot( std::string& extension )
{
  if ( !extension.empty() && *extension.begin() != '.' )
    extension.insert( 0, "." );
}


/////////////////////////////////////////////////////////////////////////////
// Sets the fileName extension to the given one.
//
void FileSystem::SetExtension( std::string& fileName, const std::string &extension, int numberOfDots )
{
  std::string newFileName = fileName;
  StripExtension( newFileName, numberOfDots );    

  // ensure that the extension has a leading '.'
  std::string newExtension = extension;
  GuaranteeDot( newExtension );

  newFileName += newExtension;
  fileName = newFileName;
}


/////////////////////////////////////////////////////////////////////////////
// Sets the fileName extension to the given one.
//
void FileSystem::AppendExtension( std::string& fileName, const std::string &extension )
{
  // ensure that the extension has a leading '.'
  std::string newExtension = extension;
  GuaranteeDot( newExtension );
  fileName += newExtension;
}


/////////////////////////////////////////////////////////////////////////////
// Sets the fileName extension to the given one.
//
void FileSystem::InsertExtension( std::string& fileName, const std::string &extension )
{
  // extract the existing extension (.wav, etc...)
  std::string oldExtension;
  FileSystem::GetExtension(fileName, oldExtension, 1);

  // strip all the extensions off
  FileSystem::StripExtension(fileName, 1);

  // append our generated extension
  FileSystem::AppendExtension(fileName, extension);

  // append our audio extension (.wav, etc...)
  FileSystem::AppendExtension(fileName, oldExtension);
}


/////////////////////////////////////////////////////////////////////////////
// There are three possible uses:
// no extension passed in -> remove the current extension
// given extension and file extension match -> remove it
// given extension and file extension don't match -> do nothing
//
void FileSystem::StripExtension( std::string &fileName, int numberOfDots )
{
  if ( numberOfDots == 0 )
    return;

  const char* s_StripExtensionsFormat = "(\\.[^\\\\/\\. ]+?){1,%d}$";

  // create the regex for removing the extension
  char searchFreq[100];
  if ( numberOfDots <= AllExtensions )
  {
    sprintf( searchFreq, s_StripExtensionsFormat, "" );
  }
  else
  {
    sprintf( searchFreq, s_StripExtensionsFormat, numberOfDots );
  }

  boost::regex stripExtPattern(searchFreq, boost::regex::perl|boost::regex::icase); 
  fileName = boost::regex_replace(fileName, stripExtPattern, ""); 
}


/////////////////////////////////////////////////////////////////////////////
// get the current extension
void FileSystem::GetExtension( const std::string &fileName, std::string &extension, int numberOfDots )
{
  extension = "";

  if ( numberOfDots == 0 )
    return;

  const char* s_GetExtensionsFormat = "^(.*?)((\\.[^\\\\/\\. ]+?){1,%d})$";

  // create the regex for removing the extension
  char searchFreq[100];
  if ( numberOfDots <= AllExtensions )
  {
    sprintf( searchFreq, s_GetExtensionsFormat, "" );
  }
  else
  {
    sprintf( searchFreq, s_GetExtensionsFormat, numberOfDots );
  }

  boost::regex  matchExtPattern(searchFreq, boost::regex::icase);
  boost::smatch matchResults; 

  if (boost::regex_match(fileName, matchResults, matchExtPattern))
  {
    extension = ResultAsString(matchResults, 2); 
  }
}


/////////////////////////////////////////////////////////////////////////////
//
std::string FileSystem::GetExtension( const std::string &fileName, int numberOfDots )
{
  std::string extension;
  GetExtension( fileName, extension, numberOfDots );
  return extension;
}


/////////////////////////////////////////////////////////////////////////////
// Escape all '.' characters in a string so that it can be used in a regex
inline void EscapeDots( std::string& path )
{
  //std::string testPath = path;
  boost::regex dot("\\."); 
  path = boost::regex_replace( path, dot, "\\\\." ); 
}


/////////////////////////////////////////////////////////////////////////////
// no extension passed in
// given extension and file extension match
// given extension and file extension don't match
bool FileSystem::HasExtension( const std::string &fileName, const std::string &extension )
{
  if ( !extension.empty() )
  {
    std::string searchExt = extension;
    EscapeDots( searchExt );
    searchExt = "^(.*?)(" + searchExt + ")$";

    boost::regex  matchExtension(searchExt, boost::regex::icase); 
    boost::smatch matchResults; 
    return boost::regex_match(fileName, matchResults, matchExtension); 
  }
  else
  {
    std::string newFileName = fileName;

    // get the current extension
    std::string::size_type leafPos = newFileName.rfind( '/' );
    leafPos = ( ( leafPos != std::string::npos ) ? leafPos : 0 );

    // find the position of the extension
    std::string::size_type extnPos = newFileName.rfind( '.' );
    extnPos = ( ( extnPos > leafPos ) ? extnPos : std::string::npos );

    return ( extnPos != std::string::npos );
  }
}
