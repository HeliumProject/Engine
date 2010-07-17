#include "Precompile.h"
#include "AppPreferences.h"
#include "Application/Preferences.h"

using namespace Luna;

REFLECT_DEFINE_CLASS( AppPreferences )

void AppPreferences::EnumerateClass( Reflect::Compositor<AppPreferences>& comp )
{
  Reflect::ElementField* elemSessionFrameSettings = comp.AddField( &AppPreferences::m_SessionFrameSettings, "m_SessionFrameSettings" );
}


///////////////////////////////////////////////////////////////////////////////
// Globals, statics, etc.
///////////////////////////////////////////////////////////////////////////////

// Pointer to the global Scene Editor preferences
ApplicationPreferencesPtr g_ApplicationPreferences = NULL;

// Increment this value to invalidate all previously saved preferences
const static tstring s_PreferencesVersion( TXT( "1" ) );

// Increment this value to invalidate just the window settings for the Session Frame
const static tstring s_SessionFrameVersion( TXT( "1" ) );

// Increment this value to invalidate just the window settings for the Run Game window
const static tstring s_RunGameWindowVersion( TXT( "1" ) );


///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void AppPreferences::InitializeType()
{
  Reflect::RegisterClass<AppPreferences>( TXT( "AppPreferences" ) );

  NOC_ASSERT( !g_ApplicationPreferences );

  g_ApplicationPreferences = new AppPreferences();
  g_ApplicationPreferences->LoadPreferences();
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void AppPreferences::CleanupType()
{
  Reflect::UnregisterClass<AppPreferences>();

  g_ApplicationPreferences = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the one and only instance of this class.
// 
AppPreferences* Luna::GetApplicationPreferences()
{
  if ( !g_ApplicationPreferences )
  {
    throw Nocturnal::Exception( TXT( "AppPreferences is not initialized, must call AppPreferences::InitializeType first." ) );
  }

  return g_ApplicationPreferences;
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AppPreferences::AppPreferences()
: m_SessionFrameSettings( new WindowSettings( s_SessionFrameVersion ) )
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to verify settings after load.
// 
void AppPreferences::PostDeserialize()
{
  __super::PostDeserialize();
  WindowSettings::CheckWindowSettings( m_SessionFrameSettings, s_SessionFrameVersion );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the current version of the preferences.  Changing this value 
// invalidates all previously saved preferences.  You can also invalidate
// separate aspects of the preferences.  See the globals section at the top
// of this file.
// 
const tstring& AppPreferences::GetCurrentVersion() const 
{
  return s_PreferencesVersion;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the path to this preference file on disk.
// 
tstring AppPreferences::GetPreferencesPath() const
{
    Nocturnal::Path prefsDir;
    if ( !Application::GetPreferencesDirectory( prefsDir ) )
    {
        throw Nocturnal::Exception( TXT( "Could not get preferences directory." ) );
    }
    return prefsDir.Get() + TXT( "LunaGlobalPrefs.nrb" );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the window settings for the Session Frame.
// 
WindowSettings* AppPreferences::GetSessionFrameSettings()
{
  return m_SessionFrameSettings;
}
