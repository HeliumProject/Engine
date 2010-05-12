#pragma once

#include "igDXRender/platform.h"
#include "igDXRender/ObjectLoader.h"

namespace igDXContent
{
  class IRBObjectLoader : public igDXRender::ObjectLoader
  {
  public:
    IRBObjectLoader();
    virtual ~IRBObjectLoader();

    u32 ParseFile(const char* fname,bool winding);
  };
}
