/*#include "Precompile.h"*/
#include "SceneSettings.h"

#include "Foundation/Preferences.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_CLASS( SceneSettings );

SceneSettings::SceneSettings()
: m_ScaleManipulatorSize( 0.3f )
, m_RotateManipulatorSize( 0.3f )
, m_RotateManipulatorAxisSnap( false )
, m_RotateManipulatorSnapDegrees( 15.0f )
, m_RotateManipulatorSpace( ManipulatorSpace::Object )
, m_TranslateManipulatorSize( 0.3f )
, m_TranslateManipulatorSpace( ManipulatorSpace::Object )
, m_TranslateManipulatorSnappingMode( TranslateSnappingMode::None )
, m_TranslateManipulatorDistance( 1.0f )
, m_TranslateManipulatorLiveObjectsOnly( false )
, m_ScaleManipulatorGridSnap( false )
, m_ScaleManipulatorDistance( 1.0f )
{
}

float32_t SceneSettings::ScaleManipulatorSize() const
{
    return m_ScaleManipulatorSize;
}

bool SceneSettings::ScaleManipulatorGridSnap() const
{
    return m_ScaleManipulatorGridSnap;
}

float32_t SceneSettings::ScaleManipulatorDistance() const
{
    return m_ScaleManipulatorDistance;
}

float32_t SceneSettings::RotateManipulatorSize() const
{
    return m_RotateManipulatorSize;
}

bool SceneSettings::RotateManipulatorAxisSnap() const
{
    return m_RotateManipulatorAxisSnap;
}

float32_t SceneSettings::RotateManipulatorSnapDegrees() const
{
    return m_RotateManipulatorSnapDegrees;
}

ManipulatorSpace SceneSettings::RotateManipulatorSpace() const
{
    return m_RotateManipulatorSpace;
}

float32_t SceneSettings::TranslateManipulatorSize() const
{
    return m_TranslateManipulatorSize;
}

ManipulatorSpace SceneSettings::TranslateManipulatorSpace() const
{
    return m_TranslateManipulatorSpace;
}

TranslateSnappingMode SceneSettings::TranslateManipulatorSnappingMode() const
{
    return m_TranslateManipulatorSnappingMode;
}

float32_t SceneSettings::TranslateManipulatorDistance() const
{
    return m_TranslateManipulatorDistance;
}

bool SceneSettings::TranslateManipulatorLiveObjectsOnly() const
{
    return m_TranslateManipulatorLiveObjectsOnly;
}
