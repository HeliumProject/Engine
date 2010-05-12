#pragma once

#include "API.h"

#include <string>
#include <iostream>
#include <strstream>

#include "Types.h"

namespace Nocturnal
{
  //
  // Command line access
  //

  // std delims
  COMMON_API extern const char* CmdLineDelimiters;

  // get/set the command line
  COMMON_API void SetCmdLine( int argc, const char** argv );
  COMMON_API const char* GetCmdLine();
  COMMON_API void ReleaseCmdLine();

  // convert from flat string to argc/argv
  COMMON_API void ProcessCmdLine(const char* command, int& argc, const char**& argv);

  // get an arg by index
  COMMON_API const char** GetCmdLine( int& argc );

  // get an arg by string
  COMMON_API const char* GetCmdLineArg( const char* arg );

  template<class T>
  inline bool GetCmdLineArg( const std::string& cmdArgName, T& cmdArgValue )
  {
    const char* cmdArgSetting = GetCmdLineArg( cmdArgName.c_str() );

    if ( cmdArgSetting )
    {
      std::istrstream str ( cmdArgSetting, (std::streamsize)strlen( cmdArgSetting ) );
      str >> cmdArgValue;
      return !str.fail();
    }

    return false;
  }

  template<>
  inline bool GetCmdLineArg( const std::string& cmdArgName, std::string& cmdArgValue )
  {
    const char *cmdArgSetting = GetCmdLineArg( cmdArgName.c_str() );

    if ( cmdArgSetting )
    {
      cmdArgValue = cmdArgSetting;
      return true;
    }

    return false;
  }

  template<>
  inline bool GetCmdLineArg( const std::string& cmdArgName, bool& cmdArgValue )
  {
    const char *cmdArgSetting = GetCmdLineArg( cmdArgName.c_str() );

    if ( cmdArgSetting )
    {
      if ( !stricmp( cmdArgSetting, "false" ) || !stricmp( cmdArgSetting, "0" ) )
      {
        cmdArgValue = false;
        return true;
      }

      if ( !stricmp( cmdArgSetting, "true" ) || !stricmp( cmdArgSetting, "1" ) )
      {
        cmdArgValue = true;
        return true;
      }
    }

    return false;
  }

  COMMON_API bool GetCmdLineFlag( const char* arg );
}