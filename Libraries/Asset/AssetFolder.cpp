#include "AssetFolder.h"

#include "FileSystem/FileSystem.h"

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
AssetFolder::AssetFolder( const std::string& fullPath )
{
  m_FullPath = fullPath;
  FileSystem::CleanName( m_FullPath, false );
  FileSystem::GetLeaf( m_FullPath, m_Directory );
}
