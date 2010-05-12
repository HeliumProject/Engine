#pragma once

#include "API.h" 

namespace Content
{
  class Scene; 
  class SceneNode; 
  class Mesh; 
  class Shader; 
  class Sky; 
  class Light; 
  class SphereLight; 
  class RectangleLight;
  class PointLight; 
  class SpotLight; 
  class SunLight; 
  class DirectionalLight; 
  class ShadowDirection; 
  class PortalLight;
  class LightingEnvironment; 
  class LightingVolume; 
  class PostProcessingVolume; 
  class Camera; 
  class CubeMapProbe; 
  class MentalRayOptions; 
  class AmbientLight;
  class AmbientVolumeLight;
  class LightScattering; 
  class CylinderLight;
  class DiscLight;

  class CONTENT_API ContentVisitor
  {
  public: 
    ContentVisitor(); 
    virtual ~ContentVisitor(); 

    virtual void VisitSceneNode(SceneNode* node) {}
    virtual void VisitMesh(Mesh* mesh) {}
    virtual void VisitShader(Shader* shader) {}
    virtual void VisitSky(Sky* sky) {}

    virtual void VisitLight(Light* light) {}
    virtual void VisitSphereLight(SphereLight* light) {}
    virtual void VisitRectangleLight(RectangleLight* light) {}
    virtual void VisitCylinderLight(CylinderLight* light) {}
    virtual void VisitDiscLight(DiscLight* light) {}
    virtual void VisitPointLight(PointLight* light) {}
    virtual void VisitSpotLight(SpotLight* light) {}
    virtual void VisitSunLight(SunLight* light) {}
    virtual void VisitPortalLight(PortalLight* light) {}
    virtual void VisitDirectionalLight(DirectionalLight* light) {}
    virtual void VisitShadowDirection(ShadowDirection* light) {}
    virtual void VisitLightingEnvironment(LightingEnvironment* env) {}
    virtual void VisitLightingVolume(LightingVolume* vol) {}
    virtual void VisitAmbientLight(AmbientLight* light) {}
    virtual void VisitAmbientVolumeLight(AmbientVolumeLight* light) {}

    virtual void VisitPostProcessingVolume(PostProcessingVolume* vol) {}
    virtual void VisitLightScattering(LightScattering* light) {}

    virtual void VisitCamera(Camera* cam) {}
    virtual void VisitCubeMapProbe(CubeMapProbe* probe) {}
    virtual void VisitMentalRayOptions(MentalRayOptions* options) {}
  };
}
