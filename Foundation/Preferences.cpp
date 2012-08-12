#include "FoundationPch.h"
#include "Preferences.h"

#include "Platform/Environment.h"

bool Helium::GetProfileDirectory( Helium::Path& preferencesDirectory )
{
    tstring prefDirectory;

    if ( !Helium::GetProfileDirectory( prefDirectory ) )
    {
        return false;
    }

    prefDirectory += TXT( "/.Helium/" );
    
    preferencesDirectory.Set( prefDirectory );
    return true;
}