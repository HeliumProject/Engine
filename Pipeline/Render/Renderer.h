#pragma once

#include "Pipeline/API.h"

#include "Pipeline/Render/DeviceManager.h"
#include "Pipeline/Render/ShaderManager.h"
#include "Pipeline/Render/RenderScene.h"

namespace Helium
{
    namespace Render
    {
        enum
        {
            DRAW_MODE_OFF,                        // dont draw the mesh, use when you just want wireframe
            DRAW_MODE_NORMAL,                     // full shader
            DRAW_MODE_DEBUG_COLOR,                // vertex colors
            DRAW_MODE_DEBUG_VERT_NORMAL,          // vertex normals
            DRAW_MODE_DEBUG_VERT_TANGENT,         // vertex tangents
            DRAW_MODE_DEBUG_UV,                   // vertex uv as color
            DRAW_MODE_DEBUG_BASEMAP,              // shader basemap
            DRAW_MODE_DEBUG_BASEMAP_ALPHA,        // shader basemap alpha channel
            DRAW_MODE_DEBUG_NORMALMAP,            // shader normal map
            DRAW_MODE_DEBUG_GPI,                  // shader gpi map for shaders that use a combined map (same id as gloss map just here for readability)
            DRAW_MODE_DEBUG_GLOSSMAP=DRAW_MODE_DEBUG_GPI,   // shader gloss map
            DRAW_MODE_DEBUG_INCANMAP,             // shader incan map
            DRAW_MODE_DEBUG_PARALLAXMAP,          // shader parallax map
            DRAW_MODE_DEBUG_SHADER_TINT,          // shader tint color
            DRAW_MODE_DEBUG_SHADER_DIRTY,         // shader dirty color
            DRAW_MODE_DEBUG_SHADER_ALPHATYPE,     // shader alpha type (green: opaque, yellow: additive, red: blended)

            // These debug modes render various indices as colors, they can be visually rendered or they can be used with the GetIndex() function to query the rendered
            // index. If you are using these index debug modes in query mode for things like drag and drop you should switch off the wireframe rendering and the reference grid
            DRAW_MODE_DEBUG_FRAGMENT_INDEX,             
        };

        // Only create a single renderer
        class PIPELINE_API Renderer : public DeviceManager
        {
        public:
            Renderer();
            ~Renderer();

            bool Init(HWND hwnd, uint32_t width, uint32_t height, uint32_t flags);
            uint32_t GetPixel(uint32_t x, uint32_t y);
            uint32_t GetIndex(uint32_t x, uint32_t y); // used with the indexed debug modes to return the index at the given pixel

        private:
            void DrawEnvironment(RenderScene* scene,RenderEnvironment* env,D3DXMATRIX& viewproj);
            void DrawMesh(RenderScene* scene,RenderMesh* mesh);
            void DrawWireframe(RenderScene* scene,RenderMesh* mesh);
            void DrawNormals(RenderScene* scene,RenderMesh* mesh);
            void DrawTangents(RenderScene* scene,RenderMesh* mesh);
            void DrawDebug(RenderScene* scene,RenderMesh* mesh);
            void DrawReferenceGrid(RenderScene* scene);

        public:
            void DrawScene(RenderScene* scene);
            void DrawScenes(int num, RenderScene** scenes);

            // the master shader database
            ShaderManager m_shader_manager;
        };
    }
}