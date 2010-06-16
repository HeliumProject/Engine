#pragma once

#include "Application/API.h"

#include <string>

// Forwards
class wxConfigBase;

namespace Nocturnal
{
  /////////////////////////////////////////////////////////////////////////////
  // Class the wraps up interaction with the Windows Registry.  You can read
  // and write string values to the registry using this class.
  // 
  class APPLICATION_API RegistryConfig
  {
  private:
    wxConfigBase* m_Config;

  private:
    RegistryConfig();

  public:
    virtual ~RegistryConfig();

    static RegistryConfig* GetInstance();
    bool Read( const std::string& relativePath, const std::string& key, std::string& value );
    bool Write( const std::string& relativePath, const std::string& key, const std::string& value );
  };
}
