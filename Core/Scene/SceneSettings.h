#pragma once

#include "Core/API.h"
#include "Core/Content/NodeVisibility.h"
#include "Core/Scene/Manipulator.h"
#include "Core/Scene/TranslateManipulator.h"

namespace Helium
{
    namespace Core
    {
        class CORE_API SceneSettings : public Reflect::ConcreteInheritor< SceneSettings, Settings >
        {
        public:
            SceneSettings();

            MRUData* GetMRU();
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
            static void EnumerateClass( Reflect::Compositor<SceneSettings>& comp )
            {
                comp.AddField( &SceneSettings::m_MRU, "m_MRU" );
                comp.AddField( &SceneSettings::m_DefaultNodeVisibility, "m_DefaultNodeVisibility" );

                comp.AddField( &SceneSettings::m_ScaleManipulatorSize, "m_ScaleManipulatorSize" );
                comp.AddField( &SceneSettings::m_ScaleManipulatorGridSnap, "m_ScaleManipulatorGridSnap" );
                comp.AddField( &SceneSettings::m_ScaleManipulatorDistance, "m_ScaleManipulatorDistance" );

                comp.AddField( &SceneSettings::m_RotateManipulatorSize, "m_RotateManipulatorSize" );
                comp.AddField( &SceneSettings::m_RotateManipulatorAxisSnap, "m_RotateManipulatorAxisSnap" );
                comp.AddField( &SceneSettings::m_RotateManipulatorSnapDegrees, "m_RotateManipulatorSnapDegrees" );
                comp.AddEnumerationField( &SceneSettings::m_RotateManipulatorSpace, "m_RotateManipulatorSpace" );

                comp.AddField( &SceneSettings::m_TranslateManipulatorSize, "m_TranslateManipulatorSize" );
                comp.AddEnumerationField( &SceneSettings::m_TranslateManipulatorSpace, "m_TranslateManipulatorSpace" );
                comp.AddEnumerationField( &SceneSettings::m_TranslateManipulatorSnappingMode, "m_TranslateManipulatorSnappingMode" );
                comp.AddField( &SceneSettings::m_TranslateManipulatorDistance, "m_TranslateManipulatorDistance" );
                comp.AddField( &SceneSettings::m_TranslateManipulatorLiveObjectsOnly, "m_TranslateManipulatorLiveObjectsOnly" );
            }
        };
        typedef Helium::SmartPtr< SceneSettings > SceneSettingsPtr;
    }
}