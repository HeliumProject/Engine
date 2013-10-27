#include "EditorScenePch.h"
#include "ViewportSettings.h"

HELIUM_DEFINE_ENUM( Helium::Editor::GeometryMode );
HELIUM_DEFINE_ENUM( Helium::Editor::ViewColorMode );
HELIUM_DEFINE_CLASS( Helium::Editor::ViewportSettings );

using namespace Helium;
using namespace Helium::Editor;

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
	return GetMetaClass()->FindField( &ViewportSettings::m_ColorMode );
}
