#include "SceneGraphPch.h"
#include "CameraSettings.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ENUMERATION( CameraMode );
REFLECT_DEFINE_ENUMERATION( ShadingMode );
REFLECT_DEFINE_OBJECT( CameraSettings );

CameraSettings::CameraSettings()
: m_CameraMode( CameraMode::Orbit ) 
, m_WireframeOnMesh( true )
, m_WireframeOnShaded( false )
, m_ShadingMode( ShadingMode::Texture )
, m_ViewFrustumCulling( true )
, m_BackFaceCulling( true )
{

}
