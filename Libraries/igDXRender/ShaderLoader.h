#pragma once

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"
 
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
