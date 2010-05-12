#include "Precompile.h"
#include "CinematicPreferences.h"
#include "CinematicEventsInit.h"
#include "Finder/LunaSpecs.h"

using namespace Luna;

REFLECT_DEFINE_CLASS( CinematicPreferences );

void CinematicPreferences::EnumerateClass( Reflect::Compositor<CinematicPreferences>& comp )
{
  Reflect::ElementField* elemCinematicEditorWindowSettings = comp.AddField( &CinematicPreferences::m_CinematicEditorWindowSettings, "m_CinematicEditorWindowSettings" );
  Reflect::ElementField* elemMRU = comp.AddField( &CinematicPreferences::m_MRU, "m_MRU" );
}


///////////////////////////////////////////////////////////////////////////////
// Globals, statics, etc.
///////////////////////////////////////////////////////////////////////////////

// Pointer to the global Scene Editor preferences
CinematicPreferencesPtr g_CinematicEditorPreferences = NULL;

// Increment this value to invalidate all previously saved preferences
const static std::string s_PreferencesVersion( "1" );

// Increment this value to invalidate just the window settings for the Cinematic Events Editor
const static std::string s_WindowSettingsVersion( "4" );


///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void CinematicPreferences::InitializeType()
{
  Luna::LunaCinematicEvents::g_RegisteredTypes.Push( Reflect::RegisterClass<CinematicPreferences>( "CinematicPreferences" ) );

  g_CinematicEditorPreferences = new CinematicPreferences();
  g_CinematicEditorPreferences->LoadPreferences();
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void CinematicPreferences::CleanupType()
{
  g_CinematicEditorPreferences = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the global Cinematic Editor preferences.  You must call 
// CinematicPreferences::InitializeType first.
// 
CinematicPreferences* Luna::CinematicEditorPreferences()
{
  if ( !g_CinematicEditorPreferences.ReferencesObject() )
  {
    throw Nocturnal::Exception( "CinematicEditorPreferences is not initialized, must call CinematicPreferences::InitializeType first." );
  }

  return g_CinematicEditorPreferences;
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
CinematicPreferences::CinematicPreferences()
: m_CinematicEditorWindowSettings( new WindowSettings( s_WindowSettingsVersion ) )
, m_MRU( new MRUData() )
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to verify settings after load.
// 
void CinematicPreferences::PostDeserialize()
{
  __super::PostDeserialize();

  WindowSettings::CheckWindowSettings( m_CinematicEditorWindowSettings, s_WindowSettingsVersion );

  if ( !m_MRU.ReferencesObject() )
  {
    m_MRU = new MRUData();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the current version of the preferences.  Changing this value 
// invalidates all previously saved preferences.  You can also invalidate
// separate aspects of the preferences.  See the globals section at the top
// of this file.
// 
const std::string& CinematicPreferences::GetCurrentVersion() const 
{
  return s_PreferencesVersion;
}

///////////////////////////////////////////////////////////////////////////////
// Load preferences.
// 
std::string CinematicPreferences::GetPreferencesPath() const
{
  return FinderSpecs::Luna::CINEMATIC_EVENTS_EDITOR_PREFS.GetFile( FinderSpecs::Luna::PREFERENCES_FOLDER );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the Cinematic Editor's window settings.
// 
WindowSettings* CinematicPreferences::GetCinematicEditorWindowSettings()
{
  return m_CinematicEditorWindowSettings.Ptr();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the Cinematic Editor's 'most recently used' settings.
// 
MRUData* CinematicPreferences::GetMRU()
{
  return m_MRU;
}