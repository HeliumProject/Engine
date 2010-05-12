#pragma once

#include "API.h"

#include "File/ManagedFile.h"

namespace File
{
  //
  // Print Files
  //
  namespace PrintFileFormats
  {
    enum PrintFileFormat
    {
      Default,
      BarePath,
      BareId,
      Verbose,
    };
  }
  typedef PrintFileFormats::PrintFileFormat PrintFileFormat;

  FILE_API void PrintFile( const File::ManagedFilePtr& file, PrintFileFormat printformat = PrintFileFormats::Default );
  FILE_API void PrintFiles( File::V_ManagedFilePtr& listOfFiles, PrintFileFormat printformat = PrintFileFormats::Default );

  //
  // Sort Files
  ///
  namespace SortFileMethods
  {
    enum SortFileMethod
    {
      Alphabetical,
      ID,
      CreatedBy,
      ModifiedBy,
    };
  }
  typedef SortFileMethods::SortFileMethod SortFileMethod;

  FILE_API void SortFiles( V_ManagedFilePtr &listOfFiles, SortFileMethod method = SortFileMethods::Alphabetical, bool reverse = false );

  //
  // Find and select files
  //
  FILE_API void  FindManagedFiles( const std::string& searchQuery, File::V_ManagedFilePtr& listOfFiles, const int maxMatches = -1, const std::string& extension = std::string(""), bool verbose = true );
  FILE_API int   DetermineAssetIndex( const File::V_ManagedFilePtr& listOfFiles, const char* operation = "work with" );//, std::ostream& stream );
  FILE_API bool  DetermineAssetIndicies( const File::V_ManagedFilePtr& listOfFiles, V_i32& indicies, const char* operation = "work with", bool selectOne = false );//, std::ostream& stream );


  //
  // Filter lists of files
  //
  /////////////////////////////////////////////////////////////////////////////
  // Used to remove Files from a V_ManagedFilePtr where the file path (or id)
  //  is in the given set.
  //  
  //  Example usage:
  //  V_ManagedFilePtr listOfFiles;
  //  listOfFiles.push_back( File( 123, 0, 0, 0, "/tmp/file1.txt" ) );
  //  listOfFiles.push_back( File( 234, 0, 0, 0, "/tmp/file2.txt" ) );
  //  listOfFiles.push_back( File( 345, 0, 0, 0, "/store/cache.txt" ) );
  //
  //  S_string removePaths;
  //  removePaths.insert( "/tmp/file1.txt" );
  //  removePaths.insert( "/tmp/file2.txt" );
  //
  //  // Remove all but "/store/cache.txt" from the listOfFiles
  //  V_ManagedFilePtr::iterator itEnd = remove_if( listOfFiles.begin(), listOfFiles.end(), FilterFilesByPath( removePaths ) );
  //  listOfFiles.erase( itEnd, listOfEvents.end() );
  //
  //  Previously AppliedPathFileFunctor
  //
  struct FILE_API FilterFilesByPath
  {
    S_string    m_Paths;      ///< the path to the file on disk

    // Filter by path
    FilterFilesByPath( const S_string filePaths ) : m_Paths( filePaths ){}
    bool operator()( const File::ManagedFilePtr& file )
    {
      return ( m_Paths.find( file->m_Path ) != m_Paths.end() );
    }
  };

  struct FILE_API FilterFilesByID
  {
    S_tuid      m_Ids;         ///< File's unique id

    // Filter by id
    FilterFilesByID( const S_tuid fileIds ) : m_Ids( fileIds ){}
    bool operator()( const File::ManagedFilePtr& file )
    {
      return ( m_Ids.find( file->m_Id ) != m_Ids.end() );
    }
  };
}