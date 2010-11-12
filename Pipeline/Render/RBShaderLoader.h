#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Render/ShaderLoader.h"
#include "Pipeline/Asset/Classes/ShaderAsset.h"

namespace Helium
{
    namespace Render
    {
        class TextureSettings;

        class PIPELINE_API RBShaderLoader : public ShaderLoader
        {
        public:
            RBShaderLoader();
            virtual ~RBShaderLoader();

            virtual RenderShader* ParseFile( const tchar_t* filename, ShaderManager* db );

            static void SetWrapUV( TextureSettings* settings, uint32_t wrapU, uint32_t wrapV );
            static void SetFilter( TextureSettings* settings, uint32_t filter );
            static void SetColorFormat( TextureSettings* settings, uint32_t colorFormat, uint32_t mode );

            static void UpdateShaderClass(ShaderManager* db, const tchar_t* shaderFilename, uint32_t alphaMode);
            static void UpdateShader(RenderShader* sh, const Asset::ShaderAsset* shader );
        };
    }
}