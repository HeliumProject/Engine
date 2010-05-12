#include "AppUtils/AppUtils.h"
#include "AssetBuilder/CacheFiles.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/File.h"
#include "Common/InitializerStack.h"
#include "Common/File/Directory.h"
#include "Common/String/Units.h"
#include "Console/Console.h"
#include "Windows/Windows.h"
#include "Windows/Error.h"

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
  std::string directory = item.m_Path;
  FileSystem::StripLeaf( directory );

  V_string files;
  FileSystem::Find( directory, files );

  for ( V_string::const_iterator itr = files.begin(), end = files.end(); itr != end; ++itr )
  {
    const std::string& file = *itr;

    if ( !DeleteFile( file.c_str() ) )
    {
      Console::Warning( "Failed to delete file: %s\t\treason%s:\n", file.c_str(), Windows::GetErrorString().c_str() );

      return false;
    }
  }

  if ( !RemoveDirectory( directory.c_str() ) )
  {
    Console::Warning( "Failed to delete directory: %s\t\treason: %s\n", directory.c_str(), Windows::GetErrorString().c_str() );

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
        Console::Print("%d found\n", lastSize );
      }
    }

    std::vector< Nocturnal::DirectoryItem > m_Files;
    u64 m_Usage;
  };

  CacheFileFinder finder;

  {
    Console::Bullet bullet ( "Finding files...\n" );

    finder.Find( path );
  }

  std::string usage = Nocturnal::BytesToString( finder.m_Usage );
  Console::Print( "Found %d files using %s\n", finder.m_Files.size(), usage.c_str() );

  Console::Print( "Sorting by last modified time...\n" );
  std::sort( finder.m_Files.begin(), finder.m_Files.end(), &ItemCompare );

  u64 storageUsed = 0;
  u32 deletedFiles = 0;
  u64 freedSpace = 0;

  Console::Print( "Deleting...\n" );
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
    Console::Print( "Deleted %d files, freeing %s of space\n", deletedFiles, freed.c_str() );
  }
  else
  {
    std::string max = Nocturnal::BytesToString( maxStorageUsed );
    Console::Print( "Current usage lower than %s, no files were deleted\n", max.c_str() );
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
    FileSystem::GuaranteeSlash( path );
  }

  if ( argc < 2 || path.empty() )
  {
    Console::Print( "Usage: CleanupCacheFiles <storage space to trim down to in gigabytes> [path]\n" );
    return 1;
  }

  u64 sizeInBytes = (u64)atol( argv[ 1 ] ) << 30;

  std::string size = Nocturnal::BytesToString( sizeInBytes );
  Console::Bullet bullet ( "Trimming %s to %s\n", path.c_str(), size.c_str() );
  CleanupCacheData( path, sizeInBytes );

  return 0;
}

int main(int argc, const char** argv)
{
  return AppUtils::StandardMain( &Main, argc, argv );
}
