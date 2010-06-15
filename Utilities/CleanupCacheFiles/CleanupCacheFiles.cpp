#include "Platform/Windows/Windows.h"
#include "Foundation/Exception.h"

#include "Application/Application.h"
#include "AssetBuilder/CacheFiles.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/File/Directory.h"
#include "Foundation/String/Units.h"
#include "Foundation/Log.h"

#include <sstream>
#include <algorithm>

bool ItemCompare( const Nocturnal::DirectoryItem& lhs, const Nocturnal::DirectoryItem& rhs )
{
  if ( lhs.m_ModTime != rhs.m_ModTime )
  {
    return lhs.m_ModTime > rhs.m_ModTime;
  }

  return lhs.m_Path.compare( rhs.m_Path ) < 0;
}

bool DeleteCacheData( const Nocturnal::DirectoryItem& item )
{
    std::string directory = Nocturnal::Path( item.m_Path ).Directory();

    Nocturnal::S_Path files;
    Nocturnal::Directory::GetFiles( directory, files );

    for ( Nocturnal::S_Path::const_iterator itr = files.begin(), end = files.end(); itr != end; ++itr )
  {
      const Nocturnal::Path& file = *itr;

      if ( file.IsFile() && !file.Delete() )
    {
      Log::Warning( "Failed to delete file: %s\t\treason%s:\n", file.c_str(), Platform::GetErrorString().c_str() );

      return false;
    }
  }

  if ( !RemoveDirectory( directory.c_str() ) )
  {
    Log::Warning( "Failed to delete directory: %s\t\treason: %s\n", directory.c_str(), Platform::GetErrorString().c_str() );

    return false;
  }

  return true;
}

// in bytes
void CleanupCacheData( const std::string& path, u64 maxStorageUsed )
{
  struct CacheFileFinder
  {
    CacheFileFinder()
      : m_Usage ( 0x0 )
    {

    }

    void Find( const std::string &path )
    {
      Nocturnal::RecurseDirectories( Nocturnal::DirectoryItemSignature::Delegate ( this, &CacheFileFinder::Found ), path, "*.file", Nocturnal::DirectoryFlags::SkipDirectories );
    }

    void Found( const Nocturnal::DirectoryItem& item )
    {
      m_Files.push_back( item );
      m_Usage += item.m_Size;

      static size_t lastSize = 0;
      if ( m_Files.size() >= lastSize + 10000 )
      {
        lastSize = m_Files.size();
        Log::Print("%d found\n", lastSize );
      }
    }

    std::vector< Nocturnal::DirectoryItem > m_Files;
    u64 m_Usage;
  };

  CacheFileFinder finder;

  {
    Log::Bullet bullet ( "Finding files...\n" );

    finder.Find( path );
  }

  std::string usage = Nocturnal::BytesToString( finder.m_Usage );
  Log::Print( "Found %d files using %s\n", finder.m_Files.size(), usage.c_str() );

  Log::Print( "Sorting by last modified time...\n" );
  std::sort( finder.m_Files.begin(), finder.m_Files.end(), &ItemCompare );

  u64 storageUsed = 0;
  u32 deletedFiles = 0;
  u64 freedSpace = 0;

  Log::Print( "Deleting...\n" );
  for ( std::vector<Nocturnal::DirectoryItem>::const_iterator itr = finder.m_Files.begin(), end = finder.m_Files.end(); itr != end; ++itr )
  {
    const Nocturnal::DirectoryItem& item = *itr;

    if ( storageUsed + item.m_Size > maxStorageUsed )
    {
      if ( DeleteCacheData( item ) )
      {
        freedSpace += item.m_Size;
        ++deletedFiles;
      }
    }
    else
    {
      storageUsed += item.m_Size;
    }
  }

  if ( deletedFiles )
  {
    std::string freed = Nocturnal::BytesToString( freedSpace );
    Log::Print( "Deleted %d files, freeing %s of space\n", deletedFiles, freed.c_str() );
  }
  else
  {
    std::string max = Nocturnal::BytesToString( maxStorageUsed );
    Log::Print( "Current usage lower than %s, no files were deleted\n", max.c_str() );
  }
}

int Main(int argc, const char** argv)
{
  Nocturnal::InitializerStack stack;
  stack.Push( CacheFiles::Initialize, CacheFiles::Cleanup );

  std::string path = CacheFiles::GetCacheFilesPath();
  if ( argc > 2 )
  {
    path = argv[ 2 ];
    Nocturnal::Path::GuaranteeSlash( path );
  }

  if ( argc < 2 || path.empty() )
  {
    Log::Print( "Usage: CleanupCacheFiles <storage space to trim down to in gigabytes> [path]\n" );
    return 1;
  }

  u64 sizeInBytes = (u64)atol( argv[ 1 ] ) << 30;

  std::string size = Nocturnal::BytesToString( sizeInBytes );
  Log::Bullet bullet ( "Trimming %s to %s\n", path.c_str(), size.c_str() );
  CleanupCacheData( path, sizeInBytes );

  return 0;
}

int main(int argc, const char** argv)
{
  return Application::StandardMain( &Main, argc, argv );
}
