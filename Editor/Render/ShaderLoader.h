#pragma once

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"
 
namespace Render
{
  class Shader;
  class ShaderManager;

  class ShaderLoader: public Helium::RefCountBase<ShaderLoader>
  {
  public:
    ShaderLoader();
    virtual ~ShaderLoader();

    virtual Shader* ParseFile( const tchar* filename, ShaderManager* db ) = 0;
  };

  typedef Helium::SmartPtr<ShaderLoader> ShaderLoaderPtr;
}
