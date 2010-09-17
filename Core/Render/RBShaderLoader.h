#pragma once

#include "Core/API.h"
#include "Core/Render/ShaderLoader.h"
#include "Core/Asset/Classes/ShaderAsset.h"

namespace Helium
{
    namespace Core
    {
        class TextureSettings;

        class CORE_API RBShaderLoader : public ShaderLoader
        {
        public:
            RBShaderLoader();
            virtual ~RBShaderLoader();

            virtual RenderShader* ParseFile( const tchar* filename, ShaderManager* db );

            static void SetWrapUV( TextureSettings* settings, u32 wrapU, u32 wrapV );
            static void SetFilter( TextureSettings* settings, u32 filter );
            static void SetColorFormat( TextureSettings* settings, u32 colorFormat, u32 mode );

            static void UpdateShaderClass(ShaderManager* db, const tchar* shaderFilename, u32 alphaMode);
            static void UpdateShader(RenderShader* sh, const Asset::ShaderAsset* shader );
        };
    }
}