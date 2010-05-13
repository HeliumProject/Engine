#include "Profile/Profile.h"

#include "Finder.h"
#include "Exceptions.h"

#include "Common/Assert.h"
#include "Common/Config.h"
#include "Common/Version.h"
#include "Common/Boost/Regex.h" 
#include "Common/Container/Insert.h"
#include "Common/Environment.h"

#include "AnimationSpecs.h"
#include "AssetSpecs.h"
#include "CodeSpecs.h"
#include "ExtensionSpecs.h"
#include "FontSpecs.h"
#include "LunaSpecs.h"
#include "ProjectSpecs.h"

using Nocturnal::Insert; 

#include "Common/Environment.h"
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


//
// We cache these constant strings into globals to save string copies
//

std::string g_CachedProjectName;
std::string g_CachedProjectGame;
std::string g_CachedProjectRoot;
std::string g_CachedProjectTemp;
std::string g_CachedProjectLog;
std::string g_CachedProjectCode;
std::string g_CachedProjectCodeBranch;
std::string g_CachedProjectAssets;
std::string g_CachedProjectAssetsBranch;
std::string g_CachedProjectMaya;
std::string g_CachedProjectTextures;
std::string g_CachedProjectBuilt;
std::string g_CachedProjectExport;
std::string g_CachedProjectMetaData;
std::string g_CachedProjectCache;
std::string g_CachedProjectProcessed;
std::string g_CachedProjectScripts;

std::string g_CachedProjectUserNetworkRoot;


// We have a bunch of tools directories, so we group them here... 
bool        g_ToolsDirsInitialized = false; 
std::string g_ToolsDirs[ToolsFolders::Count]; 


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
  FinderSpecs::Animation::Init();
  FinderSpecs::Asset::Init();
  FinderSpecs::Code::Init();
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

  g_CachedProjectName.clear();
  g_CachedProjectGame.clear();
  g_CachedProjectRoot.clear();
  g_CachedProjectTemp.clear();
  g_CachedProjectLog.clear();
  g_CachedProjectCode.clear();
  g_CachedProjectCodeBranch.clear();
  g_CachedProjectAssets.clear();
  g_CachedProjectAssetsBranch.clear();
  g_CachedProjectMaya.clear();
  g_CachedProjectTextures.clear();
  g_CachedProjectExport.clear();
  g_CachedProjectMetaData.clear();
  g_CachedProjectBuilt.clear();
  g_CachedProjectProcessed.clear();
  g_CachedProjectScripts.clear();
  g_CachedProjectCache.clear();
  g_CachedProjectUserNetworkRoot.clear();

  for(int i = 0; i < ToolsFolders::Count; i++)
  {
    g_ToolsDirs[i].clear(); 
  }
  g_ToolsDirsInitialized = false; 


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

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectName()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectName.empty() )
    return g_CachedProjectName;

  if ( !Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"PROJECT_NAME", g_CachedProjectName ) )
    throw Nocturnal::Exception( NOCTURNAL_STUDIO_PREFIX"PROJECT_NAME environment variable not set!" );

  if ( g_CachedProjectName.empty() )
    throw Nocturnal::Exception( NOCTURNAL_STUDIO_PREFIX"PROJECT_NAME environment variable is empty!" );

  FileSystem::CleanName( g_CachedProjectName );

  return g_CachedProjectName;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectGame()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectGame.empty() )
    return g_CachedProjectGame;

  if ( !Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"GAME", g_CachedProjectGame ) )
    throw Nocturnal::Exception( NOCTURNAL_STUDIO_PREFIX"GAME environment variable not set!" );

  if ( g_CachedProjectGame.empty() )
    throw Nocturnal::Exception( NOCTURNAL_STUDIO_PREFIX"GAME environment variable is empty!" );

  FileSystem::CleanName( g_CachedProjectGame );

  return g_CachedProjectGame;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectRoot()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectRoot.empty() )
    return g_CachedProjectRoot;

  if ( !Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"Root", g_CachedProjectRoot ) )
    throw Nocturnal::Exception( NOCTURNAL_STUDIO_PREFIX"Root environment variable not set!" );

  if ( g_CachedProjectRoot.empty() )
    throw Nocturnal::Exception( NOCTURNAL_STUDIO_PREFIX"Root environment variable is empty!" );

  FileSystem::GuaranteeSlash( g_CachedProjectRoot );
  g_CachedProjectRoot += ProjectName();
  FileSystem::GuaranteeSlash( g_CachedProjectRoot );
  FileSystem::CleanName( g_CachedProjectRoot );

  return g_CachedProjectRoot;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectTemp()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectTemp.empty() )
    return g_CachedProjectTemp;

  g_CachedProjectTemp = FinderSpecs::Project::TEMP_FOLDER.GetFolder();
  FileSystem::CleanName( g_CachedProjectTemp );
  FileSystem::GuaranteeSlash( g_CachedProjectTemp );

  FileSystem::MakePath( g_CachedProjectTemp );

  return g_CachedProjectTemp;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectLog()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectLog.empty() )
    return g_CachedProjectLog;

  g_CachedProjectLog = FinderSpecs::Project::LOG_FOLDER.GetFolder();
  FileSystem::CleanName( g_CachedProjectLog );
  FileSystem::GuaranteeSlash( g_CachedProjectLog );

  return g_CachedProjectLog;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectTools(ToolsFolder which)
{
  FINDER_SCOPE_TIMER((""));
  NOC_ASSERT(which < ToolsFolders::Count); 

  // first, fill out the cached tools (root) directory
  // perhaps a little agressive, but cleaner code if we initialize 
  // all of them at the same time... 

  // once EnvShell is rolled out, programmers will have these variables 
  // set. even then, other users (artists) will not, and so we need
  // the fallback cases... 
  // 

  if(g_ToolsDirsInitialized == false)
  {
    // note that the way we do this, if you have set IG_PROJECT_TOOLS
    // but none of the other variables, you will get subdirs of 
    // IG_PROJECT_TOOLS for the other directories. 

    // ROOT
    std::string& toolsRoot = g_ToolsDirs[ToolsFolders::Root]; 
    if(!Nocturnal::GetEnvVar(NOCTURNAL_STUDIO_PREFIX"PROJECT_TOOLS", toolsRoot))
    {
      toolsRoot = ProjectRoot() + "tools"; 
    }
    FileSystem::GuaranteeSlash( toolsRoot );

    // BAT
    std::string& bat = g_ToolsDirs[ToolsFolders::Bat]; 
    if(!Nocturnal::GetEnvVar(NOCTURNAL_STUDIO_PREFIX"PROJECT_BAT", bat))
    {
      bat = toolsRoot + "bat"; 
    }

    // BIN
    std::string& bin = g_ToolsDirs[ToolsFolders::Bin]; 
    if(!Nocturnal::GetEnvVar(NOCTURNAL_STUDIO_PREFIX"PROJECT_BIN", bin))
    {
      bin = toolsRoot + "bin"; 
    }

    // EXTERN
    std::string& ext = g_ToolsDirs[ToolsFolders::Extern]; 
    if(!Nocturnal::GetEnvVar(NOCTURNAL_STUDIO_PREFIX"PROJECT_EXTERN", ext))
    {
      ext = toolsRoot + "extern"; 
    }

    // MAYA
    std::string& maya = g_ToolsDirs[ToolsFolders::Maya]; 
    if(!Nocturnal::GetEnvVar(NOCTURNAL_STUDIO_PREFIX"PROJECT_MAYA", maya))
    {
      maya = toolsRoot + "maya"; 
    }

    // SCRIPTS
    std::string& scripts = g_ToolsDirs[ToolsFolders::Scripts]; 
    if(!Nocturnal::GetEnvVar(NOCTURNAL_STUDIO_PREFIX"PROJECT_SCRIPTS", scripts))
    {
      scripts = toolsRoot + "scripts"; 
    }

    for(size_t i = 0; i < ToolsFolders::Count; i++)
    {
      FileSystem::CleanName(g_ToolsDirs[i]); 
      FileSystem::GuaranteeSlash(g_ToolsDirs[i]); 
    }

    g_ToolsDirsInitialized = true; 
  }


  return g_ToolsDirs[which]; 
}

/////////////////////////////////////////////////////////////////////////////
void BranchName(const char* branchType, std::string& branchName)
{
  FINDER_SCOPE_TIMER((""));

  if ( !Nocturnal::GetEnvVar(branchType, branchName) )
  {
    branchName = FinderSpecs::Project::DEFAULT_BRANCH_FOLDER.GetFolder();
  }
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectCode()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectCode.empty() )
    return g_CachedProjectCode;

  g_CachedProjectCode = FinderSpecs::Project::CODE_FOLDER.GetFolder() + ProjectCodeBranch();
  FileSystem::CleanName( g_CachedProjectCode );
  FileSystem::GuaranteeSlash( g_CachedProjectCode );

  return g_CachedProjectCode;
}


/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectCodeBranch()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectCodeBranch.empty() )
    return g_CachedProjectCodeBranch;

  // if the environment is set, use that (makes envshell work)
  char* projectCode = getenv( NOCTURNAL_STUDIO_PREFIX"CODE_BRANCH_NAME" );
  if ( projectCode )
  {
    g_CachedProjectCodeBranch = projectCode;
  }
  else
  {
    BranchName( NOCTURNAL_STUDIO_PREFIX"CODE_BRANCH_NAME_" NOCTURNAL_PROJECT_NAME, g_CachedProjectCodeBranch );
  }

  FileSystem::CleanName( g_CachedProjectCodeBranch );
  FileSystem::GuaranteeSlash( g_CachedProjectCodeBranch );

  return g_CachedProjectCodeBranch;
}


/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectAssets()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectAssets.empty() )
    return g_CachedProjectAssets;

  char* projectAssets = getenv( NOCTURNAL_STUDIO_PREFIX"PROJECT_ASSETS" );
  if ( projectAssets )
  {
    g_CachedProjectAssets = projectAssets;
  }
  else
  {
    g_CachedProjectAssets = FinderSpecs::Project::ASSETS_FOLDER.GetFolder() + ProjectAssetsBranch();
  }

  FileSystem::CleanName( g_CachedProjectAssets );
  FileSystem::GuaranteeSlash( g_CachedProjectAssets );

  return g_CachedProjectAssets;
}


/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectAssetsBranch()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectAssetsBranch.empty() )
    return g_CachedProjectAssetsBranch;

  // if the environment is set, use that (makes envshell work)
  char* projectAssets = getenv( NOCTURNAL_STUDIO_PREFIX"ASSETS_BRANCH_NAME" );
  if ( projectAssets )
  {
    g_CachedProjectAssetsBranch = projectAssets;
  }
  else
  {
    BranchName( NOCTURNAL_STUDIO_PREFIX"ASSETS_BRANCH_NAME_" NOCTURNAL_PROJECT_NAME, g_CachedProjectAssetsBranch );
  }

  FileSystem::CleanName( g_CachedProjectAssetsBranch );
  FileSystem::GuaranteeSlash( g_CachedProjectAssetsBranch );

  return g_CachedProjectAssetsBranch;
}


/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectCache()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectCache.empty() )
    return g_CachedProjectCache;

  g_CachedProjectCache = FinderSpecs::Project::CACHE_FOLDER.GetFolder() + ProjectAssetsBranch();
  FileSystem::CleanName( g_CachedProjectCache );
  FileSystem::GuaranteeSlash( g_CachedProjectCache );

  return g_CachedProjectCache;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectMaya()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectMaya.empty() )
    return g_CachedProjectMaya;

  g_CachedProjectMaya = FinderSpecs::Asset::MAYA_FOLDER.GetFolder();
  FileSystem::CleanName( g_CachedProjectMaya );
  FileSystem::GuaranteeSlash( g_CachedProjectMaya );

  return g_CachedProjectMaya;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectTextures()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectTextures.empty() )
    return g_CachedProjectTextures;

  g_CachedProjectTextures = FinderSpecs::Asset::TEXTURES_FOLDER.GetFolder();
  FileSystem::CleanName( g_CachedProjectTextures );
  FileSystem::GuaranteeSlash( g_CachedProjectTextures );

  return g_CachedProjectTextures;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectExport()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectExport.empty() )
    return g_CachedProjectExport;

  g_CachedProjectExport = FinderSpecs::Asset::EXPORT_FOLDER.GetFolder();
  FileSystem::CleanName( g_CachedProjectExport );
  FileSystem::GuaranteeSlash( g_CachedProjectExport );

  return g_CachedProjectExport;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectMetaData()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectMetaData.empty() )
    return g_CachedProjectMetaData;

  g_CachedProjectMetaData = FinderSpecs::Asset::METADATA_FOLDER.GetFolder();
  FileSystem::CleanName( g_CachedProjectMetaData );
  FileSystem::GuaranteeSlash( g_CachedProjectMetaData );

  return g_CachedProjectMetaData;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectScripts()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectScripts.empty() )
    return g_CachedProjectScripts;

  g_CachedProjectScripts = FinderSpecs::Asset::SCRIPT_FOLDER.GetFolder();
  FileSystem::CleanName( g_CachedProjectScripts );
  FileSystem::GuaranteeSlash( g_CachedProjectScripts );

  return g_CachedProjectScripts;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectBuilt()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectBuilt.empty() )
    return g_CachedProjectBuilt;

  g_CachedProjectBuilt = FinderSpecs::Project::BUILT_FOLDER.GetFolder();
  FileSystem::CleanName( g_CachedProjectBuilt );
  FileSystem::GuaranteeSlash( g_CachedProjectBuilt );

  return g_CachedProjectBuilt;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectProcessed()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectProcessed.empty() )
    return g_CachedProjectProcessed;

  g_CachedProjectProcessed = FinderSpecs::Project::PROCESSED_FOLDER.GetFolder();
  FileSystem::CleanName( g_CachedProjectProcessed );
  FileSystem::GuaranteeSlash( g_CachedProjectProcessed );

  return g_CachedProjectProcessed;
}

/////////////////////////////////////////////////////////////////////////////
const std::string& Finder::ProjectUserNetworkRoot()
{
  FINDER_SCOPE_TIMER((""));

  if ( !g_CachedProjectUserNetworkRoot.empty() )
    return g_CachedProjectUserNetworkRoot;

  std::string computerName;
  if ( !Nocturnal::GetEnvVar( "COMPUTERNAME", computerName ) )
    throw Nocturnal::Exception( "COMPUTERNAME environment variable is not set, cannot continue!" );

  g_CachedProjectUserNetworkRoot = "//";
  g_CachedProjectUserNetworkRoot += computerName;
  g_CachedProjectUserNetworkRoot += "/projects/";
  g_CachedProjectUserNetworkRoot += ProjectName();
  g_CachedProjectUserNetworkRoot += "/";

  FileSystem::CleanName( g_CachedProjectUserNetworkRoot );

  return g_CachedProjectUserNetworkRoot;
}

/////////////////////////////////////////////////////////////////////////////
bool Finder::IsInProject( const std::string& path )
{
  return path.substr( 0, ProjectRoot().length() ).compare( ProjectRoot() ) == 0;
}

/////////////////////////////////////////////////////////////////////////////
std::string Finder::GetExportFolder( const std::string &path, bool useFilename )
{
  FINDER_SCOPE_TIMER((""));

  std::string folder = path;

  if ( !path.empty() && *path.rbegin() != '/' )
  {
    if ( useFilename )
    {
      FileSystem::StripExtension( folder );
    }
    else
    {
      FileSystem::StripLeaf( folder );
    }
  }

  FileSystem::StripPrefix( ProjectAssets(), folder );
  folder = ProjectExport() + folder;

  FileSystem::GuaranteeSlash( folder );

  return folder;
}

/////////////////////////////////////////////////////////////////////////////
std::string Finder::GetBuiltFolder( const std::string &path, bool useFilename )
{
  FINDER_SCOPE_TIMER((""));

  std::string folder = path;

  // if it has a file, strip it off
  if ( !folder.empty() && *folder.rbegin() != '/' )
  {
    // if they want us to use the filename as part of the built folder, just strip the extension
    if ( useFilename )
    {
      FileSystem::StripExtension( folder );
    }
    // otherwise, strip the filename off
    else
    {
      FileSystem::StripLeaf( folder );
    }
  }

  FileSystem::StripPrefix( ProjectAssets(), folder );
  folder = ProjectBuilt() + folder;

  FileSystem::CleanName( folder );
  FileSystem::GuaranteeSlash( folder );

  return folder;
}

static std::string GetTuidFolder( std::string folder, tuid assetId )
{
  char buf[ 32 ];
  _snprintf( buf, sizeof( buf ), TUID_HEX_FORMAT, assetId );
  buf[ sizeof(buf) - 1] = 0; 

  std::string hexTuid = buf;

  // erase the 0x
  hexTuid.erase( 0, 2 );

  folder += hexTuid.substr( 0, 3 ) + "/";
  folder += hexTuid + "/";

  FileSystem::CleanName( folder );
  FileSystem::GuaranteeSlash( folder );

  return folder;
}

std::string Finder::GetBuiltFolder( tuid assetId )
{
  return GetTuidFolder( ProjectBuilt() + FinderSpecs::Asset::TUIDS_FOLDER.GetRelativeFolder(), assetId );
}

std::string Finder::GetThumbnailFolder( tuid assetId )
{
  return GetTuidFolder( ProjectAssets() + FinderSpecs::Asset::THUMBNAILS_FOLDER.GetRelativeFolder() + FinderSpecs::Asset::TUIDS_FOLDER.GetRelativeFolder(), assetId );
}

void Finder::StripAnyProjectRoot( std::string& path )
{
  const boost::regex s_StripAnyProjectRoot("(^[a-zA-Z]\\:([\\/][\\w\\- ]+){1}[\\/])", boost::regex::perl|boost::regex::icase); 
  path = boost::regex_replace( path, s_StripAnyProjectRoot, "" );
}

std::string Finder::StripAnyProjectRoot( const char *path )
{
  std::string newPath = path;
  Finder::StripAnyProjectRoot( newPath );
  return newPath;
}

void Finder::StripAnyProjectAssets( std::string& path )
{
  const boost::regex s_StripAnyProjectAssets("(^[a-zA-Z]\\:([\\/][\\w\\- ]+)([\\/]assets)([\\/][\\w\\- ]+)[\\/])", boost::regex::perl|boost::regex::icase); 
  path = boost::regex_replace( path, s_StripAnyProjectAssets, "" );
}

std::string Finder::StripAnyProjectAssets( const char *path )
{
  std::string newPath = path;
  Finder::StripAnyProjectAssets( newPath );
  return newPath;
}
