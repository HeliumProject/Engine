#pragma once

#include "Foundation/API.h"

#include <string>
#include <iostream>
#include <strstream>

#include "Platform/Types.h"

namespace Nocturnal
{
    //
    // Command line access
    //

    // std delims
    FOUNDATION_API extern const tchar* CmdLineDelimiters;

    // get/set the command line
    FOUNDATION_API void SetCmdLine( int argc, const tchar** argv );
    FOUNDATION_API const tchar* GetCmdLine();
    FOUNDATION_API void ReleaseCmdLine();

    // convert from flat string to argc/argv
    FOUNDATION_API void ProcessCmdLine(const tchar* command, int& argc, const tchar**& argv);

    // get an arg by index
    FOUNDATION_API const tchar** GetCmdLine( int& argc );

    // get an arg by string
    FOUNDATION_API const tchar* GetCmdLineArg( const tchar* arg );

    template<class T>
    inline bool GetCmdLineArg( const tstring& cmdArgName, T& cmdArgValue )
    {
        const tchar* cmdArgSetting = GetCmdLineArg( cmdArgName.c_str() );

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
        const tchar *cmdArgSetting = GetCmdLineArg( cmdArgName.c_str() );

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
        const tchar *cmdArgSetting = GetCmdLineArg( cmdArgName.c_str() );

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

    FOUNDATION_API bool GetCmdLineFlag( const tchar* arg );
#pragma deprecated( GetCmdLineFlag )  // All commandline options should be defined and parsed once in the application, we shouldn't be parsing the entire commandline everytime! 
}