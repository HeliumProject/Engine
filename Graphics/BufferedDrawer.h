#pragma once

#include "Graphics/Graphics.h"

#include "Rendering/RRenderResource.h"
#include "GraphicsTypes/VertexTypes.h"
#include "Graphics/RenderResourceManager.h"

namespace Lunar
{
    L_DECLARE_RPTR( RIndexBuffer );
    L_DECLARE_RPTR( RPixelShader );
    L_DECLARE_RPTR( RTexture2d );
    L_DECLARE_RPTR( RVertexBuffer );
    L_DECLARE_RPTR( RVertexDescription );
    L_DECLARE_RPTR( RVertexShader );

    /// Buffered drawing interface.
    class LUNAR_GRAPHICS_API BufferedDrawer : NonCopyable
    {
    public:
        /// @name Construction/Destruction
        //@{
        BufferedDrawer();
        ~BufferedDrawer();
        //@}

        /// @name Initialization
        //@{
        bool Initialize();
        void Shutdown();
        //@}

        /// @name Draw Call Generation
        //@{
        void DrawLines(
            const SimpleVertex* pVertices, uint32_t vertexCount, const uint16_t* pIndices, uint32_t lineCount );
        void DrawWireMesh(
            const SimpleVertex* pVertices, uint32_t vertexCount, const uint16_t* pIndices, uint32_t triangleCount );
        void DrawSolidMesh(
            const SimpleVertex* pVertices, uint32_t vertexCount, const uint16_t* pIndices, uint32_t triangleCount );
        void DrawTexturedMesh(
            const SimpleTexturedVertex* pVertices, uint32_t vertexCount, const uint16_t* pIndices,
            uint32_t triangleCount, RTexture2d* pTexture );

        void DrawText(
            const Simd::Matrix44& rTransform, const String& rText, const Color& rColor = Color( 0xffffffff ),
            RenderResourceManager::EDebugFontSize size = RenderResourceManager::DEBUG_FONT_SIZE_MEDIUM );
        //@}

        /// @name Rendering
        //@{
        void BeginDrawing();
        void EndDrawing();

        void Draw();
        //@}

    private:
        /// Untextured primitive draw call information.
        struct UntexturedDrawCall
        {
            /// Starting vertex index.
            uint32_t baseVertexIndex;
            /// Vertex count.
            uint32_t vertexCount;
            /// Starting index offset.
            uint32_t startIndex;
            /// Number of primitives to draw.
            uint32_t primitiveCount;
        };

        /// Textured primitive draw call information.
        struct TexturedDrawCall : UntexturedDrawCall
        {
            /// Texture with which to draw.
            RTexture2dPtr spTexture;
        };

        /// Vertex and index buffer set for primitive drawing.
        struct ResourceSet
        {
            /// Vertex buffer for untextured primitive rendering.
            RVertexBufferPtr spUntexturedVertexBuffer;
            /// Index buffer for untextured primitive rendering.
            RIndexBufferPtr spUntexturedIndexBuffer;

            /// Vertex buffer for primitive rendering with blending of each texture channel with the vertex colors.
            RVertexBufferPtr spTextureBlendVertexBuffer;
            /// Index buffer for primitive rendering with blending of each texture channel with the vertex colors.
            RIndexBufferPtr spTextureBlendIndexBuffer;

            /// Vertex buffer for primitive rendering with blending of the texture color with the vertex alpha.
            RVertexBufferPtr spTextureAlphaVertexBuffer;
            /// Index buffer for primitive rendering with blending of the texture color with the vertex alpha.
            RIndexBufferPtr spTextureAlphaIndexBuffer;

            /// Maximum number of vertices in the untextured primitive vertex buffer.
            uint32_t untexturedVertexBufferSize;
            /// Maximum number if indices in the untextured primitive index buffer.
            uint32_t untexturedIndexBufferSize;

            /// Maximum number of vertices in the blended texture primitive vertex buffer.
            uint32_t textureBlendVertexBufferSize;
            /// Maximum number of indices in the blended texture primitive vertex buffer.
            uint32_t textureBlendIndexBufferSize;

            /// Maximum number of vertices in the alpha texture primitive vertex buffer.
            uint32_t textureAlphaVertexBufferSize;
            /// Maximum number of indices in the alpha texture primitive vertex buffer.
            uint32_t textureAlphaIndexBufferSize;
        };

        /// Untextured draw call vertices.
        DynArray< SimpleVertex > m_untexturedVertices;
        /// Vertices for draw calls that blend each texture channel with the vertex color.
        DynArray< SimpleTexturedVertex > m_textureBlendVertices;
        /// Vertices for draw calls that blend the texture color (red channel) with the vertex alpha.
        DynArray< SimpleTexturedVertex > m_textureAlphaVertices;

        /// Untextured draw call indices.
        DynArray< uint16_t > m_untexturedIndices;
        /// Indices for draw calls that blend each texture channel with the vertex color.
        DynArray< uint16_t > m_textureBlendIndices;
        /// Indices for draw calls that blend the texture color (red channel) with the vertex alpha.
        DynArray< uint16_t > m_textureAlphaIndices;

        /// Line list draw call data.
        DynArray< UntexturedDrawCall > m_lineDrawCalls;
        /// Wireframe mesh draw call data
        DynArray< UntexturedDrawCall > m_wireMeshDrawCalls;
        /// Solid mesh draw call data.
        DynArray< UntexturedDrawCall > m_solidMeshDrawCalls;
        /// Textured mesh draw call data.
        DynArray< TexturedDrawCall > m_texturedMeshDrawCalls;
        /// Text draw call data.
        DynArray< TexturedDrawCall > m_textDrawCalls;

        /// Rendering resource data.
        ResourceSet m_resourceSets[ 2 ];
        /// Current resource set to use for buffered draw calls.
        size_t m_currentResourceSetIndex;

        /// True if the buffered draw commands are in use for rendering (no new commands can be buffered), false if not
        /// (new commands can be buffered).
        bool m_bDrawing;
    };
}
