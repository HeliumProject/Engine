#include "SceneGraphPch.h"
#include "ViewportSettings.h"

REFLECT_DEFINE_ENUMERATION( Helium::SceneGraph::GeometryMode );
REFLECT_DEFINE_ENUMERATION( Helium::SceneGraph::ViewColorMode );
REFLECT_DEFINE_OBJECT( Helium::SceneGraph::ViewportSettings );

using namespace Helium;
using namespace Helium::SceneGraph;

ViewportSettings::ViewportSettings()
: m_CameraMode (CameraMode::Orbit)
, m_GeometryMode (GeometryMode::Render)
, m_ColorMode (ViewColorMode::Type)
, m_Highlighting (true)
, m_AxesVisible (true)
, m_GridVisible (true)
, m_BoundsVisible (false)
, m_StatisticsVisible (false)
{
    for(int i = 0; i < CameraMode::Count; ++i)
    {
        m_CameraPrefs.push_back( new CameraSettings() ); 
        m_CameraPrefs.back()->m_CameraMode = static_cast< CameraMode::Enum >( i );
    }
}

ViewColorMode ViewportSettings::GetColorMode() const
{
    return m_ColorMode;
}

void ViewportSettings::SetColorMode( ViewColorMode mode )
{
    if ( m_ColorMode != mode )
    {
        m_ColorMode = mode;
        RaiseChanged( ColorModeField() );
    }
}

const Reflect::Field* ViewportSettings::ColorModeField() const
{
    return GetClass()->FindField( &ViewportSettings::m_ColorMode );
}
