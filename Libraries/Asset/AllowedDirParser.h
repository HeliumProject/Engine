#pragma once

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"

namespace Asset
{

  class AllowedDirSettings : public Nocturnal::RefCountBase< AllowedDirSettings >
  {
  public:
    V_string m_AllowedDirs;
    std::string m_AboutDirSettings;
    std::string m_DefaultDir;
  };

  typedef Nocturnal::SmartPtr<class AllowedDirSettings> AllowedDirSettingsPtr;

  typedef std::map< std::string, AllowedDirSettingsPtr > M_AllowedDirs;

  class AllowedDirParser 
  {
  private:
    M_AllowedDirs m_AllowedDirSettings;

  public:
    AllowedDirParser();
    virtual ~AllowedDirParser();

    bool                Load( const std::string& file );
    const V_string&     GetPatterns( const std::string& typeName ) const;
    const std::string&  GetAboutDirSettings( const std::string& typeName ) const;
  };
}
