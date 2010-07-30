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
    tstring homedir;
    if ( !Nocturnal::GetEnvironmentVariable( TXT( "HOME" ), homedir ) )
    {
        return false;
    }

    // this isn't great, but under posix there usually isn't a set preferences directory
    preferencesDirectory = homedir + "/.preferences/";

    return true;
}