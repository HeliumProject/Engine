#include "Platform/Environment.h"

bool Nocturnal::GetUsername( tstring& username )
{
    return GetEnvironmentVariable( TXT( "USER" ), username );
}

bool Nocturnal::GetComputer( tstring& computername )
{
    tstring hostname;
    if ( !GetEnvironmentVariable( TXT( "HOSTNAME" ), hostname ) )
    {
        return false;
    }

    size_t dot = hostname.find_first_of( '.' );
    if ( dot != std::string::npos )
    {
        computername = hostname.substr( 0, dot );
    }
    else
    {
        computername = hostname;
    }

    return true;
}

bool Nocturnal::GetPreferencesDirectory( tstring& preferencesDirectory )
{
    return Nocturnal::GetEnvironmentVariable( TXT( "HOME" ), preferencesDirectory ) );
}

bool Nocturnal::GetGameDataDirectory( tstring& gameDataDirectory )
{
    return Nocturnal::GetEnvironmentVariable( TXT( "HOME" ), gameDataDirectory ) );
}
