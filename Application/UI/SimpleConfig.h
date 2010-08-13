#pragma once

#include "Application/API.h"
#include "Platform/Types.h"

#include <string>

// Forwards
class wxConfigBase;

namespace Helium
{
  /////////////////////////////////////////////////////////////////////////////
  // Class the wraps up interaction with the Windows Registry or alternative.
  // You can read and write persistent string values using this class.
  // 
  class APPLICATION_API SimpleConfig
  {
  private:
    wxConfigBase* m_Config;

  private:
    SimpleConfig();

  public:
    static SimpleConfig* GetInstance();

    bool Read( const tstring& relativePath, const tstring& key, tstring& value );
    bool Write( const tstring& relativePath, const tstring& key, const tstring& value );
  };
}
