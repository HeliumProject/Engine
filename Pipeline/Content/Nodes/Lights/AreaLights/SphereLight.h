#pragma once

#include "Pipeline/API.h"
#include "RadiusLight.h"
namespace Content
{
  class PIPELINE_API SphereLight : public RadiusLight
  {
  public:
    virtual void Host(ContentVisitor* visitor); 

    SphereLight ()
      : RadiusLight()
    {
    }

    REFLECT_DECLARE_CLASS(SphereLight, RadiusLight);

  };

  typedef Nocturnal::SmartPtr<SphereLight> SphereLightPtr;
}