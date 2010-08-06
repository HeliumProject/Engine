#include "Precompile.h"
#include "CameraPreferences.h"

using namespace Helium;
using namespace Helium::Editor; 

CameraPreferences::CameraPreferences()
: m_CameraMode( CameraModes::Orbit ) 
, m_WireframeOnMesh( true )
, m_WireframeOnShaded( false )
, m_ShadingMode( ShadingModes::Texture )
, m_ViewFrustumCulling( true )
, m_BackFaceCulling( true )
{

}
