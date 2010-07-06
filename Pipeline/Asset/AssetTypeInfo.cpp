#include "AssetTypeInfo.h"
#include "Pipeline/Asset/AssetExceptions.h"

static const tchar* s_DefaultIconFilename = TXT( "null" );

using namespace Asset;

// List of all engine types
M_AssetTypeInfo Asset::g_AssetTypeInfos;

AssetTypeInfo::AssetTypeInfo
(
 const tstring& name,
 const tstring& builder,
 const tstring& iconFilename,
 const u32 typeColor
 )
: m_Name( name )
, m_Builder( builder )
, m_IconFilename( iconFilename )
, m_TypeColor( typeColor )
{

}

const AssetTypeInfo& Asset::GetAssetTypeInfo( const AssetType AssetType, bool noFail )
{
  M_AssetTypeInfo::const_iterator found = g_AssetTypeInfos.find( AssetType );
  if ( found == g_AssetTypeInfos.end() )
  {
    if ( AssetType == AssetTypes::Null ) 
    {
      static const AssetTypeInfo defaultAssetTypeInfo( TXT( "Null" ), TXT( "" ), s_DefaultIconFilename );
      return defaultAssetTypeInfo;
    }
    else if ( noFail )
    {
      return GetAssetTypeInfo( AssetTypes::Null );
    }
    else
    {
      NOC_BREAK();
    }

  }

  return found->second;
}

const tstring& Asset::GetAssetTypeName( const AssetType assetType )
{
  const AssetTypeInfo& assetTypeInfo = GetAssetTypeInfo( assetType );
  return assetTypeInfo.m_Name;
}

const tstring& Asset::GetAssetTypeBuilder( const AssetType assetType )
{
  const AssetTypeInfo& assetTypeInfo = GetAssetTypeInfo( assetType );
  return assetTypeInfo.m_Builder;
}

const tstring& Asset::GetAssetTypeIcon( const AssetType assetType )
{
  const AssetTypeInfo& assetTypeInfo = GetAssetTypeInfo( assetType );
  return assetTypeInfo.m_IconFilename;
}

u32 Asset::GetAssetTypeColor( const AssetType assetType )
{
  const AssetTypeInfo& assetTypeInfo = GetAssetTypeInfo( assetType );
  return assetTypeInfo.m_TypeColor;
}
