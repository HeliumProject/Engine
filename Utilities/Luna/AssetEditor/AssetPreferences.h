#pragma once

#include "Luna/API.h"
#include "Editor/Preferences.h"
#include "Editor/WindowSettings.h"
#include "Editor/MRUData.h"

namespace Luna
{
  class AssetPreferences;

  // Returns the global pointer to the Asset Editor preferences.
  AssetPreferences* GetAssetEditorPreferences();

  /////////////////////////////////////////////////////////////////////////////
  // Structure representing all Asset Editor preferences.
  // 
  class AssetPreferences : public Preferences
  {
  private:
    WindowSettingsPtr m_AssetEditorWindowSettings;
    WindowSettingsPtr m_AnimGroupDlgWindowSettings;
    WindowSettingsPtr m_ComponentChooserDlgWindowSettings;
    std::string m_ComponentChooserTab;
    FilePathOption  m_FilePathOption;
    MRUDataPtr m_MRU;
    bool m_DisplayPreviewAxis;

    // RTTI
  public:
    static void InitializeType();
    static void CleanupType();
    REFLECT_DECLARE_CLASS( AssetPreferences, Preferences )
    static void EnumerateClass( Reflect::Compositor<AssetPreferences>& comp );

  public:
    AssetPreferences();

    virtual void PostDeserialize() NOC_OVERRIDE;

    virtual const std::string& GetCurrentVersion() const NOC_OVERRIDE;
    virtual std::string GetPreferencesPath() const NOC_OVERRIDE;

    const Reflect::Field* ComponentChooserTab() const;

    const Reflect::Field* FilePathOption() const;

    WindowSettings* GetAssetEditorWindowSettings();
    WindowSettings* GetAnimGroupDlgWindowSettings();
    WindowSettings* GetComponentChooserDlgWindowSettings();
    MRUData* GetMRU();

    bool DisplayPreviewAxis() const;
    void SetDisplayPreviewAxis( bool display );
    const Reflect::Field* DisplayPreviewAxisField() const;
  };
  typedef Nocturnal::SmartPtr< AssetPreferences > AssetPreferencesPtr;
}