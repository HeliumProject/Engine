#pragma once

#include "Graphics/Graphics.h"

#include "Rendering/RRenderResource.h"
#include "GraphicsTypes/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/RenderResourceManager.h"

namespace Lunar
{
    L_DECLARE_RPTR( RConstantBuffer );
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
        /// Maximum number of characters to convert for rendered text strings (including null terminator).
        static const size_t TEXT_CHARACTER_COUNT_MAX = 1024;

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

        void DrawWorldText(
            const Simd::Matrix44& rTransform, const String& rText, const Color& rColor = Color( 0xffffffff ),
            RenderResourceManager::EDebugFontSize size = RenderResourceManager::DEBUG_FONT_SIZE_MEDIUM );
        void DrawScreenText(
            int32_t x, int32_t y, const String& rText, const Color& rColor = Color( 0xffffffff ),
            RenderResourceManager::EDebugFontSize size = RenderResourceManager::DEBUG_FONT_SIZE_MEDIUM );
        //@}

        /// @name Rendering
        //@{
        void BeginDrawing();
        void EndDrawing();

        void DrawWorldElements();
        void DrawScreenElements();
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

        /// Text draw call information.
        struct TextDrawCall
        {
            /// Horizontal pixel coordinate at which to render the text.
            int32_t x;
            /// Vertical pixel coordinate at which to render the text.
            int32_t y;
            /// Text color.
            Color color;
            /// Text size.
            RenderResourceManager::EDebugFontSize size;
            /// Character glyph count;
            uint32_t glyphCount;
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

            /// Vertex buffer for screen-space text rendering.
            RVertexBufferPtr spScreenSpaceTextVertexBuffer;

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

            /// Maximum number of vertices in the screen-space text vertex buffer.
            uint32_t screenSpaceTextVertexBufferSize;
        };

        /// Glyph handler for rendering world-space text.
        class LUNAR_GRAPHICS_API WorldSpaceTextGlyphHandler : NonCopyable
        {
        public:
            /// @name Construction/Destruction
            //@{
            WorldSpaceTextGlyphHandler(
                BufferedDrawer* pDrawer, Font* pFont, const Color& rColor, const Simd::Matrix44& rTransform );
            //@}

            /// @name Overloaded Operators
            //@{
            void operator()( const Font::Character* pCharacter );
            //@}

        private:
            /// Reference to the rendering transform matrix.
            const Simd::Matrix44& m_rTransform;
            /// Pointer to the BufferedDrawer instance performing the rendering.
            BufferedDrawer* m_pDrawer;
            /// Font resource being used for rendering.
            Font* m_pFont;
            /// Text color.
            Color m_color;

            /// Cached indices to use for quad rendering.
            uint16_t m_quadIndices[ 6 ];

            /// Cached inverse width of each font texture sheet.
            float32_t m_inverseTextureWidth;
            /// Cached inverse height of each font texture sheet.
            float32_t m_inverseTextureHeight;

            /// Current horizontal pen coordinate.
            float32_t m_penX;
        };

        /// Glyph handler for rendering screen-space text.
        class LUNAR_GRAPHICS_API ScreenSpaceTextGlyphHandler : NonCopyable
        {
        public:
            /// @name Construction/Destruction
            //@{
            ScreenSpaceTextGlyphHandler(
                BufferedDrawer* pDrawer, Font* pFont, int32_t x, int32_t y, const Color& rColor,
                RenderResourceManager::EDebugFontSize size );
            //@}

            /// @name Overloaded Operators
            //@{
            void operator()( const Font::Character* pCharacter );
            //@}

        private:
            /// Pointer to the BufferedDrawer instance performing the rendering.
            BufferedDrawer* m_pDrawer;
            /// Font resource being used for rendering.
            Font* m_pFont;
            /// Text draw call to update.
            TextDrawCall* m_pDrawCall;

            /// Pixel x-coordinate at which to begin rendering the text.
            int32_t m_x;
            /// Pixel y-coordinate at which to begin rendering the text.
            int32_t m_y;
            /// Color with which to render the text.
            Color m_color;
            /// Size at which to render the text.
            RenderResourceManager::EDebugFontSize m_size;
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
        /// World-space text draw call data.
        DynArray< TexturedDrawCall > m_worldTextDrawCalls;

        /// Screen-space text draw call data.
        DynArray< TextDrawCall > m_screenTextDrawCalls;
        /// Screen-space text draw call glyph indices.
        DynArray< uint32_t > m_screenTextGlyphIndices;

        /// Index buffer for screen-space text rendering.
        RIndexBufferPtr m_spScreenSpaceTextIndexBuffer;

        /// Rendering resource data.
        ResourceSet m_resourceSets[ 2 ];
        /// Current resource set to use for buffered draw calls.
        size_t m_currentResourceSetIndex;

        /// True if the buffered draw commands are in use for rendering (no new commands can be buffered), false if not
        /// (new commands can be buffered).
        bool m_bDrawing;
    };
}
