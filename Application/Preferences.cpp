#include "Preferences.h"

#include "Platform/Environment.h"

bool Application::GetPreferencesDirectory( Nocturnal::Path& preferencesDirectory )
{
    tstring prefDirectory;

    if ( !Nocturnal::GetPreferencesDirectory( prefDirectory ) )
    {
        return false;
    }

    prefDirectory += TXT( "/Nocturnal/" );
    
    preferencesDirectory.Set( prefDirectory );
    return true;
}