#pragma once

#include "ObjectLoader.h"

namespace Render
{
  class OBJObjectLoader : public ObjectLoader
  {
  public:
    u32 ParseFile(const tchar* fname, bool winding = false);
  };
}
