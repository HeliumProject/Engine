#pragma once

#include "API.h"
#include "Core/Manipulator.h"
#include "Scene/TranslateManipulator.h"
#include "Editor/Preferences.h"
#include "Editor/WindowSettings.h"
#include "Editor/MRUData.h"

#include "Content/NodeVisibility.h"
#include "ViewPreferences.h"
#include "GridPreferences.h"

namespace Luna
{
  class ScenePreferences;

  // Returns the global pointer to the Scene Editor preferences.
  ScenePreferences* SceneEditorPreferences();

  /////////////////////////////////////////////////////////////////////////////
  // Structure representing all Scene Editor preferences.
  // 
  class ScenePreferences : public Preferences
  {
  private:
    WindowSettingsPtr m_SceneEditorWindowSettings;
    MRUDataPtr m_MRU;
    Content::NodeVisibilityPtr m_DefaultNodeVisibility; 
    ViewPreferencesPtr m_ViewPreferences; 
    GridPreferencesPtr m_GridPreferences;

    f32 m_ScaleManipulatorSize;

    f32 m_RotateManipulatorSize;
    bool m_RotateManipulatorAxisSnap;
    f32 m_RotateManipulatorSnapDegrees;
    ManipulatorSpace m_RotateManipulatorSpace;

    f32 m_TranslateManipulatorSize;
    ManipulatorSpace m_TranslateManipulatorSpace;
    TranslateSnappingMode m_TranslateManipulatorSnappingMode;
    f32 m_TranslateManipulatorDistance;
    bool m_TranslateManipulatorLiveObjectsOnly;

    bool m_ScaleManipulatorGridSnap;
    f32 m_ScaleManipulatorDistance;


    // RTTI
  public:
    static void InitializeType();
    static void CleanupType();
    REFLECT_DECLARE_CLASS( ScenePreferences, Preferences )
    static void EnumerateClass( Reflect::Compositor<ScenePreferences>& comp );

  public:
    ScenePreferences();

    virtual void PostDeserialize() NOC_OVERRIDE;

    virtual const std::string& GetCurrentVersion() const NOC_OVERRIDE;
    virtual std::string GetPreferencesPath() const NOC_OVERRIDE;

    WindowSettings* GetSceneEditorWindowSettings();

    MRUData* GetMRU();

    Content::NodeVisibility* GetDefaultNodeVisibility(); 

    ViewPreferences*  GetViewPreferences(); 
    ViewPreferencesPtr& GetViewPreferencesPtr();

    GridPreferences* GetGridPreferences();
    GridPreferencesPtr& GetGridPreferencesPtr();

    const Reflect::Field* ScaleManipulatorSize() const;

    const Reflect::Field* RotateManipulatorSize() const;
    const Reflect::Field* RotateManipulatorAxisSnap() const;
    const Reflect::Field* RotateManipulatorSnapDegrees() const;
    const Reflect::Field* RotateManipulatorSpace() const;

    const Reflect::Field* TranslateManipulatorSize() const;
    const Reflect::Field* TranslateManipulatorSpace() const;
    const Reflect::Field* TranslateManipulatorSnappingMode() const;
    const Reflect::Field* TranslateManipulatorDistance() const;
    const Reflect::Field* TranslateManipulatorLiveObjectsOnly() const;

    const Reflect::Field* ScaleManipulatorGridSnap() const;
    const Reflect::Field* ScaleManipulatorDistance() const;
  };
  typedef Nocturnal::SmartPtr< ScenePreferences > ScenePreferencesPtr;
}
