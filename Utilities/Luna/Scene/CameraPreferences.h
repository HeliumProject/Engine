#pragma once

#include "Reflect/Element.h"
#include "Content/Camera.h"
#include "Camera.h" 
#include "View.h"

namespace Luna
{
  static void ShadingModeEnumerateEnumeration( Reflect::Enumeration* info )
  {
    info->AddElement(ShadingModes::Wireframe, "Wireframe");
    info->AddElement(ShadingModes::Material, "Material");
    info->AddElement(ShadingModes::Texture, "Texture");
  }

  class CameraPreferences : public Reflect::Element
  {
  public: 
    CameraPreferences(); 
    CameraPreferences(CameraMode mode); 

    void ApplyToCamera(Luna::Camera* camera); 
    void LoadFromCamera(Luna::Camera* camera); 

  private: 
    void ConstructorInit(CameraMode mode); 


  public: 
    CameraMode  m_CameraMode; // we save what mode we correspond to
    ShadingMode m_ShadingMode; 
    
    bool m_WireframeOnMesh; 
    bool m_WireframeOnShaded;

    bool m_ViewFrustumCulling; 
    bool m_BackFaceCulling; 

  public:
    REFLECT_DECLARE_CLASS(CameraPreferences, Reflect::Element); 
    static void EnumerateClass( Reflect::Compositor<CameraPreferences>& comp );

  }; 

  typedef Nocturnal::SmartPtr<CameraPreferences> LCameraPreferencesPtr; 
  typedef std::vector< LCameraPreferencesPtr> V_CameraPreferences; 

}
