#pragma once

#include "igDXRender/ShaderLoader.h"

namespace Asset
{
  class StandardColorMapAttribute;
  class StandardNormalMapAttribute;
  class StandardExpensiveMapAttribute;
}

namespace igDXRender
{
  class TextureSettings;
}

namespace igDXContent
{
  class IRBShaderLoader : public igDXRender::ShaderLoader
  {
  public:
    IRBShaderLoader();
    virtual ~IRBShaderLoader();

    virtual igDXRender::Shader* ParseFile( const char* filename, igDXRender::ShaderDatabase* db );

    static void SetWrapUV( igDXRender::TextureSettings* settings, u32 wrapU, u32 wrapV );
    static void SetFilter( igDXRender::TextureSettings* settings, u32 filter );
    static void SetColorFormat( igDXRender::TextureSettings* settings, u32 colorFormat, u32 mode );

    static void UpdateShaderClass(igDXRender::ShaderDatabase* db, const char* shaderFilename, u32 alphaMode);
    static void UpdateShaderColorMap(igDXRender::Shader* sh, const Asset::StandardColorMapAttribute* colorMap);
    static void UpdateShaderNormalMap(igDXRender::Shader* sh, const Asset::StandardNormalMapAttribute* colorMap);
    static void UpdateShaderExpensiveMap(igDXRender::Shader* sh, const Asset::StandardExpensiveMapAttribute* colorMap);
  };
}
