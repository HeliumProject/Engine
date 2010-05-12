#pragma once

#include "API.h"
#include "Editor/Preferences.h"
#include "Editor/WindowSettings.h"
#include "Editor/MRUData.h"

namespace Luna
{
  class CharacterPreferences;

  // Returns the global pointer to the Character Editor preferences.
  CharacterPreferences* CharacterEditorPreferences();

  /////////////////////////////////////////////////////////////////////////////
  // Structure representing all Character Editor preferences.
  // 
  class CharacterPreferences : public Preferences
  {
  private:
    WindowSettingsPtr m_CharacterEditorWindowSettings;
    MRUDataPtr m_MRU;

    // RTTI
  public:
    static void InitializeType();
    static void CleanupType();
    REFLECT_DECLARE_CLASS( CharacterPreferences, Preferences )
    static void EnumerateClass( Reflect::Compositor<CharacterPreferences>& comp );

  public:
    CharacterPreferences();

    virtual void PostDeserialize() NOC_OVERRIDE;

    virtual const std::string& GetCurrentVersion() const NOC_OVERRIDE;
    virtual std::string GetPreferencesPath() const NOC_OVERRIDE;

    WindowSettings* GetCharacterEditorWindowSettings();

    MRUData* GetMRU();
  };
  typedef Nocturnal::SmartPtr< CharacterPreferences > CharacterPreferencesPtr;
}