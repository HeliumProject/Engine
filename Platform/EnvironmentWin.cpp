#include "PlatformPch.h"
#include "Platform/Environment.h"

#include "Platform/String.h"

#include <ShlObj.h>

using namespace Helium;

static bool GetEnvVar( wchar_t* var, tstring& value )
{
	DWORD count = ::GetEnvironmentVariable( var, NULL, 0 );
	wchar_t* varValue = (wchar_t*)alloca( count * sizeof( wchar_t ) );
	if ( ::GetEnvironmentVariable( var, varValue, count * sizeof( wchar_t ) ) )
	{
		HELIUM_CONVERT_TO_CHAR( varValue, convertedVarValue );
		value = convertedVarValue;
		return true;
	}

	return false;
}

bool Helium::GetUserName( tstring& username )
{
    return GetEnvVar( L"USERNAME", username );
}

bool Helium::GetMachineName( tstring& computername )
{
    return GetEnvVar( L"COMPUTERNAME", computername );
}

bool Helium::GetPreferencesDirectory( tstring& profileDirectory )
{
    wchar_t path[ MAX_PATH ];
    HRESULT result = SHGetFolderPath( NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, path );
    bool bSuccess = ( result == S_OK );
    if ( bSuccess )
    {
		HELIUM_CONVERT_TO_CHAR( path, convertedPath );
        profileDirectory = convertedPath;
    }

    return bSuccess;
}

bool Helium::GetAppDataDirectory( tstring& appDataDirectory )
{
    wchar_t path[ MAX_PATH ];
    HRESULT result = SHGetFolderPath( NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path );
    bool bSuccess = ( result == S_OK );
    if ( bSuccess )
    {
		HELIUM_CONVERT_TO_CHAR( path, convertedPath );
        appDataDirectory = convertedPath;
    }

    return bSuccess;
}