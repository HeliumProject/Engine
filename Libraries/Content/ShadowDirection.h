#pragma once

#include "API.h"
#include "Light.h"

namespace Content
{
  class CONTENT_API ShadowDirection : public Light
  {
  public:
    ShadowDirection () : Light()  {  }

    ShadowDirection (UniqueID::TUID &id) : Light (id)  {  }

    REFLECT_DECLARE_CLASS(ShadowDirection, Light);
  };

  typedef Nocturnal::SmartPtr<ShadowDirection> ShadowDirectionPtr;
  typedef std::vector<ShadowDirectionPtr> V_ShadowDirection;
}
