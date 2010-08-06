#include "Precompile.h"
#include "Editor/Scene/CameraPreferences.h"

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

void CameraPreferences::ApplyToCamera(Editor::Camera* camera)
{
  camera->SetShadingMode( (Editor::ShadingMode)m_ShadingMode ); 

  camera->SetWireframeOnMesh( m_WireframeOnMesh ); 
  camera->SetWireframeOnShaded( m_WireframeOnShaded ); 

  camera->SetViewFrustumCulling( m_ViewFrustumCulling ); 
  camera->SetBackFaceCulling( m_BackFaceCulling ); 
}

void CameraPreferences::LoadFromCamera(Editor::Camera* camera)
{
  m_ShadingMode = camera->GetShadingMode(); 

  m_WireframeOnMesh = camera->GetWireframeOnMesh(); 
  m_WireframeOnShaded = camera->GetWireframeOnShaded(); 

  m_ViewFrustumCulling = camera->IsViewFrustumCulling(); 
  m_BackFaceCulling = camera->IsBackFaceCulling(); 
}