#include "Precompile.h"

#include "Settings.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Overridden to store the current preferences version (provided by derived 
// classes) before saving.  Derived classes can change their version to 
// invalidate old preferences files.
// 
void Settings::PreSerialize()
{
    __super::PreSerialize();

    m_SavedVersion = GetCurrentVersion();
}

tstring Settings::GetCurrentVersion() const
{
#pragma TODO("We should hash the reflection data here")
    return TXT( "Unknown" );
}

///////////////////////////////////////////////////////////////////////////////
// Loads the preference settings from a file on disc, if the file exists.  If
// the file could not be loaded (or the version was old), this function will 
// return false, and any values you query will be at the default settings.
// 
bool Settings::LoadFromFile( const Nocturnal::Path& path )
{
    if ( path.Exists() )
    {
        try
        {
            // Only use the settings on disk if the version matches
            Reflect::VersionPtr version = Reflect::Archive::FromFile< Reflect::Version >( path );
            if ( version && version->IsCurrent() )
            {
                SettingsPtr fromFile = Reflect::Archive::FromFile< Settings >( path );
                if ( fromFile.ReferencesObject() && fromFile->m_SavedVersion == GetCurrentVersion() )
                {
                    fromFile->CopyTo( this );
                    m_Loaded.Raise( PreferencesLoadedArgs() );
                    return true;
                }
            }
        }
        catch ( const Nocturnal::Exception& e )
        {
            Log::Error( TXT( "Failed to load preferences from %s: %s\n" ), path.c_str(), e.What() );
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Save the preferences to the specified file on disc.
// 
bool Settings::SaveToFile( const Nocturnal::Path& path, tstring& error, Reflect::VersionPtr version )
{
    bool result = false;

    try
    {
        Reflect::Archive::ToFile( this, path, version );
        result = true;
    }
    catch ( const Reflect::Exception& e )
    {
        error = TXT( "Unable to save preferences to " ) + path;
        error += TXT( " [" ) + e.Get() + TXT( "]" );
    }

    return result;
}