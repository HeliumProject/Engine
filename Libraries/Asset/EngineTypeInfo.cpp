#include "EngineTypeInfo.h"
#include "Exceptions.h"

static const char* s_DefaultIconFilename = "null_16.png";

using namespace Asset;

// List of all engine types
M_EngineTypeInfo Asset::g_EngineTypeInfos;

EngineTypeInfo::EngineTypeInfo
(
 const std::string& name,
 const std::string& builderDLL,
 const std::string& iconFilename,
 const DWORD typeColor
 )
: m_Name( name )
, m_BuilderDLL( builderDLL )
, m_IconFilename( iconFilename )
, m_TypeColor( typeColor )
{

}

const EngineTypeInfo& Asset::GetEngineTypeInfo( const EngineType engineType, bool noFail )
{
  M_EngineTypeInfo::const_iterator found = g_EngineTypeInfos.find( engineType );
  if ( found == g_EngineTypeInfos.end() )
  {
    if ( engineType == EngineTypes::Null ) 
    {
      static const EngineTypeInfo defaultEngineTypeInfo( "Null", "", s_DefaultIconFilename );
      return defaultEngineTypeInfo;
    }
    else if ( noFail )
    {
      return GetEngineTypeInfo( EngineTypes::Null );
    }
    else
    {
      NOC_BREAK();
    }

  }

  return found->second;
}

const std::string& Asset::GetEngineTypeBuilderDLL( const EngineType engineType )
{
  const EngineTypeInfo& engineTypeInfo = GetEngineTypeInfo( engineType );
  return engineTypeInfo.m_BuilderDLL;
}

const std::string& Asset::GetEngineTypeIcon( const EngineType engineType )
{
  const EngineTypeInfo& engineTypeInfo = GetEngineTypeInfo( engineType );
  return engineTypeInfo.m_IconFilename;
}

DWORD Asset::GetEngineTypeColor( const EngineType engineType )
{
  const EngineTypeInfo& engineTypeInfo = GetEngineTypeInfo( engineType );
  return engineTypeInfo.m_TypeColor;
}
