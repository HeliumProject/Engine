/*#include "Precompile.h"*/
#include "SceneSettings.h"

#include "Foundation/Preferences.h"

using namespace Helium;
using namespace Helium::SceneGraph;

SceneSettings::SceneSettings()
: m_MRU( new MRUData() )
, m_ScaleManipulatorSize( 0.3f )
, m_RotateManipulatorSize( 0.3f )
, m_RotateManipulatorAxisSnap( false )
, m_RotateManipulatorSnapDegrees( 15.0f )
, m_RotateManipulatorSpace( ManipulatorSpaces::Object )
, m_TranslateManipulatorSize( 0.3f )
, m_TranslateManipulatorSpace( ManipulatorSpaces::Object )
, m_TranslateManipulatorSnappingMode( TranslateSnappingModes::None )
, m_TranslateManipulatorDistance( 1.0f )
, m_TranslateManipulatorLiveObjectsOnly( false )
, m_ScaleManipulatorGridSnap( false )
, m_ScaleManipulatorDistance( 1.0f )
{
}

MRUData* SceneSettings::GetMRU()
{
    return m_MRU;
}

f32 SceneSettings::ScaleManipulatorSize() const
{
    return m_ScaleManipulatorSize;
}

bool SceneSettings::ScaleManipulatorGridSnap() const
{
    return m_ScaleManipulatorGridSnap;
}

f32 SceneSettings::ScaleManipulatorDistance() const
{
    return m_ScaleManipulatorDistance;
}

f32 SceneSettings::RotateManipulatorSize() const
{
    return m_RotateManipulatorSize;
}

bool SceneSettings::RotateManipulatorAxisSnap() const
{
    return m_RotateManipulatorAxisSnap;
}

f32 SceneSettings::RotateManipulatorSnapDegrees() const
{
    return m_RotateManipulatorSnapDegrees;
}

ManipulatorSpace SceneSettings::RotateManipulatorSpace() const
{
    return m_RotateManipulatorSpace;
}

f32 SceneSettings::TranslateManipulatorSize() const
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

f32 SceneSettings::TranslateManipulatorDistance() const
{
    return m_TranslateManipulatorDistance;
}

bool SceneSettings::TranslateManipulatorLiveObjectsOnly() const
{
    return m_TranslateManipulatorLiveObjectsOnly;
}
