#include "Platform/Environment.h"
#include "Platform/Windows/Windows.h"

#include <ShlObj.h>

bool Helium::GetUsername( tstring& username )
{
    return Helium::GetEnvironmentVariable( TXT( "USERNAME" ), username );
}

bool Helium::GetComputer( tstring& computername )
{
    return Helium::GetEnvironmentVariable( TXT( "COMPUTERNAME" ), computername );
}

bool Helium::GetPreferencesDirectory( tstring& preferencesDirectory )
{
    tchar_t path[ MAX_PATH ];
    HRESULT result = SHGetFolderPath( NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, path );
    bool bSuccess = ( result == S_OK );
    if ( bSuccess )
    {
        preferencesDirectory = path;
    }

    return bSuccess;
}

bool Helium::GetGameDataDirectory( tstring& gameDataDirectory )
{
    tchar_t path[ MAX_PATH ];
    HRESULT result = SHGetFolderPath( NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, path );
    bool bSuccess = ( result == S_OK );
    if ( bSuccess )
    {
        gameDataDirectory = path;
    }

    return bSuccess;
}