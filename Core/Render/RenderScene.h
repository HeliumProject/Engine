#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "Core/API.h"
#include "Core/Render/ObjectLoader.h"
#include "Core/Render/RenderMesh.h"
#include "Core/Render/RenderEnvironment.h"

namespace Helium
{
    namespace Render
    {
        class Renderer;

        class CORE_API RenderScene
        {
        public:
            RenderScene(Renderer* render);
            ~RenderScene();

            void SetMeshHandle(uint32_t handle);
            void SetEnvironmentHandle(uint32_t handle);

            void AddShader( const uint32_t shaderHandle )
            {
                m_shader_table.push_back( shaderHandle );
            }

        private:
            uint32_t LoadNewMesh(const tchar* name,ObjectLoaderPtr loader = NULL, int bangleIndex = -1);
        public:
            uint32_t LoadMesh(const tchar* name,ObjectLoaderPtr loader = NULL, int bangleIndex = -1);
            uint32_t ExtractMesh(const tchar* name, ObjectLoaderPtr loader, int bangleIndex);

        private:
            uint32_t LoadNewEnvironment(const tchar* fname,uint32_t clear_color);
        public:
            uint32_t LoadEnvironment(const tchar* name, uint32_t clear_color);

            static RenderMesh*        ResolveMeshHandle( uint32_t handle );
            static RenderEnvironment* ResolveEnvironmentHandle( uint32_t handle );

            static void RemoveAllMeshes();
            static void RemoveAllEnvironments();

            // render class
            Renderer*                   m_renderer;

            // handle to an environment
            uint32_t                         m_environment;

            // the viewport area, where to render on the back buffer 
            uint32_t                         m_xpos;
            uint32_t                         m_ypos;
            uint32_t                         m_width;
            uint32_t                         m_height; 

            float                       m_scene_scale;                // set before loading a mesh!
            float                       m_normalscale;                // set before loading a mesh

            float                       m_expsoure;                   // global exposure
            float                       m_diffuse_light_scale;        // SH scale
            bool                        m_render_reference_grid;      // render the reference axis
            bool                        m_render_viewport_outline;    // render the 2D outline of the viewport(good for multiple viewports)
            bool                        m_render_wireframe;           // overlay wireframe  
            bool                        m_render_normals;
            bool                        m_render_tangents;
            bool                        m_render_env_cube;
            uint32_t                         m_draw_mode;                  // the main draw mode

            // the index in the mesh array
            uint32_t                         m_mesh_handle;
            std::vector< uint32_t >          m_shader_table;
            std::vector< RenderLight* > m_lights;                     // these lights are accumualted into the env sh
            D3DXVECTOR4                 m_ambient;                    // this ambient is accumulated into the env sh

            // bounding info computed when the mesh is loaded
            D3DXVECTOR3                 m_origin;
            D3DXVECTOR3                 m_min;
            D3DXVECTOR3                 m_max;

            // change these matrices as you wish, use the D3DX functions for ease
            D3DXMATRIX                  m_projmat;
            D3DXMATRIX                  m_viewmat;
            D3DXMATRIX                  m_worldmat;
        };
    }
}