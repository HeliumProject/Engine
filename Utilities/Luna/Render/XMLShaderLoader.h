#pragma once

#include "ShaderLoader.h"

namespace igDXRender
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

    virtual class Shader* ParseFile( const char* filename, class ShaderDatabase* db );
  };
}
