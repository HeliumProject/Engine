#pragma once

#include "Core/API.h"
#include "Core/SceneGraph/Manipulator.h"
#include "Core/SceneGraph/TranslateManipulator.h"
#include "Core/MRUData.h"

namespace Helium
{
    namespace SceneGraph
    {
        class CORE_API SceneSettings : public Reflect::ConcreteInheritor< SceneSettings, Reflect::Element >
        {
        public:
            SceneSettings();

            MRUData* GetMRU();

            f32 ScaleManipulatorSize() const;
            bool ScaleManipulatorGridSnap() const;
            f32 ScaleManipulatorDistance() const;

            f32 RotateManipulatorSize() const;
            bool RotateManipulatorAxisSnap() const;
            f32 RotateManipulatorSnapDegrees() const;
            ManipulatorSpace RotateManipulatorSpace() const;

            f32 TranslateManipulatorSize() const;
            ManipulatorSpace TranslateManipulatorSpace() const;
            TranslateSnappingMode TranslateManipulatorSnappingMode() const;
            f32 TranslateManipulatorDistance() const;
            bool TranslateManipulatorLiveObjectsOnly() const;

        private:
            MRUDataPtr m_MRU;

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