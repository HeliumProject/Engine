#pragma once

#include <d3d9.h>
#include <vector>

#include "Core/API.h"
#include "Core/Render/ShaderLoader.h"

namespace Helium
{
    namespace Render
    {
        class CORE_API Texture
        {
        public:
            Texture(const tchar* fname);
            ~Texture();

            u32 IncrementUsage()
            {
                return ++m_load_count;
            }

            u32 DecrementUsage()
            {
                if (m_load_count>0)
                {
                    m_load_count--;
                }

                return m_load_count;
            }

            tstring         m_filename;
            u64                 m_timestamp;
            u32                 m_crc;

            // a load count of zero means the shader is valid and can be used, its simply not currently used by any loaded mesh (the UI should indicate this)
            u32                 m_load_count;         // we don't use D3D ref-counts because they will go to zero and delete themselves

            IDirect3DTexture9*  m_d3d_texture;
            u32                 m_wrap_u;
            u32                 m_wrap_v;
            u32                 m_filter;
            u32                 m_aniso;
            D3DFORMAT           m_format;
            float               m_mip_bias;

            enum
            {
                SAMPLER_BASE_MAP,
                SAMPLER_NORMAL_MAP,
                SAMPLER_GPI_MAP,
                SAMPLER_GLOSS_MAP = SAMPLER_GPI_MAP,      // we either have the GPI map or we have them seperate (see the usage flag SHDR_FLAG_GPI_MAP)
                SAMPLER_INCAN_MAP,
                SAMPLER_PARALLAX_MAP,

                __SAMPLER_LAST__
            };

            enum
            {
                FILTER_POINT,
                FILTER_LINEAR,
                FILTER_ANISOTROPIC
            };
        };

        class CORE_API TextureSettings
        {
        public:
            tstring m_Path;
            D3DFORMAT   m_Format;
            u32         m_Levels;
            u32         m_WrapU;
            u32         m_WrapV;
            u32         m_Filter;
            u32         m_Anisotropy;
            float       m_MipBias;

            TextureSettings()
            {
                Clear();
            }

            void Clear()
            {
                m_Path = TXT( "" );
                m_Format = D3DFMT_UNKNOWN;
                m_Levels = 0;
                m_WrapU = m_WrapV = D3DTADDRESS_CLAMP;
                m_Filter = Texture::FILTER_LINEAR;
                m_Anisotropy = 0;
                m_MipBias = 0.0f;
            }
        };

        //#define SHDR_FLAG_MODIFIED    0x00000001      // shader has been modified since it was last saved (The UI should display these shaders in a different color)
#define SHDR_FLAG_GPI_MAP    0x00000001      // shader has a GPI map instead of having all three maps seperate
#define SHDR_FLAG_TWO_SIDED  0x00000002      // two sided

        class CORE_API RenderShader
        {
        public:
            RenderShader(class ShaderManager* sd, const tchar* fname);
            ~RenderShader();

            u32 IncrementUsage()
            {
                return ++m_load_count;
            }

            u32 DecrementUsage()
            {
                if (m_load_count>0)
                {
                    m_load_count--;
                }

                return m_load_count;
            }

            // replace a texture in the shader
            void ReplaceTexture(u32 texture_handle, u32 texture_slot);

            tstring   m_filename;
            u64           m_timestamp;
            u32           m_crc;
            u32           m_load_count;
            u32           m_flags;

            // Use ReplaceTexture() to modify these so the ref counting works
            class ShaderManager*  m_sd;
            u32                   m_textures[Texture::__SAMPLER_LAST__];

            // These values can be modified directly by the UI
            u32           m_alpha_type;
            float         m_basetint[4];
            float         m_glosstint[4];
            float         m_gloss_scale;
            float         m_incan_scale;        // only ever set to 0.0 or 1.0 to enable/disable incan
            float         m_specular_power;
            float         m_normal_scale;
            float         m_parallax_scale;
            float         m_parallax_bias;
            float         m_env_lod;

            enum
            {
                ALPHA_OPAQUE,
                ALPHA_ADDITIVE,
                ALPHA_BLENDED,
                ALPHA_CUTOUT
            };
        };

        // there is only a single shader database regardless of how many meshes are loaded.
        class CORE_API ShaderManager
        {
        public:
            ShaderManager();
            ~ShaderManager();

            // don't call directly
            void Init( IDirect3DDevice9* device );

        private:
            u32 AddShader(RenderShader* sh); // assign a new shader to a handle
            u32 LoadNewShader(const tchar* fname, ShaderLoaderPtr loader = NULL);

        public:
            // return the handle to the specified shader, the handle returned will either be new or it will
            // be the handle of an existing copy of that shader.
            u32 LoadShader(const tchar* fname, bool inc=false, ShaderLoaderPtr loader = NULL);

            // return the handle of the shader with the given name (0xffffffff if it cannot be found)
            u32 FindShader(const tchar* fname);

            // Duplicate an existing shader (to create an empty shader duplicate the default)
            u32 DuplicateShader(u32 handle, const tchar* new_shader);

            // load a single texture, get the handle
            u32 LoadTexture(const tchar* fname,D3DFORMAT fmt,u32 levels=0,bool inc=false);
            bool LoadTextureWithSettings(const TextureSettings& textureSettings, RenderShader* shader, u32 sampler);
            void UpdateTextureSettings(u32 handle, const TextureSettings& textureSettings);
            bool ReloadTexture( const tchar* fname );

            // return the handle of the texture with the given name (0xffffffff if it cannot be found)
            u32 FindTexture(const tchar* fname);
            Texture* ResolveTexture(u32 handle);
            RenderShader* ResolveShader(u32 handle);

            void SetShaderDefaultTexture( const tchar* shaderFilename, u32 textureIndex );
            void UpdateShaderTexture( const tchar* shaderFilename, u32 textureIndex, const TextureSettings& settings );
            void GetShaderFilenames( std::vector< tstring >& filenames );
            void GetTextureFilenames( std::vector< tstring >& filenames );

            std::vector<RenderShader*>   m_loaded_shaders;
            std::vector<Texture*>  m_loaded_textures;
            IDirect3DDevice9*      m_device;
        };
    }
}