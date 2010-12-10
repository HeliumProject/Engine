#pragma once

// Include our Windows header data first to make sure everything is set up properly before the Direct3D includes pull it
// in.
#include "Platform/Windows/Windows.h"

#include <d3d9.h>
#include <d3dx9.h>

#include "Pipeline/API.h"

#include "Platform/Types.h"

namespace Helium
{
    namespace Render
    {
        struct RenderLight
        {
            D3DXVECTOR4   m_direction;
            D3DXVECTOR4   m_color;
        };

        // the environment contains the cubemap for the background and the diffuse/specular lighting info contained derived from the cubemap
        class PIPELINE_API RenderEnvironment
        {
        public:
            tstring                     m_filename;       
            uint32_t                         m_crc;            // crc of the filename
            uint64_t                         m_timestamp;      // -1 if the mesh is a local mesh
            uint32_t                         m_load_count;

            D3DXVECTOR4                 m_sh[9];            // the spherical harmonic used for diffuse lighting and derived from the environment cubemap
            uint32_t                         m_clearcolor;
            IDirect3DCubeTexture9*      m_env_texture;    // if this is null the clear color is used

            // these parameters are used to extract hightlights from the env cube. They are specific to a given env and in some ways are specific to this appliction.
            // They should not be part of the shader.
            float                       m_env_scale;
            float                       m_env_bias;

            RenderEnvironment(const tchar_t* name);
            ~RenderEnvironment();

            uint32_t DecrementUsage()
            {
                if (m_load_count>0)
                {
                    m_load_count--;
                }
                return m_load_count;
            }
        };
    }
}
