#pragma once

#include "Render/ShaderLoader.h"

namespace Asset
{
  class StandardColorMapComponent;
  class StandardNormalMapComponent;
  class StandardExpensiveMapComponent;
}

namespace Render
{
  class TextureSettings;
}

namespace Content
{
  class RBShaderLoader : public Render::ShaderLoader
  {
  public:
    RBShaderLoader();
    virtual ~RBShaderLoader();

    virtual Render::Shader* ParseFile( const char* filename, Render::ShaderManager* db );

    static void SetWrapUV( Render::TextureSettings* settings, u32 wrapU, u32 wrapV );
    static void SetFilter( Render::TextureSettings* settings, u32 filter );
    static void SetColorFormat( Render::TextureSettings* settings, u32 colorFormat, u32 mode );

    static void UpdateShaderClass(Render::ShaderManager* db, const char* shaderFilename, u32 alphaMode);
    static void UpdateShaderColorMap(Render::Shader* sh, const Asset::StandardColorMapComponent* colorMap);
    static void UpdateShaderNormalMap(Render::Shader* sh, const Asset::StandardNormalMapComponent* colorMap);
    static void UpdateShaderExpensiveMap(Render::Shader* sh, const Asset::StandardExpensiveMapComponent* colorMap);
  };
}
