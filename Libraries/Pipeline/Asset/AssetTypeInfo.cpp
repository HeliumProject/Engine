#include "AssetTypeInfo.h"
#include "Pipeline/Asset/AssetExceptions.h"

static const char* s_DefaultIconFilename = "null_16.png";

using namespace Asset;

// List of all engine types
M_AssetTypeInfo Asset::g_AssetTypeInfos;

AssetTypeInfo::AssetTypeInfo
(
 const std::string& name,
 const std::string& builder,
 const std::string& iconFilename,
 const DWORD typeColor
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
      static const AssetTypeInfo defaultAssetTypeInfo( "Null", "", s_DefaultIconFilename );
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

const std::string& Asset::GetAssetTypeName( const AssetType assetType )
{
  const AssetTypeInfo& assetTypeInfo = GetAssetTypeInfo( assetType );
  return assetTypeInfo.m_Name;
}

const std::string& Asset::GetAssetTypeBuilder( const AssetType assetType )
{
  const AssetTypeInfo& assetTypeInfo = GetAssetTypeInfo( assetType );
  return assetTypeInfo.m_Builder;
}

const std::string& Asset::GetAssetTypeIcon( const AssetType assetType )
{
  const AssetTypeInfo& assetTypeInfo = GetAssetTypeInfo( assetType );
  return assetTypeInfo.m_IconFilename;
}

DWORD Asset::GetAssetTypeColor( const AssetType assetType )
{
  const AssetTypeInfo& assetTypeInfo = GetAssetTypeInfo( assetType );
  return assetTypeInfo.m_TypeColor;
}
