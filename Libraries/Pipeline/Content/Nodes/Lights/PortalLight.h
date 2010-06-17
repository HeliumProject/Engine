#pragma once

#include "Pipeline/API.h"
#include "Light.h"

namespace Content
{
  class PIPELINE_API PortalLight : public Light
  {
  public:
    virtual void Host(ContentVisitor* visitor); 

    float m_InnerRadius;
    float m_OuterRadius;

    PortalLight ()
    {

    }

    PortalLight (Nocturnal::TUID &id)
    {

    }

    REFLECT_DECLARE_CLASS(PortalLight, Light);

   

  };

  typedef Nocturnal::SmartPtr<PortalLight> PortalLightPtr;
  typedef std::vector<PortalLightPtr> V_PortalLight;
}
