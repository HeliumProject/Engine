/*#include "Precompile.h"*/
#include "ViewportSettings.h"

using namespace Helium;
using namespace Helium::SceneGraph; 
   
ViewportSettings::ViewportSettings()
: m_CameraMode (CameraModes::Orbit)
, m_GeometryMode (GeometryModes::Render)
, m_ColorMode (ViewColorModes::Type)
, m_Highlighting (true)
, m_AxesVisible (true)
, m_GridVisible (true)
, m_BoundsVisible (false)
, m_StatisticsVisible (false)
{
  for(int i = 0; i < CameraModes::Count; ++i)
  {
    m_CameraPrefs.push_back( new CameraSettings() ); 
    m_CameraPrefs.back()->m_CameraMode = CameraMode(i);
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
