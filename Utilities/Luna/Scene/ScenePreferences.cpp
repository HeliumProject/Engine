#include "Precompile.h"
#include "ScenePreferences.h"

#include "Application/Preferences.h"

using namespace Luna;

REFLECT_DEFINE_CLASS( ScenePreferences );

void ScenePreferences::EnumerateClass( Reflect::Compositor<ScenePreferences>& comp )
{
  Reflect::ElementField* elemSceneEditorWindowSettings = comp.AddField( &ScenePreferences::m_SceneEditorWindowSettings, "m_SceneEditorWindowSettings" );
  Reflect::ElementField* elemMRU = comp.AddField( &ScenePreferences::m_MRU, "m_MRU" );
  Reflect::ElementField* elemDefaultNodeVisibility = comp.AddField( &ScenePreferences::m_DefaultNodeVisibility, "m_DefaultNodeVisibility" );
  Reflect::ElementField* elemViewPreferences = comp.AddField( &ScenePreferences::m_ViewPreferences, "m_ViewPreferences" );
  Reflect::ElementField* elemGridPreferences = comp.AddField( &ScenePreferences::m_GridPreferences, "m_GridPreferences" );

  Reflect::Field* scaleManipSize = comp.AddField( &ScenePreferences::m_ScaleManipulatorSize, "m_ScaleManipulatorSize" );

  Reflect::Field* rotateManipSize = comp.AddField( &ScenePreferences::m_RotateManipulatorSize, "m_RotateManipulatorSize" );
  Reflect::Field* rotateManipAxisSnap = comp.AddField( &ScenePreferences::m_RotateManipulatorAxisSnap, "m_RotateManipulatorAxisSnap" );
  Reflect::Field* rotateManipSnapDegrees = comp.AddField( &ScenePreferences::m_RotateManipulatorSnapDegrees, "m_RotateManipulatorSnapDegrees" );
  Reflect::Field* rotateManipSpace = comp.AddEnumerationField( &ScenePreferences::m_RotateManipulatorSpace, "m_RotateManipulatorSpace" );

  Reflect::Field* translateManipSize = comp.AddField( &ScenePreferences::m_TranslateManipulatorSize, "m_TranslateManipulatorSize" );
  Reflect::Field* translateManipSpace = comp.AddEnumerationField( &ScenePreferences::m_TranslateManipulatorSpace, "m_TranslateManipulatorSpace" );
  Reflect::Field* translateManipSnappingMode = comp.AddEnumerationField( &ScenePreferences::m_TranslateManipulatorSnappingMode, "m_TranslateManipulatorSnappingMode" );
  Reflect::Field* translateManipGridSnap = comp.AddField( &ScenePreferences::m_TranslateManipulatorDistance, "m_TranslateManipulatorDistance" );
  Reflect::Field* translateManipLiveObjOnly = comp.AddField( &ScenePreferences::m_TranslateManipulatorLiveObjectsOnly, "m_TranslateManipulatorLiveObjectsOnly" );
  
  Reflect::Field* scaleManipGridSnap = comp.AddField( &ScenePreferences::m_ScaleManipulatorGridSnap, "m_ScaleManipulatorGridSnap" );
  Reflect::Field* scaleManipDistance = comp.AddField( &ScenePreferences::m_ScaleManipulatorDistance, "m_ScaleManipulatorDistance" );
}


///////////////////////////////////////////////////////////////////////////////
// Globals, statics, etc.
///////////////////////////////////////////////////////////////////////////////

// Pointer to the global Scene Editor preferences
ScenePreferencesPtr g_SceneEditorPreferences = NULL;

// Increment this value to invalidate all previously saved preferences
const static std::string s_PreferencesVersion( "1" );

// Increment this value to invalidate just the window settings for the Scene Editor
const static std::string s_WindowSettingsVersion( "10" );


///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ScenePreferences::InitializeType()
{
  g_SceneEditorPreferences = new ScenePreferences();
  g_SceneEditorPreferences->LoadPreferences();
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ScenePreferences::CleanupType()
{
  g_SceneEditorPreferences = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the global Scene Editor preferences.  You must call 
// ScenePreferences::InitializeType first.
// 
ScenePreferences* Luna::SceneEditorPreferences()
{
  if ( !g_SceneEditorPreferences.ReferencesObject() )
  {
    throw Nocturnal::Exception( "SceneEditorPreferences is not initialized, must call ScenePreferences::InitializeType first." );
  }

  return g_SceneEditorPreferences;
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ScenePreferences::ScenePreferences()
: m_SceneEditorWindowSettings( new WindowSettings( s_WindowSettingsVersion ) )
, m_MRU( new MRUData() )
, m_DefaultNodeVisibility( new Content::NodeVisibility() )
, m_ViewPreferences( new ViewPreferences() )
, m_GridPreferences( new GridPreferences() )
, m_ScaleManipulatorSize( 0.3f )
, m_RotateManipulatorSize( 0.3f )
, m_RotateManipulatorAxisSnap( false )
, m_RotateManipulatorSnapDegrees( 15.0f )
, m_RotateManipulatorSpace( ManipulatorSpaces::Object )
, m_TranslateManipulatorSize( 0.3f )
, m_TranslateManipulatorSpace( ManipulatorSpaces::Object )
, m_TranslateManipulatorSnappingMode( TranslateSnappingModes::None )
, m_TranslateManipulatorDistance( 1.0f )
, m_TranslateManipulatorLiveObjectsOnly( false )
, m_ScaleManipulatorGridSnap( false )
, m_ScaleManipulatorDistance( 1.0f )
{
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to verify settings after load.
// 
void ScenePreferences::PostDeserialize()
{
  __super::PostDeserialize();

  WindowSettings::CheckWindowSettings( m_SceneEditorWindowSettings, s_WindowSettingsVersion );

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
const std::string& ScenePreferences::GetCurrentVersion() const 
{
  return s_PreferencesVersion;
}

///////////////////////////////////////////////////////////////////////////////
// Path to load these preferences from.
// 
std::string ScenePreferences::GetPreferencesPath() const
{
    Nocturnal::Path prefsDir;
    if ( Application::GetPreferencesDirectory( prefsDir ) )
    {
        return prefsDir.Get() + "LunaSceneEditorPrefs.rb";
    }
    return "";
}

///////////////////////////////////////////////////////////////////////////////
// Returns the Scene Editor's window settings.
// 
WindowSettings* ScenePreferences::GetSceneEditorWindowSettings()
{
  return m_SceneEditorWindowSettings.Ptr();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the Scene Editor's 'most recently used' data.
// 
MRUData* ScenePreferences::GetMRU()
{
  return m_MRU;
}

Content::NodeVisibility* ScenePreferences::GetDefaultNodeVisibility()
{
  return m_DefaultNodeVisibility; 
}

ViewPreferences* ScenePreferences::GetViewPreferences()
{
  return m_ViewPreferences; 
}

ViewPreferencesPtr& ScenePreferences::GetViewPreferencesPtr()
{
  return m_ViewPreferences; 
}

GridPreferences* ScenePreferences::GetGridPreferences()
{
  return m_GridPreferences; 
}

GridPreferencesPtr& ScenePreferences::GetGridPreferencesPtr()
{
  return m_GridPreferences;
}

const Reflect::Field* ScenePreferences::ScaleManipulatorSize() const
{
  return GetClass()->FindField( &ScenePreferences::m_ScaleManipulatorSize );
}

const Reflect::Field* ScenePreferences::RotateManipulatorSize() const
{
  return GetClass()->FindField( &ScenePreferences::m_RotateManipulatorSize );
}

const Reflect::Field* ScenePreferences::RotateManipulatorAxisSnap() const
{
  return GetClass()->FindField( &ScenePreferences::m_RotateManipulatorAxisSnap );
}

const Reflect::Field* ScenePreferences::RotateManipulatorSnapDegrees() const
{
  return GetClass()->FindField( &ScenePreferences::m_RotateManipulatorSnapDegrees );
}

const Reflect::Field* ScenePreferences::RotateManipulatorSpace() const
{
  return GetClass()->FindField( &ScenePreferences::m_RotateManipulatorSpace );
}

const Reflect::Field* ScenePreferences::TranslateManipulatorSize() const
{
  return GetClass()->FindField( &ScenePreferences::m_TranslateManipulatorSize );
}

const Reflect::Field* ScenePreferences::TranslateManipulatorSpace() const
{
  return GetClass()->FindField( &ScenePreferences::m_TranslateManipulatorSpace );
}

const Reflect::Field* ScenePreferences::TranslateManipulatorSnappingMode() const
{
  return GetClass()->FindField( &ScenePreferences::m_TranslateManipulatorSnappingMode );
}

const Reflect::Field* ScenePreferences::TranslateManipulatorDistance() const
{
  return GetClass()->FindField( &ScenePreferences::m_TranslateManipulatorDistance );
}

const Reflect::Field* ScenePreferences::TranslateManipulatorLiveObjectsOnly() const
{
  return GetClass()->FindField( &ScenePreferences::m_TranslateManipulatorLiveObjectsOnly );
}

const Reflect::Field* ScenePreferences::ScaleManipulatorGridSnap() const
{
  return GetClass()->FindField( &ScenePreferences::m_ScaleManipulatorGridSnap );
}

const Reflect::Field* ScenePreferences::ScaleManipulatorDistance() const
{
  return GetClass()->FindField( &ScenePreferences::m_ScaleManipulatorDistance );
}
