#include "Precompile.h"
#include "ApplicationPreferences.h"
#include "Application/Preferences.h"
#include "Finder/LunaSpecs.h"

using namespace Luna;

REFLECT_DEFINE_CLASS( ApplicationPreferences )

void ApplicationPreferences::EnumerateClass( Reflect::Compositor<ApplicationPreferences>& comp )
{
  Reflect::ElementField* elemSessionFrameSettings = comp.AddField( &ApplicationPreferences::m_SessionFrameSettings, "m_SessionFrameSettings" );
  Reflect::ElementField* elemRunGameSettings = comp.AddField( &ApplicationPreferences::m_RunGameSettings, "m_RunGameSettings" );
}


///////////////////////////////////////////////////////////////////////////////
// Globals, statics, etc.
///////////////////////////////////////////////////////////////////////////////

// Pointer to the global Scene Editor preferences
ApplicationPreferencesPtr g_ApplicationPreferences = NULL;

// Increment this value to invalidate all previously saved preferences
const static std::string s_PreferencesVersion( "1" );

// Increment this value to invalidate just the window settings for the Session Frame
const static std::string s_SessionFrameVersion( "1" );

// Increment this value to invalidate just the window settings for the Run Game window
const static std::string s_RunGameWindowVersion( "1" );


///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ApplicationPreferences::InitializeType()
{
  Reflect::RegisterClass<ApplicationPreferences>( "ApplicationPreferences" );

  NOC_ASSERT( !g_ApplicationPreferences );

  g_ApplicationPreferences = new ApplicationPreferences();
  g_ApplicationPreferences->LoadPreferences();
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ApplicationPreferences::CleanupType()
{
  Reflect::UnregisterClass<ApplicationPreferences>();

  g_ApplicationPreferences = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the one and only instance of this class.
// 
ApplicationPreferences* Luna::GetApplicationPreferences()
{
  if ( !g_ApplicationPreferences )
  {
    throw Nocturnal::Exception( "ApplicationPreferences is not initialized, must call ApplicationPreferences::InitializeType first." );
  }

  return g_ApplicationPreferences;
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ApplicationPreferences::ApplicationPreferences()
: m_SessionFrameSettings( new WindowSettings( s_SessionFrameVersion ) )
, m_RunGameSettings( new WindowSettings( s_RunGameWindowVersion ) )
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to verify settings after load.
// 
void ApplicationPreferences::PostDeserialize()
{
  __super::PostDeserialize();
  WindowSettings::CheckWindowSettings( m_SessionFrameSettings, s_SessionFrameVersion );
  WindowSettings::CheckWindowSettings( m_RunGameSettings, s_RunGameWindowVersion );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the current version of the preferences.  Changing this value 
// invalidates all previously saved preferences.  You can also invalidate
// separate aspects of the preferences.  See the globals section at the top
// of this file.
// 
const std::string& ApplicationPreferences::GetCurrentVersion() const 
{
  return s_PreferencesVersion;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the path to this preference file on disk.
// 
std::string ApplicationPreferences::GetPreferencesPath() const
{
    Nocturnal::Path prefsDir;
    if ( !Application::GetPreferencesDirectory( prefsDir ) )
    {
        throw Nocturnal::Exception( "Could not get preferences directory." );
    }
    return FinderSpecs::Luna::GLOBAL_PREFS.GetFile( prefsDir.Get() );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the window settings for the Session Frame.
// 
WindowSettings* ApplicationPreferences::GetSessionFrameSettings()
{
  return m_SessionFrameSettings;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the window settings for the Run Game window.
// 
WindowSettings* ApplicationPreferences::GetRunGameSettings()
{
  return m_RunGameSettings;
}
