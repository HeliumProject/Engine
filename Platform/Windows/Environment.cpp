#include "Platform/Environment.h"

bool Nocturnal::GetUsername( tstring& username )
{
    return GetEnvironmentVariable( TXT( "USERNAME" ), username );
}

bool Nocturnal::GetComputer( tstring& computername )
{
    return GetEnvironmentVariable( TXT( "COMPUTERNAME" ), computername );
}

bool Nocturnal::GetPreferencesDirectory( tstring& preferencesDirectory )
{
    return Nocturnal::GetEnvironmentVariable( TXT( "APPDATA" ), preferencesDirectory );
}