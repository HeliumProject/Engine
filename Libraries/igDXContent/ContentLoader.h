#pragma once

#include "igDXRender/platform.h"
#include "igDXRender/ObjectLoader.h"

namespace igDXContent
{
  class RBObjectLoader : public igDXRender::ObjectLoader
  {
  public:
    RBObjectLoader();
    virtual ~RBObjectLoader();

    u32 ParseFile(const char* fname,bool winding);
  };
}
