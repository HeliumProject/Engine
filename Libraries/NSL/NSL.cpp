#include "Windows/Windows.h"
#include "NSL.h"

#include <algorithm>

#include "AppUtils/AppUtils.h"
#include "Finder/Finder.h"
#include "Finder/ProjectSpecs.h"

#include "Common/Environment.h"

#include "Profile/Profile.h"
#include "Console/Console.h"
#include "Windows/Error.h"

#define MAX_COPY_THREADS 50
#define MIN_FILES_PER_THREAD 10

namespace NSL
{
  std::string g_CachedStorageRoot;
  std::string g_CachedProcessedDir;

  struct CopyFileParams
  {
    V_string m_Sources;
    std::string m_TargetDirectory;
    bool m_UseTempFile;
    bool m_OnlyIfNewer;

    HANDLE m_FinishedEvent;
    HANDLE m_ThreadHandle;
  };

  DWORD WINAPI CopyThread( LPVOID lpParam )
  {
    CopyFileParams* params = static_cast< CopyFileParams* >( lpParam );

    V_string::iterator sourceIt = params->m_Sources.begin();
    V_string::iterator sourceEnd = params->m_Sources.end();
    for ( ; sourceIt != sourceEnd; ++sourceIt )
    {
      std::string newTarget;
      try
      {
        std::string& source = (*sourceIt);

        FileSystem::Win32Name( source );

        // figure out where we want to copy the file to
        newTarget = GetDestinationFile( source, params->m_TargetDirectory );

        CopyFile( source, newTarget, params->m_UseTempFile, params->m_OnlyIfNewer );
      }
      catch ( Nocturnal::Exception& ex )
      {
        Console::Warning( "Error copying file %s: %s\n", newTarget.c_str(), ex.what() );
      }
    }

    ::SetEvent( params->m_FinishedEvent );

    return TRUE;
  }

  std::string GetDestinationFile( const std::string& source, const std::string& targetDirectory )
  {
    std::string newTarget = targetDirectory;
    std::string relativeSource = source;

    std::string projectRoot = Finder::ProjectRoot();
    FileSystem::Win32Name( projectRoot );

    if ( relativeSource.find( projectRoot ) != std::string::npos )
    {
      relativeSource.erase( 0, projectRoot.length() );
    }
    else if ( relativeSource.find( g_CachedStorageRoot ) != std::string::npos )
    {
      relativeSource.erase( 0, g_CachedStorageRoot.length() );
    }
    else
    {
      Console::Warning( "File %s is not in the network storage or project root!", relativeSource.c_str() );
      return newTarget;
    }

    return newTarget + relativeSource;
  }

  inline void GuaranteeSlash( std::string& path )
  {
    if ( !path.empty() && *path.rbegin() != '\\' )
      path += "\\";
  }

  inline void GuaranteeNoSlash( std::string& path )
  {
    if ( !path.empty() && *path.rbegin() == '\\' )
    {
      path.erase( path.length() - 1 );
    }
  }

  const std::string& StorageRoot()
  {
    NSL_SCOPE_TIMER((""))

    if ( !g_CachedStorageRoot.empty() )
      return g_CachedStorageRoot;

    if ( !Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"PROJECT_NETWORKSTORAGE_ROOT", g_CachedStorageRoot ) )
      throw Nocturnal::Exception( NOCTURNAL_STUDIO_PREFIX"PROJECT_NETWORKSTORAGE_ROOT environment variable not set!" );

    if ( g_CachedStorageRoot.empty() )
      throw Nocturnal::Exception( NOCTURNAL_STUDIO_PREFIX"PROJECT_NETWORKSTORAGE_ROOT environment variable is empty!" );

    GuaranteeSlash( g_CachedStorageRoot );

    FileSystem::Win32Name( g_CachedStorageRoot );

    return g_CachedStorageRoot;
  }
  
  const std::string& RelativeProcessed()
  {
    if ( g_CachedProcessedDir.empty() )
    {
      g_CachedProcessedDir = Finder::ProjectAssetsBranch() +
                             FinderSpecs::Project::PROCESSED_FOLDER.GetRelativeFolder();
    }

    return g_CachedProcessedDir;
  }

  // Copy assumes that if targetPath
  void Copy( std::string sourcePath, std::string targetPath, bool useTempFile, std::string keyFile, bool onlyIfNewer )
  {
    NSL_SCOPE_TIMER((""))

    {
      NSL_SCOPE_TIMER(("Win32Names"))
      FileSystem::Win32Name( sourcePath );
      FileSystem::Win32Name( targetPath );
      FileSystem::Win32Name( keyFile );
    }
    
    // if we have a key file, use the timestamp on it to see if we should copy.
    if ( !keyFile.empty() )
    {
      std::string destinationKey = GetDestinationFile( keyFile, targetPath );
      if ( !SourceIsNewer( keyFile, destinationKey ) )
      {
        return;
      }
      else
      {
        CopyFile( keyFile, destinationKey, useTempFile, false );
      }
    }

    GuaranteeNoSlash( sourcePath );

    // if the source is a directory, get the list of files/directories under it and copy everything
    {
      NSL_SCOPE_TIMER(("GetFileAttributes/Recurse/CopyFile"))
      if ( ::GetFileAttributes( sourcePath.c_str() ) & FILE_ATTRIBUTE_DIRECTORY )
      {
        V_string files;
        // reserve an arbitrarily large number of spaces
        files.reserve( 10000 );

        try
        {
          FileSystem::Find( sourcePath, files, "*.*", 
            (FileSystem::FileFindFlags)( FileSystem::FindFlags::NoSort | FileSystem::FindFlags::Recursive ),
            (FileSystem::FileIteratorFlags)( FileSystem::IteratorFlags::NoDirs | FileSystem::IteratorFlags::NoClean ) );
        }
        catch ( Nocturnal::Exception& )
        {
          return;
        }
        
        GuaranteeSlash( targetPath );

        std::vector< CopyFileParams > params;
        params.resize( MAX_COPY_THREADS );

        int numThreads = 0;

        // split the files up into chunks to be copied in worker threads
        for ( int i = 0; i < MAX_COPY_THREADS && !files.empty(); ++i, ++numThreads )
        {
          CopyFileParams& param = params[ i ];
          param.m_OnlyIfNewer = onlyIfNewer;
          param.m_UseTempFile = useTempFile;

          param.m_TargetDirectory = targetPath;

          size_t numFiles = std::min<size_t>( files.size(), std::max<size_t>( MIN_FILES_PER_THREAD, files.size() / MAX_COPY_THREADS ) );
          if ( i == MAX_COPY_THREADS - 1 )
          {
            numFiles = files.size();
          }

          param.m_Sources.reserve( numFiles );
          param.m_Sources.insert( param.m_Sources.begin(), files.rbegin(), files.rbegin() + numFiles );

          files.erase( files.begin() + files.size() - numFiles, files.end() );

          char buf[256];
          sprintf( buf, "Copy Thread Finished %d", i );

          param.m_FinishedEvent = ::CreateEvent( NULL, TRUE, FALSE, buf );
          param.m_ThreadHandle = ::CreateThread( NULL, NULL, CopyThread, &param, NULL, NULL );
        }

        // wait for our threads to finish
        std::vector< CopyFileParams >::iterator threadIt = params.begin();
        std::vector< CopyFileParams >::iterator threadEnd = params.end();
        for ( ; threadIt != threadEnd && numThreads; ++threadIt, --numThreads )
        {
          CopyFileParams& param = (*threadIt);

          ::WaitForSingleObject( param.m_FinishedEvent, INFINITE );

          ::CloseHandle( param.m_FinishedEvent );
          ::CloseHandle( param.m_ThreadHandle );
        }
      }
      else
      {
        CopyFile( sourcePath, GetDestinationFile( sourcePath, targetPath ), useTempFile, onlyIfNewer );
      }
    }
  }

  void Copy( V_string& sourcePaths, const std::string& targetPath, bool useTempFile, const std::string& keyFile, bool onlyIfNewer )
  {
    NSL_SCOPE_TIMER((""))

    V_string::iterator pathIt = sourcePaths.begin();
    V_string::iterator pathEnd = sourcePaths.end();
    for (; pathIt != pathEnd; ++pathIt )
    {
      Copy( *pathIt, targetPath, useTempFile, keyFile, onlyIfNewer );
    }
  }

  void CopyToNetwork( std::string relativeSource, const std::string& keyFile, bool onlyIfNewer )
  {
    NSL_SCOPE_TIMER((""))

    FileSystem::Win32Name( relativeSource );
    std::string absoluteSource = relativeSource;

    std::string projectRoot = Finder::ProjectRoot();
    FileSystem::Win32Name( projectRoot );

    if ( relativeSource.find( projectRoot ) == std::string::npos )
    {
      absoluteSource = projectRoot + relativeSource;
    }

    std::string absoluteKey = keyFile;
    FileSystem::Win32Name( absoluteKey );
    if ( !absoluteKey.empty() )
    {
      if ( absoluteKey.find( projectRoot ) == std::string::npos )
      {
        absoluteKey = projectRoot + keyFile;
      }
    }

    FileSystem::Win32Name( absoluteKey );
    
    Copy( absoluteSource, StorageRoot(), true, absoluteKey, onlyIfNewer );
  }

  void CopyFromNetwork( const std::string& relativeSource, const std::string& keyFile, bool onlyIfNewer )
  {
    NSL_SCOPE_TIMER((""))

    std::string absoluteSource = relativeSource;

    FileSystem::Win32Name( absoluteSource );

    std::string storageRoot = StorageRoot();
    FileSystem::Win32Name( storageRoot );

    if ( absoluteSource.find( storageRoot ) == std::string::npos )
    {
      absoluteSource = storageRoot + absoluteSource;
    }

    std::string absoluteKey = keyFile;
    if ( !keyFile.empty() )
    {
      if ( keyFile.find( StorageRoot() ) == std::string::npos )
      {
        absoluteKey = StorageRoot() + keyFile;
      }
    }

    FileSystem::Win32Name( absoluteKey );

    Copy( absoluteSource, Finder::ProjectRoot(), true, absoluteKey, onlyIfNewer );
  }

  void CopyToNetwork( V_string& relativePaths, const std::string& keyFile, bool onlyIfNewer )
  {
    NSL_SCOPE_TIMER((""))

    V_string::iterator pathIt = relativePaths.begin();
    V_string::iterator pathEnd = relativePaths.end();
    for (; pathIt != pathEnd; ++pathIt )
    {
      CopyToNetwork( *pathIt, keyFile, onlyIfNewer );
    }
  }

  void CopyFromNetwork( V_string& relativePaths, const std::string& keyFile, bool onlyIfNewer )
  {
    NSL_SCOPE_TIMER((""))

    V_string::iterator pathIt = relativePaths.begin();
    V_string::iterator pathEnd = relativePaths.end();
    for (; pathIt != pathEnd; ++pathIt )
    {
      CopyFromNetwork( *pathIt, keyFile, onlyIfNewer );
    }
  }

  bool SourceIsNewer( const std::string& source, const std::string& target )
  {
    WIN32_FILE_ATTRIBUTE_DATA sourceStats, targetStats;

    memset( &sourceStats, 0, sizeof( sourceStats ) );
    memset( &targetStats, 0, sizeof( targetStats ) );

    {
      NSL_SCOPE_TIMER(("GetFileAttributesEx"))
      if ( !::GetFileAttributesEx( source.c_str(), GetFileExInfoStandard, &sourceStats ) )
      {
        throw FailedToGetAttributesException( source.c_str() );
      }
    }

    u64 sourceModified, targetModified;
    memcpy( &sourceModified, &sourceStats.ftLastWriteTime, sizeof( sourceModified ) );

    {
      NSL_SCOPE_TIMER(("GetFileAttributesEx"))
      if ( !::GetFileAttributesEx( target.c_str(), GetFileExInfoStandard, &targetStats ) )
      {
        targetModified = 0;
      }
      else
      {
        memcpy( &targetModified, &targetStats.ftLastWriteTime, sizeof( targetModified ) );
      }
    }

    return ( sourceModified > targetModified );
  }

  void CopyFile( std::string sourceFile, std::string targetFile, bool useTempFile, bool onlyIfNewer )
  {
    NSL_SCOPE_TIMER((""))

    GuaranteeNoSlash( sourceFile );
    GuaranteeNoSlash( targetFile );

    // if we're not forcing this copy, and the target is newer than (or equivalent to) the source, don't copy
    if ( onlyIfNewer && !SourceIsNewer( sourceFile, targetFile ) )
    {
      return;
    }

    // This can fail with an exception if another thread is attempting to make the same path at the same time.
    // That's fine, just continue if that happens.
    try
    {
      FileSystem::MakePath( targetFile, true );
    }
    catch ( ... )
    {
      std::string path = targetFile;
      FileSystem::StripLeaf( path );
      if ( !FileSystem::Exists( path ) )
      {
        throw;
      }
    }

    if ( useTempFile )
    {
      std::string tempTargetFile = targetFile + ".NSLtmp";
      {
        NSL_SCOPE_TIMER(("CopyFile"))
        if ( !::CopyFile( sourceFile.c_str(), tempTargetFile.c_str(), false ) )
        {
          throw FailedCopyException( sourceFile.c_str(), tempTargetFile.c_str(), Windows::GetErrorString().c_str() );
        }
      }

      SetFileAttributes( tempTargetFile.c_str(), FILE_ATTRIBUTE_NORMAL );
      
      {
        NSL_SCOPE_TIMER(("MoveFile"))
        if ( !::MoveFileEx( tempTargetFile.c_str(), targetFile.c_str(), MOVEFILE_REPLACE_EXISTING ) )
        {
          throw FailedCopyException( tempTargetFile.c_str(), targetFile.c_str(), Windows::GetErrorString().c_str() );
        }
      }
    }
    else
    {
      {
        NSL_SCOPE_TIMER(("CopyFile"))
        if ( !::CopyFile( sourceFile.c_str(), targetFile.c_str(), false ) )
        {
          throw FailedCopyException( sourceFile.c_str(), targetFile.c_str(), Windows::GetErrorString().c_str() );
        }
      }
    }

    SetFileAttributes( targetFile.c_str(), FILE_ATTRIBUTE_NORMAL );
  }
}