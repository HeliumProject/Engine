#include "ApplicationPch.h"
#include "Preferences.h"

#include "Platform/Environment.h"

bool Helium::GetPreferencesDirectory( Helium::Path& preferencesDirectory )
{
    tstring prefDirectory;

    if ( !Helium::GetPreferencesDirectory( prefDirectory ) )
    {
        return false;
    }

    prefDirectory += TXT( "/.Helium/" );
    
    preferencesDirectory.Set( prefDirectory );
    return true;
}