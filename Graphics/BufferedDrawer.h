#pragma once

#include "Graphics/Graphics.h"

#include "Rendering/RRenderResource.h"
#include "GraphicsTypes/VertexTypes.h"

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

            /// Vertex buffer for textured primitive rendering.
            RVertexBufferPtr spTexturedVertexBuffer;
            /// Index buffer for textured primitive rendering.
            RIndexBufferPtr spTexturedIndexBuffer;

            /// Maximum number of vertices in the untextured primitive vertex buffer.
            uint32_t untexturedVertexBufferSize;
            /// Maximum number if indices in the untextured primitive index buffer.
            uint32_t untexturedIndexBufferSize;

            /// Maximum number of vertices in the textured primitive vertex buffer.
            uint32_t texturedVertexBufferSize;
            /// Maximum number of indices in the textured primitive vertex buffer.
            uint32_t texturedIndexBufferSize;
        };

        /// Untextured draw call vertices.
        DynArray< SimpleVertex > m_untexturedVertices;
        /// Textured draw call vertices.
        DynArray< SimpleTexturedVertex > m_texturedVertices;

        /// Untextured draw call indices.
        DynArray< uint16_t > m_untexturedIndices;
        /// Textured draw call indices.
        DynArray< uint16_t > m_texturedIndices;

        /// Line list draw call data.
        DynArray< UntexturedDrawCall > m_lineDrawCalls;
        /// Wireframe mesh draw call data
        DynArray< UntexturedDrawCall > m_wireMeshDrawCalls;
        /// Solid mesh draw call data.
        DynArray< UntexturedDrawCall > m_solidMeshDrawCalls;
        /// Textured mesh draw call data.
        DynArray< TexturedDrawCall > m_texturedMeshDrawCalls;

        /// Rendering resource data.
        ResourceSet m_resourceSets[ 2 ];
        /// Current resource set to use for buffered draw calls.
        size_t m_currentResourceSetIndex;

        /// True if the buffered draw commands are in use for rendering (no new commands can be buffered), false if not
        /// (new commands can be buffered).
        bool m_bDrawing;
    };
}
