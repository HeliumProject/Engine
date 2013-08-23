#include "SceneGraphPch.h"
#include "CameraSettings.h"

HELIUM_DEFINE_ENUM( Helium::SceneGraph::CameraMode );
HELIUM_DEFINE_ENUM( Helium::SceneGraph::ShadingMode );
HELIUM_DEFINE_CLASS( Helium::SceneGraph::CameraSettings );

using namespace Helium;
using namespace Helium::SceneGraph;

CameraSettings::CameraSettings()
: m_CameraMode( CameraMode::Orbit ) 
, m_WireframeOnMesh( true )
, m_WireframeOnShaded( false )
, m_ShadingMode( ShadingMode::Texture )
, m_ViewFrustumCulling( true )
, m_BackFaceCulling( true )
{

}
