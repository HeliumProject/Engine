#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>

#include "Pipeline/API.h"
#include "Pipeline/Render/ShaderLoader.h"

namespace Helium
{
    namespace Render
    {
        class PIPELINE_API ShaderFrag
        {
        public:
            ShaderFrag()
                : m_shader( TXT( "" ) )
                , m_shader_loader( NULL )
                , m_bangle_index( 0 )
            {
            }

            virtual ~ShaderFrag()
            {
            }

            tstring             m_file;
            tstring             m_shader;
            ShaderLoaderPtr     m_shader_loader;

            std::vector<uint32_t>    m_pIndex;
            std::vector<uint32_t>    m_nIndex;
            std::vector<uint32_t>    m_tIndex;
            std::vector<uint32_t>    m_tanIndex;
            std::vector<uint32_t>    m_cIndex;

            //data structures optimized for rendering, compiled model
            std::vector<uint32_t>    m_indices;

            //bangle index for this shader fragment
            uint32_t                 m_bangle_index;
        };

        struct PIPELINE_API Fragment
        {
            uint32_t     m_orig_shader;
            uint32_t     m_prim_count;
            uint32_t     m_base_index;
        };

        class PIPELINE_API RenderMesh
        {
        public:
            RenderMesh( const tchar* name );
            virtual ~RenderMesh();

            tstring                   m_filename;       
            uint32_t                       m_crc;            // crc of the filename
            uint64_t                       m_timestamp;      // -1 if the mesh is a local mesh
            uint32_t                       m_load_count;

            IDirect3DVertexBuffer9*   m_dbg_normal;
            IDirect3DVertexBuffer9*   m_dbg_tangent;
            IDirect3DVertexBuffer9*   m_verts;
            IDirect3DIndexBuffer9*    m_indices;
            uint32_t                       m_vert_count;
            uint32_t                       m_vert_size;
            uint32_t                       m_index_count;
            uint32_t                       m_fragment_count;
            Fragment*                 m_fragments;

            // bounding info
            D3DXVECTOR3       m_origin;
            D3DXVECTOR3       m_min;
            D3DXVECTOR3       m_max;

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