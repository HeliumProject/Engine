#include "Precompile.h"
#include "DependencyLoader.h"
#include "DependencyCollection.h"

#include "Browser.h"

using namespace Luna;


///////////////////////////////////////////////////////////////////////////////
/// class DependencyLoader
///////////////////////////////////////////////////////////////////////////////
DependencyLoader::DependencyLoader( const Nocturnal::Path& rootDirectory, Luna::DependencyCollection* collection )
: Nocturnal::ThreadMechanism( TXT( "DependencyLoader" ) )
, m_RootDirectory( rootDirectory )
, m_Collection( collection )
{
}

///////////////////////////////////////////////////////////////////////////////
DependencyLoader::~DependencyLoader()
{
    m_AssetPaths.clear();
}

///////////////////////////////////////////////////////////////////////////////
void DependencyLoader::InitData()
{
    m_AssetPaths.clear();
}

///////////////////////////////////////////////////////////////////////////////
void DependencyLoader::ThreadProc( i32 threadID )
{
    ThreadEnter( threadID );

    if ( CheckThreadLeave( threadID ) )
    {
        return;
    }

    m_Collection->SetAssetReferences( m_AssetPaths );

    ThreadLeave( threadID );
}

///////////////////////////////////////////////////////////////////////////////
void DependencyLoader::OnEndThread( const Nocturnal::ThreadProcArgs& args )
{
    if ( !IsCurrentThread( args.m_ThreadID ) )
        return;

    m_Collection->IsLoading( false );
    m_Collection->Thaw();
}
