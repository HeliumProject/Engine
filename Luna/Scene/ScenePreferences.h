#pragma once

#include "Pipeline/Content/NodeVisibility.h"

#include "Luna/API.h"
#include "Luna/Scene/Manipulator.h"
#include "Luna/Scene/TranslateManipulator.h"
#include "Luna/WindowSettings.h"

namespace Luna
{
    class ScenePreferences : public Reflect::ConcreteInheritor< ScenePreferences, Settings >
    {
    public:
        ScenePreferences();

        MRUData* GetMRU();
        WindowSettings* GetWindowSettings();
        Content::NodeVisibility* GetDefaultNodeVisibility(); 

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

    private:
        MRUDataPtr m_MRU;
        WindowSettingsPtr m_WindowSettings;
        Content::NodeVisibilityPtr m_DefaultNodeVisibility; 

        f32 m_ScaleManipulatorSize;
        bool m_ScaleManipulatorGridSnap;
        f32 m_ScaleManipulatorDistance;

        f32 m_RotateManipulatorSize;
        bool m_RotateManipulatorAxisSnap;
        f32 m_RotateManipulatorSnapDegrees;
        ManipulatorSpace m_RotateManipulatorSpace;

        f32 m_TranslateManipulatorSize;
        ManipulatorSpace m_TranslateManipulatorSpace;
        TranslateSnappingMode m_TranslateManipulatorSnappingMode;
        f32 m_TranslateManipulatorDistance;
        bool m_TranslateManipulatorLiveObjectsOnly;

    public:
        static void EnumerateClass( Reflect::Compositor<ScenePreferences>& comp )
        {
            comp.AddField( &ScenePreferences::m_WindowSettings, "m_WindowSettings" );
            comp.AddField( &ScenePreferences::m_MRU, "m_MRU" );
            comp.AddField( &ScenePreferences::m_DefaultNodeVisibility, "m_DefaultNodeVisibility" );

            comp.AddField( &ScenePreferences::m_ScaleManipulatorSize, "m_ScaleManipulatorSize" );
            comp.AddField( &ScenePreferences::m_ScaleManipulatorGridSnap, "m_ScaleManipulatorGridSnap" );
            comp.AddField( &ScenePreferences::m_ScaleManipulatorDistance, "m_ScaleManipulatorDistance" );

            comp.AddField( &ScenePreferences::m_RotateManipulatorSize, "m_RotateManipulatorSize" );
            comp.AddField( &ScenePreferences::m_RotateManipulatorAxisSnap, "m_RotateManipulatorAxisSnap" );
            comp.AddField( &ScenePreferences::m_RotateManipulatorSnapDegrees, "m_RotateManipulatorSnapDegrees" );
            comp.AddEnumerationField( &ScenePreferences::m_RotateManipulatorSpace, "m_RotateManipulatorSpace" );

            comp.AddField( &ScenePreferences::m_TranslateManipulatorSize, "m_TranslateManipulatorSize" );
            comp.AddEnumerationField( &ScenePreferences::m_TranslateManipulatorSpace, "m_TranslateManipulatorSpace" );
            comp.AddEnumerationField( &ScenePreferences::m_TranslateManipulatorSnappingMode, "m_TranslateManipulatorSnappingMode" );
            comp.AddField( &ScenePreferences::m_TranslateManipulatorDistance, "m_TranslateManipulatorDistance" );
            comp.AddField( &ScenePreferences::m_TranslateManipulatorLiveObjectsOnly, "m_TranslateManipulatorLiveObjectsOnly" );
        }
    };
    typedef Nocturnal::SmartPtr< ScenePreferences > ScenePreferencesPtr;
}
