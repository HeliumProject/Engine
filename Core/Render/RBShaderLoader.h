#pragma once

#include "Core/API.h"
#include "Core/Render/ShaderLoader.h"
#include "Core/Asset/Classes/ShaderAsset.h"

namespace Helium
{
    namespace Render
    {
        class TextureSettings;

        class CORE_API RBShaderLoader : public ShaderLoader
        {
        public:
            RBShaderLoader();
            virtual ~RBShaderLoader();

            virtual RenderShader* ParseFile( const tchar* filename, ShaderManager* db );

            static void SetWrapUV( TextureSettings* settings, uint32_t wrapU, uint32_t wrapV );
            static void SetFilter( TextureSettings* settings, uint32_t filter );
            static void SetColorFormat( TextureSettings* settings, uint32_t colorFormat, uint32_t mode );

            static void UpdateShaderClass(ShaderManager* db, const tchar* shaderFilename, uint32_t alphaMode);
            static void UpdateShader(RenderShader* sh, const Asset::ShaderAsset* shader );
        };
    }
}