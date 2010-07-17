#pragma once

#include "Luna/API.h"

#include "Preferences.h"
#include "WindowSettings.h"

namespace Luna
{
  class AppPreferences;

  // Global preferences for the Luna application
  LUNA_EDITOR_API AppPreferences* GetApplicationPreferences();

  /////////////////////////////////////////////////////////////////////////////
  // Global preferences for Luna.
  // 
  class LUNA_EDITOR_API AppPreferences : public Reflect::ConcreteInheritor< AppPreferences, Preferences >
  {
  public:
    static void EnumerateClass( Reflect::Compositor<AppPreferences>& comp );

    // RTTI
  public:
    static void InitializeType();
    static void CleanupType();

  public:
    AppPreferences();

    virtual void PostDeserialize() NOC_OVERRIDE;

    virtual const tstring& GetCurrentVersion() const NOC_OVERRIDE;
    virtual tstring GetPreferencesPath() const NOC_OVERRIDE;

    WindowSettings* GetSessionFrameSettings();

  private:
    WindowSettingsPtr m_SessionFrameSettings;
  };
  typedef Nocturnal::SmartPtr< AppPreferences > ApplicationPreferencesPtr;
}