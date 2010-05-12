#include "Precompile.h"
#include "DependencyLoader.h"
#include "DependencyCollection.h"

#include "Browser.h"

#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/ProjectSpecs.h"

using namespace Luna;


///////////////////////////////////////////////////////////////////////////////
/// class DependencyLoader
///////////////////////////////////////////////////////////////////////////////
DependencyLoader::DependencyLoader( DependencyCollection* collection )
: UIToolKit::ThreadMechanism( "DependencyLoader" )
, m_Collection( collection )
{
}

///////////////////////////////////////////////////////////////////////////////
DependencyLoader::~DependencyLoader()
{
  m_AssetIDs.clear();
}

///////////////////////////////////////////////////////////////////////////////
void DependencyLoader::InitData()
{
  m_AssetIDs.clear();
}

///////////////////////////////////////////////////////////////////////////////
void DependencyLoader::ThreadProc( i32 threadID )
{
  ThreadEnter( threadID );

  Asset::CacheDBPtr cacheDB = new Asset::CacheDB( "LunaBrowserDependencyLoader-AssetCacheDB" );

  // Connect the DB
  std::string rootDir = Finder::ProjectAssets() + FinderSpecs::Project::ASSET_TRACKER_FOLDER.GetRelativeFolder();
  FileSystem::GuaranteeSlash( rootDir );
  FileSystem::MakePath( rootDir );
  cacheDB->Open( FinderSpecs::Project::ASSET_TRACKER_DB.GetFile( rootDir ),
    FinderSpecs::Project::ASSET_TRACKER_CONFIGS.GetFolder(),
    cacheDB->s_TrackerDBVersion,
    SQLITE_OPEN_READONLY );

  cacheDB->GetAssetDependencies( m_Collection->GetRootID(), m_AssetIDs, m_Collection->IsReverse(), m_Collection->GetRecursionDepthForLoad(), 0, &m_StopThread );

  if ( CheckThreadLeave( threadID ) )
    return;

  m_Collection->SetAssetIDs( m_AssetIDs );
  
  ThreadLeave( threadID );
}

///////////////////////////////////////////////////////////////////////////////
void DependencyLoader::OnEndThread( const UIToolKit::ThreadProcArgs& args )
{
  if ( !IsCurrentThread( args.m_ThreadID ) )
    return;

  m_Collection->IsLoading( false );
  m_Collection->Thaw();
}
