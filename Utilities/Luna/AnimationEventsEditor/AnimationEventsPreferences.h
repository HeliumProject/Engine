#pragma once

#include "API.h"
#include "Editor/Preferences.h"
#include "Editor/WindowSettings.h"
#include "Editor/MRUData.h"

namespace Luna
{
  class AnimationEventsPreferences;

  // Returns the global pointer to the Asset Editor preferences.
  AnimationEventsPreferences* AnimationEventsEditorPreferences();

  /////////////////////////////////////////////////////////////////////////////
  // Structure representing all Asset Editor preferences.
  // 
  class AnimationEventsPreferences : public Preferences
  {
  private:
    WindowSettingsPtr m_EventsWindowSettings;
    MRUDataPtr m_MRU;

    // RTTI
  public:
    static void InitializeType();
    static void CleanupType();
    REFLECT_DECLARE_CLASS( AnimationEventsPreferences, Preferences )
    static void EnumerateClass( Reflect::Compositor<AnimationEventsPreferences>& comp );

  public:
    AnimationEventsPreferences();

    virtual void PostDeserialize() NOC_OVERRIDE;

    virtual const std::string& GetCurrentVersion() const NOC_OVERRIDE;
    virtual std::string GetPreferencesPath() const NOC_OVERRIDE;

    WindowSettings* GetAnimationEventsWindowSettings();

    MRUData* GetMRU();
  };
  typedef Nocturnal::SmartPtr< AnimationEventsPreferences > AnimationEventsPreferencesPtr;
}