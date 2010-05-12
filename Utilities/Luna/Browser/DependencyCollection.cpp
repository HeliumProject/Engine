#include "Precompile.h"
#include "DependencyCollection.h"

#include "Browser.h"

#include "FileSystem/FileSystem.h"

using namespace Luna;



///////////////////////////////////////////////////////////////////////////////
/// class DependencyCollection
///////////////////////////////////////////////////////////////////////////////
REFLECT_DEFINE_CLASS( DependencyCollection )
void DependencyCollection::EnumerateClass( Reflect::Compositor<DependencyCollection>& comp )
{
  Reflect::Field* fieldRootID = comp.AddField( &DependencyCollection::m_RootID, "m_RootID" );
  Reflect::Field* fieldIsReverse = comp.AddField( &DependencyCollection::m_IsReverse, "m_IsReverse" );
}

/////////////////////////////////////////////////////////////////////////////
DependencyCollection::DependencyCollection()
: AssetCollection( "", AssetCollectionFlags::Dynamic )
, m_RootID( TUID::Null )
, m_IsReverse( false )
, m_AssetFile( NULL )
, m_IsLoading( false )
, m_DependencyLoader( this )
{
}

/////////////////////////////////////////////////////////////////////////////
DependencyCollection::DependencyCollection( const std::string& name, const u32 flags, const bool reverse )
: AssetCollection( name, flags | AssetCollectionFlags::Dynamic )
, m_RootID( TUID::Null )
, m_IsReverse( reverse )
, m_AssetFile( NULL )
, m_IsLoading( false )
, m_DependencyLoader( this )
{
}

/////////////////////////////////////////////////////////////////////////////
DependencyCollection::~DependencyCollection()
{
}

///////////////////////////////////////////////////////////////////////////////
void DependencyCollection::InitializeCollection() 
{
  __super::InitializeCollection();
  GlobalBrowser().GetBrowserPreferences()->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &DependencyCollection::OnPreferencesChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
void DependencyCollection::CleanupCollection() 
{
  GlobalBrowser().GetBrowserPreferences()->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &DependencyCollection::OnPreferencesChanged ) );
  __super::CleanupCollection();
}

///////////////////////////////////////////////////////////////////////////////
void DependencyCollection::PreDeserialize()
{
  IsLoading( true );

  __super::PreDeserialize();  
}

///////////////////////////////////////////////////////////////////////////////
void DependencyCollection::PostDeserialize()
{
  __super::PostDeserialize();

  if ( m_RootID != TUID::Null )
  {
    m_AssetFile = new Asset::AssetFile();
    m_AssetFile->m_Id = m_RootID;
  }

  IsLoading( false );
}

///////////////////////////////////////////////////////////////////////////////
std::string DependencyCollection::GetDisplayName() const
{
  std::stringstream stream;
  stream << GetName();
  stream << " (" ;
  if ( IsLoading() )
  {
    stream << "Loading...";
  }
  else
  {
    stream << GetAssetIDs().size() << " " << ( ( GetAssetIDs().size() == 1 ) ? "item" : "items" );
  }
  stream << ")";

  return stream.str();
}

///////////////////////////////////////////////////////////////////////////////
void DependencyCollection::SetRootID( const tuid rootID )
{
  if ( m_RootID != rootID )
  {
    m_RootID = rootID;

    m_AssetFile = NULL;
    m_AssetFile = new Asset::AssetFile();
    m_AssetFile->m_Id = m_RootID;

    DirtyField( GetClass()->FindField( &DependencyCollection::m_RootID ) );

    ClearAssetIDs();
  }
}

/////////////////////////////////////////////////////////////////////////////
std::string DependencyCollection::GetAssetName() const
{
  std::string assetName;
  if ( m_AssetFile )
  {
    FileSystem::GetLeaf( m_AssetFile->GetFilePath(), assetName );
  }

  return assetName;
}

/////////////////////////////////////////////////////////////////////////////
u32 DependencyCollection::GetRecursionDepthForLoad() const
{
  u32 maxRecursionDepth = 0;
  if ( IsReverse() )
  {
    maxRecursionDepth = GlobalBrowser().GetBrowserPreferences()->GetUsageCollectionRecursionDepth();
  }
  else
  {
    maxRecursionDepth = GlobalBrowser().GetBrowserPreferences()->GetDependencyCollectionRecursionDepth();
  }
  return maxRecursionDepth;
}

/////////////////////////////////////////////////////////////////////////////
void DependencyCollection::LoadDependencies( bool threaded )
{
  if ( m_RootID == TUID::Null )
    return;

  if ( threaded )
  {
    Freeze();
    IsLoading( true );
    m_DependencyLoader.StartThread();
    // It will be thawed and m_IsLoading set to false in the DependencyLoader::OnEndThread callback
  }
  else
  {
    Freeze();
    IsLoading( true );
    {
      S_tuid ids;
      GlobalBrowser().GetCacheDB()->GetAssetDependencies( m_RootID, ids, m_IsReverse, GetRecursionDepthForLoad() );
      SetAssetIDs( ids );
    }
    IsLoading( false );
    Thaw();
  }
}

/////////////////////////////////////////////////////////////////////////////
void DependencyCollection::IsLoading( bool isLoading )
{
  m_IsLoading = isLoading;
  DirtyField( GetClass()->FindField( &DependencyCollection::m_IsLoading ) );
}

/////////////////////////////////////////////////////////////////////////////
void DependencyCollection::OnPreferencesChanged( const Reflect::ElementChangeArgs& args )
{
  LoadDependencies();
}

/////////////////////////////////////////////////////////////////////////////
void DependencyCollection::SetFlags( const u32 flags )
{
  __super::SetFlags( flags | AssetCollectionFlags::Dynamic );
}

/////////////////////////////////////////////////////////////////////////////
void DependencyCollection::SetReverse( const bool reverse )
{
  m_IsReverse = reverse;
  DirtyField( GetClass()->FindField( &DependencyCollection::m_IsReverse ) );
}