/*#include "Precompile.h"*/
#include "CameraSettings.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_CLASS( CameraSettings );

CameraSettings::CameraSettings()
: m_CameraMode( CameraModes::Orbit ) 
, m_WireframeOnMesh( true )
, m_WireframeOnShaded( false )
, m_ShadingMode( ShadingModes::Texture )
, m_ViewFrustumCulling( true )
, m_BackFaceCulling( true )
{

}
