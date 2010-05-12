#pragma once

#include "API.h"
#include "AreaLight.h"
namespace Content
{
  class CONTENT_API RadiusLight : public AreaLight
  {
  public:
    

    f32 m_Radius;

    RadiusLight ()
      : AreaLight()
      , m_Radius( 1.0f )
    {

    }

    REFLECT_DECLARE_ABSTRACT(RadiusLight, AreaLight);

    static void EnumerateClass( Reflect::Compositor<RadiusLight>& comp );  

    virtual void CalculateSampleDensity();
  };

  typedef Nocturnal::SmartPtr<RadiusLight> RadiusLightPtr;
}