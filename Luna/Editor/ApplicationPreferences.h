#pragma once

#include "Luna/API.h"

#include "Preferences.h"
#include "WindowSettings.h"

namespace Luna
{
  class ApplicationPreferences;

  // Global preferences for the Luna application
  LUNA_EDITOR_API ApplicationPreferences* GetApplicationPreferences();

  /////////////////////////////////////////////////////////////////////////////
  // Global preferences for Luna.
  // 
  class LUNA_EDITOR_API ApplicationPreferences : public Preferences
  {
  private:
    WindowSettingsPtr m_SessionFrameSettings;
    WindowSettingsPtr m_RunGameSettings;

    // RTTI
  public:
    static void InitializeType();
    static void CleanupType();
    REFLECT_DECLARE_CLASS( ApplicationPreferences, Preferences )
    static void EnumerateClass( Reflect::Compositor<ApplicationPreferences>& comp );

  public:
    ApplicationPreferences();

    virtual void PostDeserialize() NOC_OVERRIDE;

    virtual const tstring& GetCurrentVersion() const NOC_OVERRIDE;
    virtual tstring GetPreferencesPath() const NOC_OVERRIDE;

    WindowSettings* GetSessionFrameSettings();

    WindowSettings* GetRunGameSettings();
  };
  typedef Nocturnal::SmartPtr< ApplicationPreferences > ApplicationPreferencesPtr;
}