#pragma once

#include "API.h"
#include "Exceptions.h"
#include "FileIterator.h"
#include "Common/Types.h"

#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <fstream>

namespace FileSystem
{ 
  namespace FindFlags
  {
    enum Flags
    {
      Default       = 0,
      NoThrowEx     = 1 << 0,          // Catch all exceptions and return empty/partial data
      NoSort        = 1 << 1,          // Sort the result vector
      Recursive     = 1 << 2           // Search sub-directories recursively
    };
  }
  typedef u32 FileFindFlags;


  //
  // Clean name
  //

  FILESYSTEM_API void         CleanName( const std::string &dirtyName, std::string &cleanName, const bool lower = true );
  FILESYSTEM_API void         CleanName( std::string &fileName, const bool lower = true );

  FILESYSTEM_API void         Win32Name( const std::string &dirtyName, std::string &win32Name, const bool lower = true );
  FILESYSTEM_API void         Win32Name( std::string &fileName, const bool lower = true );

  FILESYSTEM_API void         MakeAbsolute( const std::string &filePath, std::string &absFilePath );
  FILESYSTEM_API void         MakeAbsolute( std::string &filePath );

  FILESYSTEM_API void         MakeRelative( const std::string &filePath, const std::string &dirPath, std::string &relativePath );
  FILESYSTEM_API void         MakeRelative( std::string &filePath, const std::string &dirPath );

  FILESYSTEM_API bool         IsRelative( const std::string& filePath );


  //
  // Modify path
  // 

  FILESYSTEM_API void         GuaranteeSlash( std::string &path );

  FILESYSTEM_API void         SplitPath( const std::string &filePath, V_string &decomposed );
  FILESYSTEM_API void         ComposePath( const V_string &decomposed, std::string &filePath );
  FILESYSTEM_API void         AppendPath( std::string &filePath, const std::string &addPath );
  FILESYSTEM_API void         GetLeaf( const std::string &filePath, std::string &leaf );
  FILESYSTEM_API std::string  GetLeaf( const std::string &filePath );
  FILESYSTEM_API void         StripLeaf( std::string &filePath );

  FILESYSTEM_API bool         HasPrefix( const std::string& prefix, const std::string& path );
  FILESYSTEM_API void         StripPrefix( const std::string& prefix, std::string& path );
  FILESYSTEM_API std::string  StripPrefix( const std::string& prefix, const char *path );


  //
  // File functions
  //

  FILESYSTEM_API void         Copy( const std::string& source, const std::string& destination, bool overwrite = true );
  FILESYSTEM_API void         Move( const std::string& source, const std::string& destination );
  FILESYSTEM_API void         Delete( const std::string& fileName );

  FILESYSTEM_API void         AppendFile( std::ofstream& dst, std::ifstream& src );

  //
  // Directory functions
  //

  FILESYSTEM_API void         MakePath( const std::string &path, const bool stripFileName = false );
  FILESYSTEM_API int          Find( const std::string &dirPath, V_string &listOfFiles, const std::string &spec = "*.*", const FileFindFlags flags = FindFlags::Default, const FileIteratorFlags itrFlags = IteratorFlags::Default, const i32 recurseDepth = -1, const i32 curDepth = 0 );
  FILESYSTEM_API int          GetFiles( const std::string &dirPath, V_string &listOfFiles, const std::string &spec = "*.*", const FileFindFlags flags = FindFlags::Default, const FileIteratorFlags itrFlags = IteratorFlags::Default, const i32 recurseDepth = -1 );
  FILESYSTEM_API int          GetDirectories( const std::string &dirPath, V_string &listOfFiles, const std::string &spec = "*.*", const FileFindFlags flags = FindFlags::Default, const FileIteratorFlags itrFlags = IteratorFlags::Default, const i32 recurseDepth = -1 );

  FILESYSTEM_API bool         CompareFilePath( const std::string& lhs, const std::string& rhs );
  FILESYSTEM_API bool         ReverseCompareFilePath( const std::string& lhs, const std::string& rhs );

  FILESYSTEM_API void         SortFiles( V_string &listOfFiles, bool reverse = false );


  //
  // File Attributes
  //

  FILESYSTEM_API std::string   MakeStatSafePath( const std::string& filePath );
  FILESYSTEM_API bool         GetStats( const std::string &filePath, struct _stat &fileStats );
  FILESYSTEM_API bool         GetStats64( const std::string &filePath, struct _stati64 &fileStats );

  FILESYSTEM_API u64          GetSize( const std::string &fileName );
  FILESYSTEM_API bool         Exists( const std::string& path );
  FILESYSTEM_API bool         IsWritable( const std::string& path );
  FILESYSTEM_API bool         IsFile( const std::string& path );
  FILESYSTEM_API bool         IsFolder( const std::string& path );

  FILESYSTEM_API void         GetWinVersionInfo( const std::string &filename, std::string &winVersion );

  FILESYSTEM_API void         GetAttributes( const std::string& path, u32 &attributes );
  FILESYSTEM_API void         SetAttributes( const std::string& path, u32 attributes );
  FILESYSTEM_API void         ClearAttributes( const std::string& path, u32 attributes );
  FILESYSTEM_API bool         HasAttribute( const std::string& filePath, const u32 attributes );

  FILESYSTEM_API bool         UpdatedSince( const std::string& filePath, const __time64_t updateTime );

  FILESYSTEM_API void         SetLastModifiedToCurrentTime( const std::string& path );

  //
  // CompareModified Returns:
  //  *  0 if the times are equal
  //  *  1 if the LEFT hand side is newer
  //  * -1 if the RIGHT hand side is newer
  //

  FILESYSTEM_API int          CompareModified( const std::string &lhsFileName, const std::string &rhsFileName );

  FILESYSTEM_API bool         ArePathsEquivalent( const std::string &lhsPath, const std::string &rhsPath );


  //
  // File extensions
  //

  // used to strip/find/etc.. all extensions on a file.  
  static const int AllExtensions = -1;

  FILESYSTEM_API void         GuaranteeDot( std::string& extension );

  FILESYSTEM_API void         SetExtension( std::string& fileName, const std::string& extension, int numberOfDots = AllExtensions );
  FILESYSTEM_API void         AppendExtension( std::string& fileName, const std::string& extension );
  FILESYSTEM_API void         InsertExtension( std::string& fileName, const std::string& extension );
  FILESYSTEM_API void         StripExtension( std::string& fileName, int numberOfDots = AllExtensions );
  FILESYSTEM_API void         GetExtension( const std::string& fileName, std::string& extension, int numberOfDots = AllExtensions );
  FILESYSTEM_API std::string  GetExtension( const std::string& fileName, int numberOfDots = AllExtensions );
  FILESYSTEM_API bool         HasExtension( const std::string& fileName, const std::string& extension = "" );
}