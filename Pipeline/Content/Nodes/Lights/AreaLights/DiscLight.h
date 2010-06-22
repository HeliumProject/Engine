#pragma once

#include "Pipeline/API.h"
#include "RadiusLight.h"
namespace Content
{
  class PIPELINE_API DiscLight : public RadiusLight
  {
  public:
    virtual void Host(ContentVisitor* visitor); 

    DiscLight ()
      : RadiusLight()
    {

    }

    REFLECT_DECLARE_CLASS(DiscLight, RadiusLight);

  };

  typedef Nocturnal::SmartPtr<DiscLight> DiscLightPtr;
}