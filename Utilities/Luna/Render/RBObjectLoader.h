#pragma once

#include "Render/Platform.h"
#include "Render/ObjectLoader.h"

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
