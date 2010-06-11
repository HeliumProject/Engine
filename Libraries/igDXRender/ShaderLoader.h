#pragma once

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"
 
namespace igDXRender
{
  class Shader;
  class ShaderDatabase;

  class ShaderLoader: public Nocturnal::RefCountBase<ShaderLoader>

  {
  public:
    ShaderLoader();
    virtual ~ShaderLoader();

    virtual Shader* ParseFile( const char* filename, ShaderDatabase* db ) = 0;
  };

  typedef Nocturnal::SmartPtr<ShaderLoader> ShaderLoaderPtr;
}
