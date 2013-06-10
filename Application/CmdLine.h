#pragma once

#include "Application/API.h"

#include <string>
#include <iostream>
#include <strstream>

#include "Platform/Types.h"
#include "Foundation/String.h"

namespace Helium
{
    //
    // Command line access
    //

    // std delims
    HELIUM_APPLICATION_API extern const char* CmdLineDelimiters;

    // get/set the command line
    HELIUM_APPLICATION_API void SetCmdLine( int argc, const char** argv );
    HELIUM_APPLICATION_API const char* GetCmdLine();
    HELIUM_APPLICATION_API void ReleaseCmdLine();

    // convert from flat string to argc/argv
    HELIUM_APPLICATION_API void ProcessCmdLine(const char* command, int& argc, const char**& argv);

    // get an arg by index
    HELIUM_APPLICATION_API const char** GetCmdLine( int& argc );

    // get an arg by string
    HELIUM_APPLICATION_API const char* GetCmdLineArg( const char* arg );

    template<class T>
    inline bool GetCmdLineArg( const std::string& cmdArgName, T& cmdArgValue )
    {
        const char* cmdArgSetting = GetCmdLineArg( cmdArgName.c_str() );

        if ( cmdArgSetting )
        {
            std::istrstream str ( cmdArgSetting, (std::streamsize)_tcslen( cmdArgSetting ) );
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
            if ( !CaseInsensitiveCompareString( cmdArgSetting, TXT( "false" ) ) || !CaseInsensitiveCompareString( cmdArgSetting, TXT( "0" ) ) )
            {
                cmdArgValue = false;
                return true;
            }

            if ( !CaseInsensitiveCompareString( cmdArgSetting, TXT( "true" ) ) || !CaseInsensitiveCompareString( cmdArgSetting, TXT( "1" ) ) )
            {
                cmdArgValue = true;
                return true;
            }
        }

        return false;
    }

    HELIUM_APPLICATION_API bool GetCmdLineFlag( const char* arg );
}