#pragma once

#include "ShaderLoader.h"

namespace Render
{
  class XMLShaderLoader : public ShaderLoader
  {
  public:
    XMLShaderLoader()
    {
    }

    virtual ~XMLShaderLoader()
    {
    }

    virtual class Shader* ParseFile( const char* filename, class ShaderManager* db );
  };
}
