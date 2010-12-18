#pragma once

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/Manipulator.h"
#include "Pipeline/SceneGraph/TranslateManipulator.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PIPELINE_API SceneSettings : public Reflect::Element
        {
        public:
            REFLECT_DECLARE_CLASS( SceneSettings, Reflect::Element );

            SceneSettings();

            float32_t ScaleManipulatorSize() const;
            bool ScaleManipulatorGridSnap() const;
            float32_t ScaleManipulatorDistance() const;

            float32_t RotateManipulatorSize() const;
            bool RotateManipulatorAxisSnap() const;
            float32_t RotateManipulatorSnapDegrees() const;
            ManipulatorSpace RotateManipulatorSpace() const;

            float32_t TranslateManipulatorSize() const;
            ManipulatorSpace TranslateManipulatorSpace() const;
            TranslateSnappingMode TranslateManipulatorSnappingMode() const;
            float32_t TranslateManipulatorDistance() const;
            bool TranslateManipulatorLiveObjectsOnly() const;

        private:
            float32_t m_ScaleManipulatorSize;
            bool m_ScaleManipulatorGridSnap;
            float32_t m_ScaleManipulatorDistance;

            float32_t m_RotateManipulatorSize;
            bool m_RotateManipulatorAxisSnap;
            float32_t m_RotateManipulatorSnapDegrees;
            ManipulatorSpace m_RotateManipulatorSpace;

            float32_t m_TranslateManipulatorSize;
            ManipulatorSpace m_TranslateManipulatorSpace;
            TranslateSnappingMode m_TranslateManipulatorSnappingMode;
            float32_t m_TranslateManipulatorDistance;
            bool m_TranslateManipulatorLiveObjectsOnly;

        public:
            static void EnumerateClass( Reflect::Compositor<SceneSettings>& comp )
            {
                comp.AddField( &SceneSettings::m_ScaleManipulatorSize, "Scale Manipulator Size" );
                comp.AddField( &SceneSettings::m_ScaleManipulatorGridSnap, "Scale Manipulator Grid Snapping" );
                comp.AddField( &SceneSettings::m_ScaleManipulatorDistance, "Scale Manipulator Distance" );

                comp.AddField( &SceneSettings::m_RotateManipulatorSize, "Rotate Manipulator Size" );
                comp.AddField( &SceneSettings::m_RotateManipulatorAxisSnap, "Rotate Manipulator Axis Snapping" );
                comp.AddField( &SceneSettings::m_RotateManipulatorSnapDegrees, "Rotate Manipulator Snap Degrees" );
                comp.AddEnumerationField( &SceneSettings::m_RotateManipulatorSpace, "Rotate Manipulator Space" );

                comp.AddField( &SceneSettings::m_TranslateManipulatorSize, "Translate Manipulator Size" );
                comp.AddEnumerationField( &SceneSettings::m_TranslateManipulatorSpace, "Translate Manipulator Space" );
                comp.AddEnumerationField( &SceneSettings::m_TranslateManipulatorSnappingMode, "Translate Manipulator Snapping Mode" );
                comp.AddField( &SceneSettings::m_TranslateManipulatorDistance, "Translate Manipulator Distance" );
                comp.AddField( &SceneSettings::m_TranslateManipulatorLiveObjectsOnly, "Translate Manipulator Live Objects Only" );
            }
        };
        typedef Helium::SmartPtr< SceneSettings > SceneSettingsPtr;
    }
}