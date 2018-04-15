#include "Precompile.h"
#include "CameraSettings.h"

HELIUM_DEFINE_ENUM( Helium::Editor::CameraMode );
HELIUM_DEFINE_ENUM( Helium::Editor::ShadingMode );
HELIUM_DEFINE_CLASS( Helium::Editor::CameraSettings );

using namespace Helium;
using namespace Helium::Editor;

CameraSettings::CameraSettings()
	: m_CameraMode( CameraMode::Orbit ) 
	, m_WireframeOnMesh( true )
	, m_WireframeOnShaded( false )
	, m_ShadingMode( ShadingMode::Texture )
	, m_ViewFrustumCulling( true )
	, m_BackFaceCulling( true )
{

}
