#include "AssetFolder.h"

#include "Foundation/File/Path.h"

using namespace Asset;

REFLECT_DEFINE_CLASS( AssetFolder );
void AssetFolder::EnumerateClass( Reflect::Compositor<AssetFolder>& comp )
{
  Reflect::Field* fieldDirectory = comp.AddField( &AssetFolder::m_Directory, "m_Directory" );
  Reflect::Field* fieldFullPath = comp.AddField( &AssetFolder::m_FullPath, "m_FullPath" );
}

///////////////////////////////////////////////////////////////////////////////
// Ctor
//
AssetFolder::AssetFolder( const tstring& fullPath )
: m_FullPath( fullPath )
{
    m_Directory.Set( m_FullPath.Directory() );
}
