#include "Preferences.h"

#include "Foundation/Environment.h"

bool Application::GetPreferencesDirectory( Nocturnal::Path& preferencesDirectory )
{
    tstring prefDirectory;

    if ( !Nocturnal::GetEnvVar( TXT( "APPDATA" ), prefDirectory ) )
    {
        return false;
    }

    prefDirectory += TXT( "/Nocturnal/" );
    
    preferencesDirectory.Set( prefDirectory );
    return true;
}