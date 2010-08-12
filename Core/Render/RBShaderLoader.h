#pragma once

#include "Core/API.h"
#include "Core/Render/ShaderLoader.h"
#include "Core/Asset/Classes/ShaderAsset.h"

namespace Helium
{
    namespace Core
    {
        namespace Render
        {
            class TextureSettings;

            class CORE_API RBShaderLoader : public ShaderLoader
            {
            public:
                RBShaderLoader();
                virtual ~RBShaderLoader();

                virtual Render::Shader* ParseFile( const tchar* filename, Render::ShaderManager* db );

                static void SetWrapUV( Render::TextureSettings* settings, u32 wrapU, u32 wrapV );
                static void SetFilter( Render::TextureSettings* settings, u32 filter );
                static void SetColorFormat( Render::TextureSettings* settings, u32 colorFormat, u32 mode );

                static void UpdateShaderClass(Render::ShaderManager* db, const tchar* shaderFilename, u32 alphaMode);
                static void UpdateShader(Render::Shader* sh, const Asset::ShaderAsset* shader );
            };
        }
    }
}