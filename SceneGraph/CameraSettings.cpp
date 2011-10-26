#include "SceneGraphPch.h"
#include "CameraSettings.h"

REFLECT_DEFINE_ENUMERATION( Helium::SceneGraph::CameraMode );
REFLECT_DEFINE_ENUMERATION( Helium::SceneGraph::ShadingMode );
REFLECT_DEFINE_OBJECT( Helium::SceneGraph::CameraSettings );

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
