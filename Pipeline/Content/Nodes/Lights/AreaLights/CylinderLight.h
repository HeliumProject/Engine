#pragma once

#include "Pipeline/API.h"
#include "RadiusLight.h"
namespace Content
{
  class PIPELINE_API CylinderLight : public RadiusLight
  {
  public:
    virtual void Host(ContentVisitor* visitor); 

    f32 m_Length;

    CylinderLight ()
      : RadiusLight()
      , m_Length( 1.0f )
    {
      m_Radius= 0.25f;
      
    }

    REFLECT_DECLARE_CLASS(CylinderLight, RadiusLight);

    static void EnumerateClass( Reflect::Compositor<CylinderLight>& comp );

    virtual void CalculateSampleDensity();
  };

  typedef Nocturnal::SmartPtr<CylinderLight> CylinderLightPtr;
}