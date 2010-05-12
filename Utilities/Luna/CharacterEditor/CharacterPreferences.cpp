#include "Precompile.h"
#include "CharacterPreferences.h"
#include "CharacterInit.h"
#include "Finder/LunaSpecs.h"

using namespace Luna;

REFLECT_DEFINE_CLASS( CharacterPreferences );

void CharacterPreferences::EnumerateClass( Reflect::Compositor<CharacterPreferences>& comp )
{
  Reflect::ElementField* elemCharacterEditorWindowSettings = comp.AddField( &CharacterPreferences::m_CharacterEditorWindowSettings, "m_CharacterEditorWindowSettings" );
  Reflect::ElementField* elemMRU = comp.AddField( &CharacterPreferences::m_MRU, "m_MRU" );
}


///////////////////////////////////////////////////////////////////////////////
// Globals, statics, etc.
///////////////////////////////////////////////////////////////////////////////

// Pointer to the global Scene Editor preferences
CharacterPreferencesPtr g_CharacterEditorPreferences = NULL;

// Increment this value to invalidate all previously saved preferences
const static std::string s_PreferencesVersion( "1" );

// Increment this value to invalidate just the window settings for the Character Editor
const static std::string s_WindowSettingsVersion( "3" );


///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void CharacterPreferences::InitializeType()
{
  Luna::LunaCharacter::g_RegisteredTypes.Push( Reflect::RegisterClass<CharacterPreferences>( "CharacterPreferences" ) );

  g_CharacterEditorPreferences = new CharacterPreferences();
  g_CharacterEditorPreferences->LoadPreferences();
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void CharacterPreferences::CleanupType()
{
  g_CharacterEditorPreferences = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the global Character Editor preferences.  You must call 
// CharacterPreferences::InitializeType first.
// 
CharacterPreferences* Luna::CharacterEditorPreferences()
{
  if ( !g_CharacterEditorPreferences.ReferencesObject() )
  {
    throw Nocturnal::Exception( "CharacterEditorPreferences is not initialized, must call CharacterPreferences::InitializeType first." );
  }

  return g_CharacterEditorPreferences;
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
CharacterPreferences::CharacterPreferences()
: m_CharacterEditorWindowSettings( new WindowSettings( s_WindowSettingsVersion ) )
, m_MRU( new MRUData() )
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to verify settings after load.
// 
void CharacterPreferences::PostDeserialize()
{
  __super::PostDeserialize();

  WindowSettings::CheckWindowSettings( m_CharacterEditorWindowSettings, s_WindowSettingsVersion );

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
const std::string& CharacterPreferences::GetCurrentVersion() const 
{
  return s_PreferencesVersion;
}

///////////////////////////////////////////////////////////////////////////////
// Load preferences.
// 
std::string CharacterPreferences::GetPreferencesPath() const
{
  return FinderSpecs::Luna::CHARACTER_EDITOR_PREFS.GetFile( FinderSpecs::Luna::PREFERENCES_FOLDER );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the Character Editor's window settings.
// 
WindowSettings* CharacterPreferences::GetCharacterEditorWindowSettings()
{
  return m_CharacterEditorWindowSettings.Ptr();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the 'most recently used' information for this editor.
// 
MRUData* CharacterPreferences::GetMRU()
{
  return m_MRU;
}