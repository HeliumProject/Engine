#pragma once

#include "Foundation/API.h"

#include <string>
#include <iostream>
#include <strstream>

#include "Platform/Types.h"

namespace Nocturnal
{
    //
    // Environment variable access
    //

    template<class T>
    inline bool GetEnvVar( const tstring& envVarName, T& envVarValue )
    {
        tchar* envVarSetting = _tgetenv( envVarName.c_str() );

        if ( envVarSetting )
        {
            tstringstream str ( envVarSetting, (std::streamsize)_tcslen( envVarSetting ) );
            str >> envVarValue;
            return !str.fail();
        }

        return false;
    }

    template<>
    inline bool GetEnvVar( const tstring& envVarName, tstring& envVarValue )
    {
        tchar *envVarSetting = _tgetenv( envVarName.c_str() );

        if ( envVarSetting )
        {
            envVarValue = envVarSetting;
            return true;
        }

        return false;
    }

    template<>
    inline bool GetEnvVar( const tstring& envVarName, bool& envVarValue )
    {
        tchar *envVarSetting = _tgetenv( envVarName.c_str() );

        if ( envVarSetting )
        {
            if ( !_tcsicmp( envVarSetting, TXT( "false" ) ) || !_tcsicmp( envVarSetting, TXT( "0" ) ) )
            {
                envVarValue = false;
                return true;
            }

            if ( !_tcsicmp( envVarSetting, TXT( "true" ) ) || !_tcsicmp( envVarSetting, TXT( "1" ) ) )
            {
                envVarValue = true;
                return true;
            }
        }

        return false;
    }

    FOUNDATION_API bool GetEnvFlag( const tstring &envVarName );
}