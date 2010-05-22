#pragma once

#include "API.h"
#include "AssetType.h"
#include "EntityAsset.h"

#include "Finder/Finder.h"

namespace Asset
{
  /////////////////////////////////////////////////////////////////////////////
  // stores information relivant to the AssetTypes
  class ASSET_API AssetTypeInfo
  {
  public:
    std::string               m_Name;
    std::string               m_Builder;

    std::string               m_IconFilename;
    DWORD                     m_TypeColor;

    AssetTypeInfo (
      const std::string& name = std::string( "" ),
      const std::string& builder = std::string( "" ),
      const std::string& iconFilename = std::string( "null_16.png" ),
      const DWORD typeColor = 0 );

    bool IsValid() const { return ( !m_Name.empty() || m_Name.compare( "Null" ) == 0 ); }
  };

  // Defines a map of asset type to the structure that defines them.
  typedef ASSET_API std::map< AssetType, AssetTypeInfo> M_AssetTypeInfo;

  // List of all asset types
  extern M_AssetTypeInfo g_AssetTypeInfos;

  ASSET_API const AssetTypeInfo& GetAssetTypeInfo( const AssetType assetType, bool noFail = true );

  ASSET_API const std::string& GetAssetTypeName( const AssetType assetType );
  ASSET_API const std::string& GetAssetTypeBuilder( const AssetType assetType );
  ASSET_API const std::string& GetAssetTypeIcon( const AssetType assetType );
  ASSET_API DWORD GetAssetTypeColor( const AssetType assetType );
}