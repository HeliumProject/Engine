#include "Precompile.h"
#include "CameraPreferences.h"

using namespace Luna; 

REFLECT_DEFINE_CLASS( CameraPreferences );

void CameraPreferences::EnumerateClass( Reflect::Compositor<CameraPreferences>& comp )
{
  Reflect::EnumerationField* enumCameraMode = comp.AddEnumerationField( &CameraPreferences::m_CameraMode, "m_CameraMode" );
  Reflect::EnumerationField* enumShadingMode = comp.AddEnumerationField( &CameraPreferences::m_ShadingMode, "m_ShadingMode" );
  Reflect::Field* fieldWireframeOnMesh = comp.AddField( &CameraPreferences::m_WireframeOnMesh, "m_WireframeOnMesh" );
  Reflect::Field* fieldWireframeOnShaded = comp.AddField( &CameraPreferences::m_WireframeOnShaded, "m_WireframeOnShaded" );
  Reflect::Field* fieldViewFrustumCulling = comp.AddField( &CameraPreferences::m_ViewFrustumCulling, "m_ViewFrustumCulling" );
  Reflect::Field* fieldBackFaceCulling = comp.AddField( &CameraPreferences::m_BackFaceCulling, "m_BackFaceCulling" );
}
    
CameraPreferences::CameraPreferences()
{
  ConstructorInit(CameraModes::Orbit); 
}

CameraPreferences::CameraPreferences(CameraMode mode)
{
  ConstructorInit(mode); 
}

void CameraPreferences::ConstructorInit(CameraMode mode)
{
  m_CameraMode = mode; 

  m_WireframeOnMesh = true;
  m_WireframeOnShaded = false;
  m_ShadingMode = ShadingModes::Texture;

  m_ViewFrustumCulling = true;
  m_BackFaceCulling = true;
}

void CameraPreferences::ApplyToCamera(Luna::Camera* camera)
{
  camera->SetShadingMode( (Luna::ShadingMode)m_ShadingMode ); 

  camera->SetWireframeOnMesh( m_WireframeOnMesh ); 
  camera->SetWireframeOnShaded( m_WireframeOnShaded ); 

  camera->SetViewFrustumCulling( m_ViewFrustumCulling ); 
  camera->SetBackFaceCulling( m_BackFaceCulling ); 
}

void CameraPreferences::LoadFromCamera(Luna::Camera* camera)
{
  m_ShadingMode = camera->GetShadingMode(); 

  m_WireframeOnMesh = camera->GetWireframeOnMesh(); 
  m_WireframeOnShaded = camera->GetWireframeOnShaded(); 

  m_ViewFrustumCulling = camera->IsViewFrustumCulling(); 
  m_BackFaceCulling = camera->IsBackFaceCulling(); 
}