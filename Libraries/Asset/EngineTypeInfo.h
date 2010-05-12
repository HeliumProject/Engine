#pragma once

#include "API.h"
#include "EngineType.h"
#include "EntityAsset.h"

#include "Finder/Finder.h"

namespace Asset
{
  /////////////////////////////////////////////////////////////////////////////
  // stores information relivant to the EngineTypes
  class ASSET_API EngineTypeInfo
  {
  public:
    std::string               m_Name;
    std::string               m_BuilderDLL;

    EntityAssetPtr            m_EntityAsset;
    std::string               m_IconFilename;
    DWORD                     m_TypeColor;

    EngineTypeInfo (
      const std::string& name = std::string( "" ),
      const std::string& builderDLL = std::string( "" ),
      const std::string& iconFilename = std::string( "null_16.png" ),
      const DWORD typeColor = 0 );

    bool IsValid() const { return ( !m_Name.empty() || m_Name.compare( "Null" ) == 0 ); }
  };

  // Defines a map of engine type to the structure that defines them.
  typedef ASSET_API std::map< EngineType, EngineTypeInfo > M_EngineTypeInfo;

  // List of all engine types
  extern M_EngineTypeInfo g_EngineTypeInfos;

  ASSET_API const EngineTypeInfo& GetEngineTypeInfo( const EngineType engineType, bool noFail = true );
  
  // EngineTypeInfo funcitons
  ASSET_API const std::string& GetEngineTypeBuilderDLL( const EngineType engineType );
  ASSET_API const std::string& GetEngineTypeIcon( const EngineType engineType );
  ASSET_API DWORD GetEngineTypeColor( const EngineType engineType );
}