#pragma once

#include "Pipeline/API.h"
#include "AssetType.h"
#include "Pipeline/Asset/Classes/EntityAsset.h"

namespace Asset
{
  /////////////////////////////////////////////////////////////////////////////
  // stores information relivant to the AssetTypes
  class PIPELINE_API AssetTypeInfo
  {
  public:
    tstring               m_Name;
    tstring               m_Builder;

    tstring               m_IconFilename;
    u32                       m_TypeColor;

    AssetTypeInfo (
      const tstring& name = TXT( "" ),
      const tstring& builder = TXT( "" ),
      const tstring& iconFilename = TXT( "null_16.png" ),
      const u32 typeColor = 0 );

    bool IsValid() const { return ( !m_Name.empty() || m_Name.compare( TXT( "Null" ) ) == 0 ); }
  };

  // Defines a map of asset type to the structure that defines them.
  typedef PIPELINE_API std::map< AssetType, AssetTypeInfo> M_AssetTypeInfo;

  // List of all asset types
  extern M_AssetTypeInfo g_AssetTypeInfos;

  PIPELINE_API const AssetTypeInfo& GetAssetTypeInfo( const AssetType assetType, bool noFail = true );

  PIPELINE_API const tstring& GetAssetTypeName( const AssetType assetType );
  PIPELINE_API const tstring& GetAssetTypeBuilder( const AssetType assetType );
  PIPELINE_API const tstring& GetAssetTypeIcon( const AssetType assetType );
  PIPELINE_API u32 GetAssetTypeColor( const AssetType assetType );
}