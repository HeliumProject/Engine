#pragma once

#include "API.h"
#include "Light.h"

namespace Content
{
  class CONTENT_API PortalLight : public Light
  {
  public:
    virtual void Host(ContentVisitor* visitor); 

    float m_InnerRadius;
    float m_OuterRadius;

    PortalLight ()
    {

    }

    PortalLight (Nocturnal::UID::TUID &id)
    {

    }

    REFLECT_DECLARE_CLASS(PortalLight, Light);

   

  };

  typedef Nocturnal::SmartPtr<PortalLight> PortalLightPtr;
  typedef std::vector<PortalLightPtr> V_PortalLight;
}
