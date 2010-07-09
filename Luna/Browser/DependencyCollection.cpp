#include "Precompile.h"
#include "DependencyCollection.h"

#include "Browser.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
/// class DependencyCollection
///////////////////////////////////////////////////////////////////////////////
REFLECT_DEFINE_CLASS( DependencyCollection )
void DependencyCollection::EnumerateClass( Reflect::Compositor<DependencyCollection>& comp )
{
    Reflect::Field* fieldRootPath = comp.AddField( &DependencyCollection::m_RootPath, "m_RootPath" );
    Reflect::Field* fieldIsReverse = comp.AddField( &DependencyCollection::m_IsReverse, "m_IsReverse" );
}

/////////////////////////////////////////////////////////////////////////////
DependencyCollection::DependencyCollection()
: AssetCollection( TXT( "" ), AssetCollectionFlags::Dynamic )
, m_IsReverse( false )
, m_IsLoading( false )
, m_DependencyLoader( NULL )
{
}

/////////////////////////////////////////////////////////////////////////////
DependencyCollection::DependencyCollection( const tstring& name, const u32 flags, const bool reverse )
: AssetCollection( name, flags | AssetCollectionFlags::Dynamic )
, m_IsReverse( reverse )
, m_IsLoading( false )
, m_DependencyLoader( NULL )
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
#pragma TODO( "reimplemnent without GlobalBrowser" )
    //GlobalBrowser().GetBrowserPreferences()->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &DependencyCollection::OnPreferencesChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
void DependencyCollection::CleanupCollection() 
{
#pragma TODO( "reimplemnent without GlobalBrowser" )
    //    GlobalBrowser().GetBrowserPreferences()->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &DependencyCollection::OnPreferencesChanged ) );
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

    IsLoading( false );
}

///////////////////////////////////////////////////////////////////////////////
tstring DependencyCollection::GetDisplayName() const
{
    tstringstream stream;
    stream << GetName();
    stream << TXT( " (" );
    if ( IsLoading() )
    {
        stream << TXT( "Loading..." );
    }
    else
    {
        stream << GetAssetPaths().size() << TXT( " " ) << ( ( GetAssetPaths().size() == 1 ) ? TXT( "item" ) : TXT( "items" ) );
    }
    stream << TXT( ")" );

    return stream.str();
}

///////////////////////////////////////////////////////////////////////////////
void DependencyCollection::SetRoot( const Nocturnal::Path& path )
{
    m_RootPath = path;
    DirtyField( GetClass()->FindField( &DependencyCollection::m_Path ) );
    ClearAssets();
}

/////////////////////////////////////////////////////////////////////////////
tstring DependencyCollection::GetAssetName() const
{
    return m_RootPath.Filename();
}

/////////////////////////////////////////////////////////////////////////////
u32 DependencyCollection::GetRecursionDepthForLoad() const
{
    u32 maxRecursionDepth = 0;
#pragma TODO( "reimplemnent without GlobalBrowser" )
    //if ( IsReverse() )
    //{
    //    maxRecursionDepth = GlobalBrowser().GetBrowserPreferences()->GetUsageCollectionRecursionDepth();
    //}
    //else
    //{
    //    maxRecursionDepth = GlobalBrowser().GetBrowserPreferences()->GetDependencyCollectionRecursionDepth();
    //}
    return maxRecursionDepth;
}

/////////////////////////////////////////////////////////////////////////////
void DependencyCollection::LoadDependencies( bool threaded )
{
    if ( threaded )
    {
        Freeze();
        IsLoading( true );
#pragma TODO( "reimplemnent without GlobalBrowser" )
        //        m_DependencyLoader->StartThread();
        // It will be thawed and m_IsLoading set to false in the DependencyLoader::OnEndThread callback
    }
    else
    {
        Freeze();
        IsLoading( true );
        {
            std::set< Nocturnal::Path > assets;
#pragma TODO( "reimplemnent without GlobalBrowser" )
            //            GlobalBrowser().GetCacheDB()->GetAssetDependencies( m_spFileReference, assets, m_IsReverse, GetRecursionDepthForLoad() );
            SetAssetReferences( assets );
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