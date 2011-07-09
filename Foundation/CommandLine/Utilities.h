#pragma once

#include "Foundation/API.h"

#include <string>
#include <iostream>
#include <strstream>

#include "Platform/Types.h"

namespace Helium
{
    //
    // Command line access
    //

    // std delims
    HELIUM_FOUNDATION_API extern const tchar_t* CmdLineDelimiters;

    // get/set the command line
    HELIUM_FOUNDATION_API void SetCmdLine( int argc, const tchar_t** argv );
    HELIUM_FOUNDATION_API const tchar_t* GetCmdLine();
    HELIUM_FOUNDATION_API void ReleaseCmdLine();

    // convert from flat string to argc/argv
    HELIUM_FOUNDATION_API void ProcessCmdLine(const tchar_t* command, int& argc, const tchar_t**& argv);

    // get an arg by index
    HELIUM_FOUNDATION_API const tchar_t** GetCmdLine( int& argc );

    // get an arg by string
    HELIUM_FOUNDATION_API const tchar_t* GetCmdLineArg( const tchar_t* arg );

    template<class T>
    inline bool GetCmdLineArg( const tstring& cmdArgName, T& cmdArgValue )
    {
        const tchar_t* cmdArgSetting = GetCmdLineArg( cmdArgName.c_str() );

        if ( cmdArgSetting )
        {
            std::istrstream str ( cmdArgSetting, (std::streamsize)_tcslen( cmdArgSetting ) );
            str >> cmdArgValue;
            return !str.fail();
        }

        return false;
    }

    template<>
    inline bool GetCmdLineArg( const tstring& cmdArgName, tstring& cmdArgValue )
    {
        const tchar_t *cmdArgSetting = GetCmdLineArg( cmdArgName.c_str() );

        if ( cmdArgSetting )
        {
            cmdArgValue = cmdArgSetting;
            return true;
        }

        return false;
    }

    template<>
    inline bool GetCmdLineArg( const tstring& cmdArgName, bool& cmdArgValue )
    {
        const tchar_t *cmdArgSetting = GetCmdLineArg( cmdArgName.c_str() );

        if ( cmdArgSetting )
        {
            if ( !_tcsicmp( cmdArgSetting, TXT( "false" ) ) || !_tcsicmp( cmdArgSetting, TXT( "0" ) ) )
            {
                cmdArgValue = false;
                return true;
            }

            if ( !_tcsicmp( cmdArgSetting, TXT( "true" ) ) || !_tcsicmp( cmdArgSetting, TXT( "1" ) ) )
            {
                cmdArgValue = true;
                return true;
            }
        }

        return false;
    }

    HELIUM_FOUNDATION_API bool GetCmdLineFlag( const tchar_t* arg );
}