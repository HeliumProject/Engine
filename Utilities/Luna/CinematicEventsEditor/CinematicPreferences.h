#pragma once

#include "API.h"
#include "Editor/Preferences.h"
#include "Editor/WindowSettings.h"
#include "Editor/MRUData.h"

namespace Luna
{
  class CinematicPreferences;

  // Returns the global pointer to the Cinematic Editor preferences.
  CinematicPreferences* CinematicEditorPreferences();

  /////////////////////////////////////////////////////////////////////////////
  // Structure representing all Cinematic Editor preferences.
  // 
  class CinematicPreferences : public Preferences
  {
  private:
    WindowSettingsPtr m_CinematicEditorWindowSettings;
    MRUDataPtr m_MRU;

    // RTTI
  public:
    static void InitializeType();
    static void CleanupType();
    REFLECT_DECLARE_CLASS( CinematicPreferences, Preferences )
    static void EnumerateClass( Reflect::Compositor<CinematicPreferences>& comp );

  public:
    CinematicPreferences();

    virtual void PostDeserialize() NOC_OVERRIDE;

    virtual const std::string& GetCurrentVersion() const NOC_OVERRIDE;
    virtual std::string GetPreferencesPath() const NOC_OVERRIDE;

    WindowSettings* GetCinematicEditorWindowSettings();

    MRUData* GetMRU();
  };
  typedef Nocturnal::SmartPtr< CinematicPreferences > CinematicPreferencesPtr;
}