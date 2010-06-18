#pragma once

#include "Render/Platform.h"
#include "Render/ObjectLoader.h"

namespace Content
{
  class RBObjectLoader : public Render::ObjectLoader
  {
  public:
    RBObjectLoader();
    virtual ~RBObjectLoader();

    u32 ParseFile(const char* fname,bool winding);
  };
}
