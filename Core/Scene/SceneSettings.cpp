/*#include "Precompile.h"*/
#include "SceneSettings.h"

#include "Foundation/Preferences.h"

using namespace Helium;
using namespace Helium::Core;

SceneSettings::SceneSettings()
: m_MRU( new MRUData() )
, m_DefaultNodeVisibility( new Content::NodeVisibility() )
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

Content::NodeVisibility* SceneSettings::GetDefaultNodeVisibility()
{
    return m_DefaultNodeVisibility; 
}

const Reflect::Field* SceneSettings::ScaleManipulatorSize() const
{
    return GetClass()->FindField( &SceneSettings::m_ScaleManipulatorSize );
}

const Reflect::Field* SceneSettings::RotateManipulatorSize() const
{
    return GetClass()->FindField( &SceneSettings::m_RotateManipulatorSize );
}

const Reflect::Field* SceneSettings::RotateManipulatorAxisSnap() const
{
    return GetClass()->FindField( &SceneSettings::m_RotateManipulatorAxisSnap );
}

const Reflect::Field* SceneSettings::RotateManipulatorSnapDegrees() const
{
    return GetClass()->FindField( &SceneSettings::m_RotateManipulatorSnapDegrees );
}

const Reflect::Field* SceneSettings::RotateManipulatorSpace() const
{
    return GetClass()->FindField( &SceneSettings::m_RotateManipulatorSpace );
}

const Reflect::Field* SceneSettings::TranslateManipulatorSize() const
{
    return GetClass()->FindField( &SceneSettings::m_TranslateManipulatorSize );
}

const Reflect::Field* SceneSettings::TranslateManipulatorSpace() const
{
    return GetClass()->FindField( &SceneSettings::m_TranslateManipulatorSpace );
}

const Reflect::Field* SceneSettings::TranslateManipulatorSnappingMode() const
{
    return GetClass()->FindField( &SceneSettings::m_TranslateManipulatorSnappingMode );
}

const Reflect::Field* SceneSettings::TranslateManipulatorDistance() const
{
    return GetClass()->FindField( &SceneSettings::m_TranslateManipulatorDistance );
}

const Reflect::Field* SceneSettings::TranslateManipulatorLiveObjectsOnly() const
{
    return GetClass()->FindField( &SceneSettings::m_TranslateManipulatorLiveObjectsOnly );
}

const Reflect::Field* SceneSettings::ScaleManipulatorGridSnap() const
{
    return GetClass()->FindField( &SceneSettings::m_ScaleManipulatorGridSnap );
}

const Reflect::Field* SceneSettings::ScaleManipulatorDistance() const
{
    return GetClass()->FindField( &SceneSettings::m_ScaleManipulatorDistance );
}
