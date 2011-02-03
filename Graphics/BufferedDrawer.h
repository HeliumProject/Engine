#pragma once

#include "Graphics/Graphics.h"

#include "Platform/Math/Simd/Matrix44.h"
#include "Rendering/RRenderResource.h"
#include "GraphicsTypes/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/RenderResourceManager.h"

namespace Lunar
{
    class RVertexInputLayout;

    L_DECLARE_RPTR( RConstantBuffer );
    L_DECLARE_RPTR( RFence );
    L_DECLARE_RPTR( RIndexBuffer );
    L_DECLARE_RPTR( RPixelShader );
    L_DECLARE_RPTR( RRenderCommandProxy );
    L_DECLARE_RPTR( RTexture2d );
    L_DECLARE_RPTR( RVertexBuffer );
    L_DECLARE_RPTR( RVertexDescription );
    L_DECLARE_RPTR( RVertexShader );

    /// Buffered drawing interface.
    class LUNAR_GRAPHICS_API BufferedDrawer : NonCopyable
    {
    public:
        /// Number of constant buffers to cycle through for vertex shader transform data.
        static const size_t INSTANCE_VERTEX_CONSTANT_BUFFER_COUNT = 64;
        /// Number of constant buffers to cycle through for pixel shader blend color parameters.
        static const size_t INSTANCE_PIXEL_CONSTANT_BUFFER_COUNT = 16;

        /// Maximum number of characters to convert for rendered text strings (including null terminator).
        static const size_t TEXT_CHARACTER_COUNT_MAX = 1024;

        /// Depth buffer modes.
        enum EDepthMode
        {
            DEPTH_MODE_FIRST   =  0,
            DEPTH_MODE_INVALID = -1,

            /// Enable depth tests and writes.
            DEPTH_MODE_ENABLED,
            /// Disable depth tests and writes.
            DEPTH_MODE_DISABLED,

            DEPTH_MODE_MAX,
            DEPTH_MODE_LAST = DEPTH_MODE_MAX - 1
        };

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
        void DrawWire(
            ERendererPrimitiveType primitiveType, const SimpleVertex* pVertices, uint32_t vertexCount,
            const uint16_t* pIndices, uint32_t primitiveCount, Color blendColor = Color( 0xffffffff ),
            EDepthMode depthMode = DEPTH_MODE_ENABLED );
        void DrawWire(
            ERendererPrimitiveType primitiveType, const Simd::Matrix44& rTransform, RVertexBuffer* pVertices,
            RIndexBuffer* pIndices, uint32_t baseVertexIndex, uint32_t vertexCount, uint32_t startIndex,
            uint32_t primitiveCount, Color blendColor = Color( 0xffffffff ),
            EDepthMode depthMode = DEPTH_MODE_ENABLED );

        void DrawSolid(
            ERendererPrimitiveType primitiveType, const SimpleVertex* pVertices, uint32_t vertexCount,
            const uint16_t* pIndices, uint32_t primitiveCount, Color blendColor = Color( 0xffffffff ),
            EDepthMode depthMode = DEPTH_MODE_ENABLED );
        void DrawSolid(
            ERendererPrimitiveType primitiveType, const Simd::Matrix44& rTransform, RVertexBuffer* pVertices,
            RIndexBuffer* pIndices, uint32_t baseVertexIndex, uint32_t vertexCount, uint32_t startIndex,
            uint32_t primitiveCount, Color blendColor = Color( 0xffffffff ),
            EDepthMode depthMode = DEPTH_MODE_ENABLED );

        void DrawTextured(
            ERendererPrimitiveType primitiveType, const SimpleTexturedVertex* pVertices, uint32_t vertexCount,
            const uint16_t* pIndices, uint32_t primitiveCount, RTexture2d* pTexture,
            Color blendColor = Color( 0xffffffff ), EDepthMode depthMode = DEPTH_MODE_ENABLED );
        void DrawTextured(
            ERendererPrimitiveType primitiveType, const Simd::Matrix44& rTransform, RVertexBuffer* pVertices,
            RIndexBuffer* pIndices, uint32_t baseVertexIndex, uint32_t vertexCount, uint32_t startIndex,
            uint32_t primitiveCount, RTexture2d* pTexture, Color blendColor = Color( 0xffffffff ),
            EDepthMode depthMode = DEPTH_MODE_ENABLED );

        void DrawPoints(
            const SimpleVertex* pVertices, uint32_t pointCount, Color blendColor = Color( 0xffffffff ),
            EDepthMode depthMode = DEPTH_MODE_ENABLED );
        void DrawPoints(
            const Simd::Matrix44& rTransform, RVertexBuffer* pVertices, uint32_t baseVertexIndex, uint32_t pointCount,
            Color blendColor = Color( 0xffffffff ), EDepthMode depthMode = DEPTH_MODE_ENABLED );

        void DrawWorldText(
            const Simd::Matrix44& rTransform, const String& rText, Color color = Color( 0xffffffff ),
            RenderResourceManager::EDebugFontSize size = RenderResourceManager::DEBUG_FONT_SIZE_MEDIUM,
            EDepthMode depthMode = DEPTH_MODE_ENABLED );
        void DrawScreenText(
            int32_t x, int32_t y, const String& rText, Color color = Color( 0xffffffff ),
            RenderResourceManager::EDebugFontSize size = RenderResourceManager::DEBUG_FONT_SIZE_MEDIUM );
        //@}

        /// @name Rendering
        //@{
        void BeginDrawing();
        void EndDrawing();

        void DrawWorldElements( const Simd::Matrix44& rInverseViewProjection );
        void DrawScreenElements();
        //@}

    private:
        /// Untextured primitive draw call information using internal vertex/index buffers.
        struct UntexturedDrawCall
        {
            /// Primitive type.
            ERendererPrimitiveType primitiveType;
            /// Starting vertex index.
            uint32_t baseVertexIndex;
            /// Vertex count.
            uint32_t vertexCount;
            /// Starting index offset.
            uint32_t startIndex;
            /// Number of primitives to draw.
            uint32_t primitiveCount;
            /// Color with which to blend each vertex color.
            Color blendColor;
        };

        /// Textured primitive draw call information using internal vertex/index buffers.
        struct TexturedDrawCall : UntexturedDrawCall
        {
            /// Texture with which to draw.
            RTexture2dPtr spTexture;
        };

        /// Untextured primitive draw call information using external vertex/index buffers.
        HELIUM_SIMD_ALIGN_PRE struct UntexturedBufferDrawCall : UntexturedDrawCall
        {
            /// Vertex buffer.
            RVertexBufferPtr spVertexBuffer;
            /// Index buffer.
            RIndexBufferPtr spIndexBuffer;

            /// World transform.
            Simd::Matrix44 transform;
        } HELIUM_SIMD_ALIGN_POST;

        /// Textured primitive draw call information using external vertex/index buffers.
        HELIUM_SIMD_ALIGN_PRE struct TexturedBufferDrawCall : UntexturedBufferDrawCall
        {
            /// Texture with which to draw.
            RTexture2dPtr spTexture;
        } HELIUM_SIMD_ALIGN_POST;

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
        HELIUM_SIMD_ALIGN_PRE struct ResourceSet
        {
            /// Vertex buffer for untextured primitive rendering.
            RVertexBufferPtr spUntexturedVertexBuffer;
            /// Index buffer for untextured primitive rendering.
            RIndexBufferPtr spUntexturedIndexBuffer;

            /// Vertex buffer for textured primitive rendering.
            RVertexBufferPtr spTexturedVertexBuffer;
            /// Index buffer for textured primitive rendering.
            RIndexBufferPtr spTexturedIndexBuffer;

            /// Vertex buffer for screen-space text rendering.
            RVertexBufferPtr spScreenSpaceTextVertexBuffer;

            /// Vertex constant buffers.
            RConstantBufferPtr instanceVertexConstantBuffers[ INSTANCE_VERTEX_CONSTANT_BUFFER_COUNT ];
            /// Current instance vertex constant buffer transform.
            Simd::Matrix44 instanceVertexConstantTransform;
            /// Index of the current instance vertex constant buffer.
            uint32_t instanceVertexConstantBufferIndex;

            /// Pixel constant buffers.
            RConstantBufferPtr instancePixelConstantBuffers[ INSTANCE_PIXEL_CONSTANT_BUFFER_COUNT ];
            /// Current instance pixel constant buffer blend color.
            Color instancePixelConstantBlendColor;
            /// Index of the current instance pixel constant buffer.
            uint32_t instancePixelConstantBufferIndex;

            /// Maximum number of vertices in the untextured primitive vertex buffer.
            uint32_t untexturedVertexBufferSize;
            /// Maximum number if indices in the untextured primitive index buffer.
            uint32_t untexturedIndexBufferSize;

            /// Maximum number of vertices in the textured primitive vertex buffer.
            uint32_t texturedVertexBufferSize;
            /// Maximum number of indices in the textured primitive index buffer.
            uint32_t texturedIndexBufferSize;

            /// Maximum number of vertices in the screen-space text vertex buffer.
            uint32_t screenSpaceTextVertexBufferSize;
        } HELIUM_SIMD_ALIGN_POST;

        /// Cached renderer state information.
        class StateCache
        {
        public:
            /// @name Construction/Destruction
            //@{
            explicit StateCache( RRenderCommandProxy* pCommandProxy = NULL );
            //@}

            /// @name Render Command Proxy Modification
            //@{
            void SetRenderCommandProxy( RRenderCommandProxy* pCommandProxy );
            //@}

            /// @name State Modification
            //@{
            void SetRasterizerState( RRasterizerState* pState );
            void SetBlendState( RBlendState* pState );

            void SetVertexBuffer( RVertexBuffer* pBuffer, uint32_t stride );
            void SetIndexBuffer( RIndexBuffer* pBuffer );

            void SetVertexShader( RVertexShader* pShader );
            void SetPixelShader( RPixelShader* pShader );
            void SetVertexInputLayout( RVertexInputLayout* pLayout );

            void SetVertexConstantBuffer( RConstantBuffer* pConstantBuffer );
            void SetPixelConstantBuffer( RConstantBuffer* pConstantBuffer );

            void SetTexture( RTexture2d* pTexture );
            //@}

        private:
            /// Render command proxy used to issue render commands.
            RRenderCommandProxyPtr m_spRenderCommandProxy;

            /// Current rasterizer state.
            RRasterizerState* m_pRasterizerState;
            /// Current blend state.
            RBlendState* m_pBlendState;

            /// Current vertex buffer.
            RVertexBuffer* m_pVertexBuffer;
            /// Current vertex stride.
            uint32_t m_vertexStride;

            /// Current index buffer.
            RIndexBuffer* m_pIndexBuffer;

            /// Current vertex shader.
            RVertexShader* m_pVertexShader;
            /// Current pixel shader.
            RPixelShader* m_pPixelShader;
            /// Current vertex input layout.
            RVertexInputLayout* m_pVertexInputLayout;

            /// Current vertex shader constant buffer.
            RConstantBuffer* m_pVertexConstantBuffer;
            /// Current pixel shader constant buffer.
            RConstantBuffer* m_pPixelConstantBuffer;

            /// Current texture.
            RTexture2d* m_pTexture;

            /// @name Private Utility Functions
            //@{
            void ResetStateCache();
            //@}
        };

        /// Rendering resources used for drawing world elements.
        HELIUM_SIMD_ALIGN_PRE struct WorldElementResources
        {
            /// Inverse view/projection matrix.
            Simd::Matrix44 inverseViewProjection;

            /// Render command proxy interface.
            RRenderCommandProxyPtr spCommandProxy;

            /// Untextured rendering vertex shader.
            RVertexShaderPtr spUntexturedVertexShader;
            /// Untextured point sprite vertex shader.
            RVertexShaderPtr spUntexturedPointsVertexShader;
            /// Untextured rendering pixel shader.
            RPixelShaderPtr spUntexturedPixelShader;

            /// Vertex shader for textured rendering blended with the vertex color.
            RVertexShaderPtr spTextureBlendVertexShader;
            /// Pixel shader for textured rendering blended with the vertex color.
            RPixelShaderPtr spTextureBlendPixelShader;

            /// Vertex shader for textured rendering blending the vertex color with the texture alpha.
            RVertexShaderPtr spTextureAlphaVertexShader;
            /// Pixel shader for textured rendering blending the vertex color with the texture alpha.
            RPixelShaderPtr spTextureAlphaPixelShader;

            /// Cached reference to the vertex description for SimpleVertex.
            RVertexDescriptionPtr spSimpleVertexDescription;
            /// Cached reference to the vertex description for SimpleTexturedVertex;
            RVertexDescriptionPtr spSimpleTexturedVertexDescription;

            /// Render state cache.
            StateCache* pStateCache;
        } HELIUM_SIMD_ALIGN_POST;

        /// Glyph handler for rendering world-space text.
        class LUNAR_GRAPHICS_API WorldSpaceTextGlyphHandler : NonCopyable
        {
        public:
            /// @name Construction/Destruction
            //@{
            WorldSpaceTextGlyphHandler(
                BufferedDrawer* pDrawer, Font* pFont, Color color, EDepthMode depthMode,
                const Simd::Matrix44& rTransform );
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
            /// Mode in which to handle depth testing and writing.
            EDepthMode m_depthMode;

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
                BufferedDrawer* pDrawer, Font* pFont, int32_t x, int32_t y, Color color,
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
        /// Textured draw call vertices.
        DynArray< SimpleTexturedVertex > m_texturedVertices;

        /// Untextured draw call indices.
        DynArray< uint16_t > m_untexturedIndices;
        /// Textured draw call indices.
        DynArray< uint16_t > m_texturedIndices;

        /// Wireframe draw call data using internal vertex/index buffers.
        DynArray< UntexturedDrawCall > m_wireDrawCalls[ DEPTH_MODE_MAX ];
        /// Solid draw call data using internal vertex/index buffers.
        DynArray< UntexturedDrawCall > m_solidDrawCalls[ DEPTH_MODE_MAX ];
        /// Textured draw call data using internal vertex/index buffers.
        DynArray< TexturedDrawCall > m_texturedDrawCalls[ DEPTH_MODE_MAX ];
        /// Point draw call data using internal vertex/index buffers.
        DynArray< UntexturedDrawCall > m_pointDrawCalls[ DEPTH_MODE_MAX ];

        /// Wireframe draw call data using external vertex/index buffers.
        DynArray< UntexturedBufferDrawCall > m_wireBufferDrawCalls[ DEPTH_MODE_MAX ];
        /// Solid draw call data using external vertex/index buffers.
        DynArray< UntexturedBufferDrawCall > m_solidBufferDrawCalls[ DEPTH_MODE_MAX ];
        /// Textured draw call data using external vertex/index buffers.
        DynArray< TexturedBufferDrawCall > m_texturedBufferDrawCalls[ DEPTH_MODE_MAX ];
        /// Point draw call data using external vertex/index buffers.
        DynArray< UntexturedBufferDrawCall > m_pointBufferDrawCalls[ DEPTH_MODE_MAX ];

        /// World-space text draw call data.
        DynArray< TexturedDrawCall > m_worldTextDrawCalls[ DEPTH_MODE_MAX ];

        /// Screen-space text draw call data.
        DynArray< TextDrawCall > m_screenTextDrawCalls;
        /// Screen-space text draw call glyph indices.
        DynArray< uint32_t > m_screenTextGlyphIndices;

        /// Index buffer for screen-space text rendering.
        RIndexBufferPtr m_spScreenSpaceTextIndexBuffer;

        /// Render fences used to mark the end of when a per-instance vertex shader constant buffer is in use.
        RFencePtr m_instanceVertexConstantFences[ INSTANCE_VERTEX_CONSTANT_BUFFER_COUNT ];
        /// Render fences used to mark the end of when a per-instance pixel shader constant buffer is in use.
        RFencePtr m_instancePixelConstantFences[ INSTANCE_PIXEL_CONSTANT_BUFFER_COUNT ];

        /// Rendering resource data.
        ResourceSet m_resourceSets[ 2 ];
        /// Current resource set to use for buffered draw calls.
        size_t m_currentResourceSetIndex;

        /// True if the buffered draw commands are in use for rendering (no new commands can be buffered), false if not
        /// (new commands can be buffered).
        bool m_bDrawing;

        /// @name Rendering Utility Functions
        //@{
        RConstantBuffer* SetInstanceVertexConstantData(
            RRenderCommandProxy* pCommandProxy, ResourceSet& rResourceSet, const Simd::Matrix44& rInverseViewProjection,
            const Simd::Matrix44& rTransform );
        RConstantBuffer* SetInstancePixelConstantData(
            RRenderCommandProxy* pCommandProxy, ResourceSet& rResourceSet, Color blendColor );

        void DrawDepthModeWorldElements( WorldElementResources& rWorldResources, EDepthMode depthMode );
        //@}
    };
}
