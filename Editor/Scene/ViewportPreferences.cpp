#include "Precompile.h"
#include "ViewportPreferences.h"

using namespace Helium;
using namespace Helium::Editor; 
   
ViewportPreferences::ViewportPreferences()
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
    m_CameraPrefs.push_back( new CameraPreferences() ); 
    m_CameraPrefs.back()->m_CameraMode = CameraMode(i);
  }
}

ViewColorMode ViewportPreferences::GetColorMode() const
{
  return m_ColorMode;
}

void ViewportPreferences::SetColorMode( ViewColorMode mode )
{
  if ( m_ColorMode != mode )
  {
    m_ColorMode = mode;
    RaiseChanged( ColorModeField() );
  }
}

const Reflect::Field* ViewportPreferences::ColorModeField() const
{
  return GetClass()->FindField( &ViewportPreferences::m_ColorMode );
}
