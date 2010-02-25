#pragma once

#include "API.h"

#include <string>
#include <iostream>
#include <strstream>

#include "Types.h"

namespace Nocturnal
{
  //
  // Environment variable access
  //

  template<class T>
  inline bool GetEnvVar( const std::string& envVarName, T& envVarValue )
  {
    char* envVarSetting = getenv( envVarName.c_str() );

    if ( envVarSetting )
    {
      std::strstream str ( envVarSetting, (std::streamsize)strlen( envVarSetting ) );
      str >> envVarValue;
      return !str.fail();
    }

    return false;
  }

  template<>
  inline bool GetEnvVar( const std::string& envVarName, std::string& envVarValue )
  {
    char *envVarSetting = getenv( envVarName.c_str() );

    if ( envVarSetting )
    {
      envVarValue = envVarSetting;
      return true;
    }

    return false;
  }

  template<>
  inline bool GetEnvVar( const std::string& envVarName, bool& envVarValue )
  {
    char *envVarSetting = getenv( envVarName.c_str() );

    if ( envVarSetting )
    {
      if ( !stricmp( envVarSetting, "false" ) || !stricmp( envVarSetting, "0" ) )
      {
        envVarValue = false;
        return true;
      }

      if ( !stricmp( envVarSetting, "true" ) || !stricmp( envVarSetting, "1" ) )
      {
        envVarValue = true;
        return true;
      }
    }

    return false;
  }

  COMMON_API bool GetEnvFlag( const std::string &envVarName );
}