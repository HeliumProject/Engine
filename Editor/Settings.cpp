#include "Precompile.h"
#include "Settings.h"

#include "Platform/Debug.h"

using namespace Helium;
using namespace Helium::Editor;

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
bool Settings::LoadFromFile( const Helium::Path& path )
{
    Reflect::V_Element elements;

    if ( path.Exists() )
    {
        if ( Platform::IsDebuggerPresent() )
        {
            Reflect::Archive::FromFile( path, elements );
        }
        else
        {
            try
            {
                Reflect::Archive::FromFile( path, elements );
            }
            catch ( const Helium::Exception& e )
            {
                Log::Error( TXT( "Failed to load preferences from %s: %s\n" ), path.c_str(), e.What() );
            }
        }

        if ( elements.size() > 1 )
        {
            // Only use the settings on disk if the version matches
            Reflect::VersionPtr version = Reflect::TryCast<Reflect::Version>( elements.at(0) );
            if ( version && version->IsCurrent() )
            {
                SettingsPtr settings = Reflect::TryCast<Settings>( elements.at(1) );
                if ( settings.ReferencesObject() && settings->m_SavedVersion == GetCurrentVersion() )
                {
                    settings->CopyTo( this );
                    m_Loaded.Raise( PreferencesLoadedArgs() );
                    return true;
                }
            }
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Save the preferences to the specified file on disc.
// 
bool Settings::SaveToFile( const Helium::Path& path, tstring& error, Reflect::VersionPtr version )
{
    bool result = false;

    if ( Platform::IsDebuggerPresent() )
    {
        Reflect::Archive::ToFile( this, path, version );
        result = true;
    }
    else
    {
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
    }

    return result;
}