#include "Precompile.h"

#include "PreferencesBase.h"

using namespace Luna;

REFLECT_DEFINE_ABSTRACT( PreferencesBase );

void PreferencesBase::EnumerateClass( Reflect::Compositor<PreferencesBase>& comp )
{
    Reflect::Field* fieldSavedVersion = comp.AddField( &PreferencesBase::m_SavedVersion, "m_SavedVersion", Reflect::FieldFlags::Force | Reflect::FieldFlags::Hide );
}



///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void PreferencesBase::InitializeType()
{
    Reflect::RegisterClass<PreferencesBase>( TXT( "PreferencesBase" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void PreferencesBase::CleanupType()
{
    Reflect::UnregisterClass<PreferencesBase>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
PreferencesBase::PreferencesBase()
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
PreferencesBase::~PreferencesBase()
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to store the current preferences version (provided by derived 
// classes) before saving.  Derived classes can change their version to 
// invalidate old preferences files.
// 
void PreferencesBase::PreSerialize()
{
    __super::PreSerialize();

    m_SavedVersion = GetCurrentVersion();
}

///////////////////////////////////////////////////////////////////////////////
// Loads the preference settings from a file on disc, if the file exists.  If
// the file could not be loaded (or the version was old), this function will 
// return false, and any values you query will be at the default settings.
// 
bool PreferencesBase::LoadFromFile( const tstring& path )
{
    Nocturnal::Path file( path );
    if ( file.Exists() )
    {
        try
        {
            // Only use the settings on disk if the version matches
            Reflect::VersionPtr version = Reflect::Archive::FromFile< Reflect::Version >( path );
            if ( version && version->IsCurrent() )
            {
                PreferencesBasePtr fromFile = Reflect::Archive::FromFile< PreferencesBase >( path );
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
bool PreferencesBase::SaveToFile( const tstring& path, tstring& error, Reflect::VersionPtr version )
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