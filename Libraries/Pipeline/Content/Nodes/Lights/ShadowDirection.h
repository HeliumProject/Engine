#pragma once

#include "Pipeline/API.h"
#include "Light.h"

namespace Content
{
  class PIPELINE_API ShadowDirection : public Light
  {
  public:
    ShadowDirection () : Light()  {  }

    ShadowDirection (Nocturnal::TUID &id) : Light (id)  {  }

    REFLECT_DECLARE_CLASS(ShadowDirection, Light);
  };

  typedef Nocturnal::SmartPtr<ShadowDirection> ShadowDirectionPtr;
  typedef std::vector<ShadowDirectionPtr> V_ShadowDirection;
}
