#include "Profile/Profile.h"

#include "Finder.h"
#include "Exceptions.h"

#include "Common/Assert.h"
#include "Common/Config.h"
#include "Common/Version.h"
#include "Common/Boost/Regex.h" 
#include "Common/Container/Insert.h"
#include "Common/Environment.h"

#include "AssetSpecs.h"
#include "ExtensionSpecs.h"
#include "FontSpecs.h"
#include "LunaSpecs.h"
#include "ProjectSpecs.h"

using Nocturnal::Insert; 

#include "Common/Environment.h"
#include "Common/Checksum/MurmurHash2.h"
#include "FileSystem/FileSystem.h"

using namespace Finder;


typedef std::map< std::string, const FileSpec* > FileSpecMap;
typedef std::map< std::string, const FinderSpec* > FinderSpecMap;

typedef std::map< std::string, const ModifierSpec* > FileExtenisionSpecMap;

// the master map of specs to use in the APIs below
FileSpecMap       g_FileSpecMap;
FinderSpecMap     g_FinderSpecMap;

FileExtenisionSpecMap   g_FileExtenisionSpecMap;

// the reference count for this module, we init when incr to 1, and clean when decr to 0
u32               g_RefCount = 0;


/////////////////////////////////////////////////////////////////////////////
inline void ThrowIfNotInitialized()
{
  if( g_RefCount < 1 ) throw NotInitializedException();
}

/////////////////////////////////////////////////////////////////////////////
void Finder::Initialize()
{
  if ( ++g_RefCount > 1 )
    return;

  FINDER_SCOPE_TIMER((""));

  {
    // init all the globally registered FileSpecs
    FileSpec* current = FileSpec::s_LinkedListHead;
    while ( current )
    {
      AddFileSpec( *current );
      current = current->m_NextSpec;
    }
    FileSpec::s_LinkedListHead = NULL;
  }

  {
    // init all the globally registered ExtensionSpecs
    ModifierSpec* current = ModifierSpec::s_LinkedListHead;
    while ( current )
    {
      AddFileExtensionSpec( *current );
      AddFinderSpec( *current );
      current = current->m_NextSpec;
    }
    ModifierSpec::s_LinkedListHead = NULL;
  }

  {
    // init all the globally registered ExtensionSpecs
    FolderSpec* current = FolderSpec::s_LinkedListHead;
    while ( current )
    {
      AddFinderSpec( *current );
      current = current->m_NextSpec;
    }
    FolderSpec::s_LinkedListHead = NULL;
  }

  {
    // init all the globally registered ExtensionSpecs
    FilterSpec* current = FilterSpec::s_LinkedListHead;
    while ( current )
    {
      AddFinderSpec( *current );
      current = current->m_NextSpec;
    }
    FilterSpec::s_LinkedListHead = NULL;
  }

  // call init functions for filespec namespaces that still need it
  FinderSpecs::Asset::Init();
  FinderSpecs::Extension::Init();
  FinderSpecs::Font::Init();
  FinderSpecs::Luna::Init();
}

/////////////////////////////////////////////////////////////////////////////
void Finder::Cleanup()
{
  if ( --g_RefCount )
    return;

  FINDER_SCOPE_TIMER((""));

  g_FileSpecMap.clear();
  g_FinderSpecMap.clear();
  g_FileExtenisionSpecMap.clear();

  g_RefCount = 0;
}

/////////////////////////////////////////////////////////////////////////////
void Finder::AddFileSpec( const FileSpec &spec )
{
  FINDER_SCOPE_TIMER((""));

  NOC_ASSERT( g_FileSpecMap.find( spec.GetName() ) == g_FileSpecMap.end() );

  Insert<FileSpecMap>::Result inserted = g_FileSpecMap.insert( FileSpecMap::value_type( spec.GetName(), &spec ) );
  NOC_ASSERT( inserted.second );
}

/////////////////////////////////////////////////////////////////////////////
const FileSpec* Finder::GetFileSpec( const std::string &specName )
{
  FINDER_SCOPE_TIMER((""));

  ThrowIfNotInitialized();

  FileSpecMap::const_iterator found = g_FileSpecMap.find( specName );

  if ( found == g_FileSpecMap.end() )
    throw Finder::Exception( "Unknown spec name: %s!", specName.c_str() );

  return found->second;
}

void Finder::HackSpec( FileSpec* spec )
{
  tuid randomId = TUID::Generate();
  std::stringstream str;
  str << randomId;
  spec->SetFormatVersion( str.str() );
}

bool Finder::HackSpec( const std::string& specName )
{
  const FileSpec* constSpec = GetFileSpec( specName );
  if ( !constSpec )
  {
    return false;
  }

  Finder::FileSpec* spec = const_cast<Finder::FileSpec*>( constSpec );
  HackSpec( spec );
  
  return true;
}

void Finder::HackAllSpecs()
{
  FileSpecMap::iterator itr = g_FileSpecMap.begin();
  FileSpecMap::iterator end = g_FileSpecMap.end();

  for( ; itr != end; ++itr )
  {
    Finder::FileSpec* spec = const_cast<Finder::FileSpec*>( (*itr).second );
    HackSpec( spec );
  }
}

/////////////////////////////////////////////////////////////////////////////
void Finder::AddFileExtensionSpec( const ModifierSpec &spec )
{
  FINDER_SCOPE_TIMER((""));

  // first come, first server
  Insert<FileExtenisionSpecMap>::Result inserted = g_FileExtenisionSpecMap.insert( FileExtenisionSpecMap::value_type( spec.GetModifier(), &spec ) );
}

/////////////////////////////////////////////////////////////////////////////
const ModifierSpec* Finder::GetFileExtensionSpec( const std::string &fileName )
{
  FINDER_SCOPE_TIMER((""));

  ThrowIfNotInitialized();

  std::string extension = FileSystem::GetExtension( fileName );

  FileExtenisionSpecMap::const_iterator found = g_FileExtenisionSpecMap.find( extension );

  if ( found == g_FileExtenisionSpecMap.end() )
    throw Finder::Exception( "Unknown extension: %s!", extension.c_str() );

  return found->second;
}

/////////////////////////////////////////////////////////////////////////////
const FolderSpec* Finder::GetFolderSpec( const std::string &specName )
{
  return (const FolderSpec*) GetFinderSpec( specName );
}

/////////////////////////////////////////////////////////////////////////////
void Finder::AddFinderSpec( const FinderSpec &spec )
{
  FINDER_SCOPE_TIMER((""));

  NOC_ASSERT( g_FinderSpecMap.find( spec.GetName() ) == g_FinderSpecMap.end() );

  Insert<FinderSpecMap>::Result inserted = g_FinderSpecMap.insert( FinderSpecMap::value_type( spec.GetName(), &spec ) );
  NOC_ASSERT( inserted.second );
}

/////////////////////////////////////////////////////////////////////////////
const FinderSpec* Finder::GetFinderSpec( const std::string &specName )
{
  FINDER_SCOPE_TIMER((""));

  ThrowIfNotInitialized();

  FinderSpecMap::const_iterator found = g_FinderSpecMap.find( specName );

  if ( found == g_FinderSpecMap.end() )
    throw Finder::Exception( "Unknown spec name: %s!", specName.c_str() );

  return found->second;
}

static std::string GetHashFolder( std::string folder, u64 hash )
{
  char buf[ 32 ];
  _snprintf( buf, sizeof( buf ), TUID_HEX_FORMAT, hash );
  buf[ sizeof(buf) - 1] = 0; 

  std::string hex = buf;

  // erase the 0x
  hex.erase( 0, 2 );

  folder += hex.substr( 0, 3 ) + "/";
  folder += hex + "/";

  FileSystem::CleanName( folder );
  FileSystem::GuaranteeSlash( folder );

  return folder;
}
