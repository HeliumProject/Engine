#include "Precompile.h"
#include "AnimationEventsPreferences.h"
#include "AnimationEventsInit.h"
#include "Finder/LunaSpecs.h"

using namespace Luna;

REFLECT_DEFINE_CLASS( AnimationEventsPreferences );

void AnimationEventsPreferences::EnumerateClass( Reflect::Compositor<AnimationEventsPreferences>& comp )
{
  Reflect::ElementField* elemAnimationEventsWindowSettings = comp.AddField( &AnimationEventsPreferences::m_EventsWindowSettings, "m_EventsWindowSettings" );
  Reflect::ElementField* elemMRU = comp.AddField( &AnimationEventsPreferences::m_MRU, "m_MRU" );
}


///////////////////////////////////////////////////////////////////////////////
// Globals, statics, etc.
///////////////////////////////////////////////////////////////////////////////

// Pointer to the global Scene Editor preferences
AnimationEventsPreferencesPtr g_AnimationEventsEditorPreferences = NULL;

// Increment this value to invalidate all previously saved preferences
const static std::string s_PreferencesVersion( "1" );

// Increment this value to invalidate just the window settings for the Animation Events Editor
const static std::string s_WindowSettingsVersion( "4" );


///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void AnimationEventsPreferences::InitializeType()
{
  Luna::LunaAnimationEvents::g_RegisteredTypes.Push( Reflect::RegisterClass<AnimationEventsPreferences>( "AnimationEventsPreferences" ) );

  g_AnimationEventsEditorPreferences = new AnimationEventsPreferences();
  g_AnimationEventsEditorPreferences->LoadPreferences();
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void AnimationEventsPreferences::CleanupType()
{
  g_AnimationEventsEditorPreferences = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the global Asset Editor preferences.  You must call 
// AnimationEventsPreferences::InitializeType first.
// 
AnimationEventsPreferences* Luna::AnimationEventsEditorPreferences()
{
  if ( !g_AnimationEventsEditorPreferences.ReferencesObject() )
  {
    throw Nocturnal::Exception( "AnimationEventsEditorPreferences is not initialized, must call AnimationEventsPreferences::InitializeType first." );
  }

  return g_AnimationEventsEditorPreferences;
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AnimationEventsPreferences::AnimationEventsPreferences()
: m_EventsWindowSettings( new WindowSettings( s_WindowSettingsVersion ) )
, m_MRU( new MRUData() )
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to verify settings after load.
// 
void AnimationEventsPreferences::PostDeserialize()
{
  __super::PostDeserialize();

  WindowSettings::CheckWindowSettings( m_EventsWindowSettings, s_WindowSettingsVersion );

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
const std::string& AnimationEventsPreferences::GetCurrentVersion() const 
{
  return s_PreferencesVersion;
}

///////////////////////////////////////////////////////////////////////////////
// Path to load preferences from.
// 
std::string AnimationEventsPreferences::GetPreferencesPath() const
{
  return FinderSpecs::Luna::ANIMATION_EVENTS_EDITOR_PREFS.GetFile( FinderSpecs::Luna::PREFERENCES_FOLDER );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the Animation Events Editor's window settings.
// 
WindowSettings* AnimationEventsPreferences::GetAnimationEventsWindowSettings()
{
  return m_EventsWindowSettings.Ptr();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the Animation Events Editor's 'most recently used' settings.
// 
MRUData* AnimationEventsPreferences::GetMRU()
{
  return m_MRU;
}