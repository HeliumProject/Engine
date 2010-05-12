#pragma once

#include "API.h"
#include "AreaLight.h"
namespace Content
{
  class CONTENT_API RectangleLight : public AreaLight
  {
  public:
    virtual void Host(ContentVisitor* visitor); 

    Math::Vector2 m_Dimensions;

    RectangleLight ()
      : AreaLight()
      , m_Dimensions( 1.0, 1.0 )
    {

    }

    REFLECT_DECLARE_CLASS(RectangleLight, AreaLight);

    static void EnumerateClass( Reflect::Compositor<RectangleLight>& comp );

    virtual void CalculateSampleDensity();
  };

  typedef Nocturnal::SmartPtr<RectangleLight> RectangleLightPtr;
}