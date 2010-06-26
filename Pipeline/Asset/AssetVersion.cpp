#include "AssetVersion.h"

const tchar* Asset::AssetVersion::ASSET_VERSION = TXT( "1" );

using namespace Asset;

REFLECT_DEFINE_CLASS(AssetVersion)

void AssetVersion::EnumerateClass( Reflect::Compositor<AssetVersion>& comp )
{
  Reflect::Field* fieldAssetVersion = comp.AddField( &AssetVersion::m_AssetVersion, "m_AssetVersion", Reflect::FieldFlags::Force );
}


AssetVersion::AssetVersion()
{
  m_AssetVersion = ASSET_VERSION;
}