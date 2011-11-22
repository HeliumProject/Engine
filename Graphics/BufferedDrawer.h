#pragma once

#include "Graphics/Graphics.h"

#include "Foundation/Math/SimdMatrix44.h"
#include "Rendering/RRenderResource.h"
#include "GraphicsTypes/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/RenderResourceManager.h"

namespace Helium
{
    HELIUM_DECLARE_RPTR( RConstantBuffer );
    HELIUM_DECLARE_RPTR( RFence );
    HELIUM_DECLARE_RPTR( RIndexBuffer );
    HELIUM_DECLARE_RPTR( RPixelShader );
    HELIUM_DECLARE_RPTR( RRenderCommandProxy );
    HELIUM_DECLARE_RPTR( RTexture2d );
    HELIUM_DECLARE_RPTR( RVertexBuffer );
    HELIUM_DECLARE_RPTR( RVertexDescription );
    HELIUM_DECLARE_RPTR( RVertexInputLayout );
    HELIUM_DECLARE_RPTR( RVertexShader );

    /// Buffered drawing interface.
    class HELIUM_GRAPHICS_API BufferedDrawer : NonCopyable
    {
    public:
        /// Number of constant buffers to cycle through for vertex shader transform data.
        static const size_t INSTANCE_VERTEX_CONSTANT_BUFFER_COUNT = 64;
        /// Number of constant buffers to cycle through for pixel shader blend color parameters.
        static const size_t INSTANCE_PIXEL_CONSTANT_BUFFER_COUNT = 16;

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
        void DrawUntextured(
            ERendererPrimitiveType primitiveType, const SimpleVertex* pVertices, uint32_t vertexCount,
            const uint16_t* pIndices, uint32_t primitiveCount, Color blendColor = Color( 0xffffffff ),
            RenderResourceManager::ERasterizerState rasterizerState = RenderResourceManager::RASTERIZER_STATE_DEFAULT,
            RenderResourceManager::EDepthStencilState depthStencilState = RenderResourceManager::DEPTH_STENCIL_STATE_DEFAULT );
        void DrawUntextured(
            ERendererPrimitiveType primitiveType, const Simd::Matrix44& rTransform, RVertexBuffer* pVertices,
            RIndexBuffer* pIndices, uint32_t baseVertexIndex, uint32_t vertexCount, uint32_t startIndex,
            uint32_t primitiveCount, Color blendColor = Color( 0xffffffff ),
            RenderResourceManager::ERasterizerState rasterizerState = RenderResourceManager::RASTERIZER_STATE_DEFAULT,
            RenderResourceManager::EDepthStencilState depthStencilState = RenderResourceManager::DEPTH_STENCIL_STATE_DEFAULT );

        void DrawTextured(
            ERendererPrimitiveType primitiveType, const SimpleTexturedVertex* pVertices, uint32_t vertexCount,
            const uint16_t* pIndices, uint32_t primitiveCount, RTexture2d* pTexture,
            Color blendColor = Color( 0xffffffff ),
            RenderResourceManager::ERasterizerState rasterizerState = RenderResourceManager::RASTERIZER_STATE_DEFAULT,
            RenderResourceManager::EDepthStencilState depthStencilState = RenderResourceManager::DEPTH_STENCIL_STATE_DEFAULT );
        void DrawTextured(
            ERendererPrimitiveType primitiveType, const Simd::Matrix44& rTransform, RVertexBuffer* pVertices,
            RIndexBuffer* pIndices, uint32_t baseVertexIndex, uint32_t vertexCount, uint32_t startIndex,
            uint32_t primitiveCount, RTexture2d* pTexture, Color blendColor = Color( 0xffffffff ),
            RenderResourceManager::ERasterizerState rasterizerState = RenderResourceManager::RASTERIZER_STATE_DEFAULT,
            RenderResourceManager::EDepthStencilState depthStencilState = RenderResourceManager::DEPTH_STENCIL_STATE_DEFAULT );

        void DrawPoints(
            const SimpleVertex* pVertices, uint32_t pointCount, Color blendColor = Color( 0xffffffff ),
            RenderResourceManager::EDepthStencilState depthStencilState = RenderResourceManager::DEPTH_STENCIL_STATE_DEFAULT );
        void DrawPoints(
            const Simd::Matrix44& rTransform, RVertexBuffer* pVertices, uint32_t baseVertexIndex, uint32_t pointCount,
            Color blendColor = Color( 0xffffffff ),
            RenderResourceManager::EDepthStencilState depthStencilState = RenderResourceManager::DEPTH_STENCIL_STATE_DEFAULT );

        void DrawWorldText(
            const Simd::Matrix44& rTransform, const String& rText, Color color = Color( 0xffffffff ),
            RenderResourceManager::EDebugFontSize size = RenderResourceManager::DEBUG_FONT_SIZE_MEDIUM,
            RenderResourceManager::ERasterizerState rasterizerState = RenderResourceManager::RASTERIZER_STATE_DEFAULT,
            RenderResourceManager::EDepthStencilState depthStencilState = RenderResourceManager::DEPTH_STENCIL_STATE_DEFAULT );
        void DrawScreenText(
            int32_t x, int32_t y, const String& rText, Color color = Color( 0xffffffff ),
            RenderResourceManager::EDebugFontSize size = RenderResourceManager::DEBUG_FONT_SIZE_MEDIUM );
        void DrawProjectedText(
            const Simd::Vector3& rWorldOffset, int32_t screenOffsetX, int32_t screenOffsetY, const String& rText,
            Color color = Color( 0xffffffff ),
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

        /// Screen-space text draw call information.
        struct ScreenTextDrawCall
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

        /// Projected text draw call information.
        struct ProjectedTextDrawCall : ScreenTextDrawCall
        {
            /// Base world-space position at which to render the text.
            float32_t worldPosition[ 3 ];
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

            /// Vertex buffer for screen-space text rendering.
            RVertexBufferPtr spScreenSpaceTextVertexBuffer;
            /// Vertex buffer for projected text rendering.
            RVertexBufferPtr spProjectedTextVertexBuffer;

            /// Vertex constant buffers.
            RConstantBufferPtr instanceVertexConstantBuffers[ INSTANCE_VERTEX_CONSTANT_BUFFER_COUNT ];
            /// Pixel constant buffers.
            RConstantBufferPtr instancePixelConstantBuffers[ INSTANCE_PIXEL_CONSTANT_BUFFER_COUNT ];

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
            /// Maximum number of vertices in the projected text vertex buffer.
            uint32_t projectedTextVertexBufferSize;
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
            void SetDepthStencilState( RDepthStencilState* pState, uint8_t stencilReferenceValue );

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
            RRasterizerStatePtr m_spRasterizerState;
            /// Current blend state.
            RBlendStatePtr m_spBlendState;
            /// Current depth-stencil state.
            RDepthStencilStatePtr m_spDepthStencilState;
            /// Current stencil reference value.
            uint8_t m_stencilReferenceValue;

            /// Current vertex buffer.
            RVertexBufferPtr m_spVertexBuffer;
            /// Current vertex stride.
            uint32_t m_vertexStride;

            /// Current index buffer.
            RIndexBufferPtr m_spIndexBuffer;

            /// Current vertex shader.
            RVertexShaderPtr m_spVertexShader;
            /// Current pixel shader.
            RPixelShaderPtr m_spPixelShader;
            /// Current vertex input layout.
            RVertexInputLayoutPtr m_spVertexInputLayout;

            /// Current vertex shader constant buffer.
            RConstantBufferPtr m_spVertexConstantBuffer;
            /// Current pixel shader constant buffer.
            RConstantBufferPtr m_spPixelConstantBuffer;

            /// Current texture.
            RTexture2dPtr m_spTexture;

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
        class HELIUM_GRAPHICS_API WorldSpaceTextGlyphHandler : NonCopyable
        {
        public:
            /// @name Construction/Destruction
            //@{
            WorldSpaceTextGlyphHandler(
                BufferedDrawer* pDrawer, Font* pFont, Color color,
                RenderResourceManager::ERasterizerState rasterizerState,
                RenderResourceManager::EDepthStencilState depthStencilState, const Simd::Matrix44& rTransform );
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
            /// Draw call set index for the desired rasterizer and depth-stencil state.
            size_t m_stateIndex;
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
        class HELIUM_GRAPHICS_API ScreenSpaceTextGlyphHandler : NonCopyable
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
            ScreenTextDrawCall* m_pDrawCall;

            /// Pixel x-coordinate at which to begin rendering the text.
            int32_t m_x;
            /// Pixel y-coordinate at which to begin rendering the text.
            int32_t m_y;
            /// Color with which to render the text.
            Color m_color;
            /// Size at which to render the text.
            RenderResourceManager::EDebugFontSize m_size;
        };

        /// Glyph handler for rendering projected text.
        class HELIUM_GRAPHICS_API ProjectedTextGlyphHandler : NonCopyable
        {
        public:
            /// @name Construction/Destruction
            //@{
            ProjectedTextGlyphHandler(
                BufferedDrawer* pDrawer, Font* pFont, const Simd::Vector3& rWorldOffset, int32_t screenOffsetX,
                int32_t screenOffsetY, Color color, RenderResourceManager::EDebugFontSize size );
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
            ProjectedTextDrawCall* m_pDrawCall;

            /// World x-coordinate at which to begin rendering the text.
            float32_t m_worldOffsetX;
            /// World y-coordinate at which to begin rendering the text.
            float32_t m_worldOffsetY;
            /// World z-coordinate at which to begin rendering the text.
            float32_t m_worldOffsetZ;

            /// Pixel x-coordinate at which to begin rendering the text.
            int32_t m_screenOffsetX;
            /// Pixel y-coordinate at which to begin rendering the text.
            int32_t m_screenOffsetY;

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

        /// Untextured draw call data using internal vertex/index buffers.
        DynArray< UntexturedDrawCall > m_untexturedDrawCalls[ RenderResourceManager::RASTERIZER_STATE_MAX * RenderResourceManager::DEPTH_STENCIL_STATE_MAX ];
        /// Textured draw call data using internal vertex/index buffers.
        DynArray< TexturedDrawCall > m_texturedDrawCalls[ RenderResourceManager::RASTERIZER_STATE_MAX * RenderResourceManager::DEPTH_STENCIL_STATE_MAX ];
        /// Point draw call data using internal vertex/index buffers.
        DynArray< UntexturedDrawCall > m_pointDrawCalls[ RenderResourceManager::DEPTH_STENCIL_STATE_MAX ];

        /// Untextured draw call data using external vertex/index buffers.
        DynArray< UntexturedBufferDrawCall > m_untexturedBufferDrawCalls[ RenderResourceManager::RASTERIZER_STATE_MAX * RenderResourceManager::DEPTH_STENCIL_STATE_MAX ];
        /// Textured draw call data using external vertex/index buffers.
        DynArray< TexturedBufferDrawCall > m_texturedBufferDrawCalls[ RenderResourceManager::RASTERIZER_STATE_MAX * RenderResourceManager::DEPTH_STENCIL_STATE_MAX ];
        /// Point draw call data using external vertex/index buffers.
        DynArray< UntexturedBufferDrawCall > m_pointBufferDrawCalls[ RenderResourceManager::DEPTH_STENCIL_STATE_MAX ];

        /// World-space text draw call data.
        DynArray< TexturedDrawCall > m_worldTextDrawCalls[ RenderResourceManager::RASTERIZER_STATE_MAX * RenderResourceManager::DEPTH_STENCIL_STATE_MAX ];

        /// Screen-space text draw call data.
        DynArray< ScreenTextDrawCall > m_screenTextDrawCalls;
        /// Screen-space text draw call glyph indices.
        DynArray< uint32_t > m_screenTextGlyphIndices;

        /// Projected text draw call data.
        DynArray< ProjectedTextDrawCall > m_projectedTextDrawCalls;
        /// Projected text draw call glyph indices.
        DynArray< uint32_t > m_projectedTextGlyphIndices;

        /// Index buffer for screen-space text rendering.
        RIndexBufferPtr m_spScreenSpaceTextIndexBuffer;

        /// Render fences used to mark the end of when a per-instance vertex shader constant buffer is in use.
        RFencePtr m_instanceVertexConstantFences[ INSTANCE_VERTEX_CONSTANT_BUFFER_COUNT ];
        /// Current instance vertex constant buffer transform.
        Simd::Matrix44 m_instanceVertexConstantTransform;
        /// Index of the current instance vertex constant buffer.
        uint32_t m_instanceVertexConstantBufferIndex;

        /// Render fences used to mark the end of when a per-instance pixel shader constant buffer is in use.
        RFencePtr m_instancePixelConstantFences[ INSTANCE_PIXEL_CONSTANT_BUFFER_COUNT ];
        /// Current instance pixel constant buffer blend color.
        Color m_instancePixelConstantBlendColor;
        /// Index of the current instance pixel constant buffer.
        uint32_t m_instancePixelConstantBufferIndex;

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

        void DrawDepthStencilStateWorldElements(
            WorldElementResources& rWorldResources, RenderResourceManager::EDepthStencilState depthStencilState );
        void DrawStateWorldElements(
            WorldElementResources& rWorldResources, RenderResourceManager::ERasterizerState rasterizerState,
            RenderResourceManager::EDepthStencilState depthStencilState );
        //@}

        /// @name Static Utility Functions
        //@{
        static size_t GetStateIndex(
            RenderResourceManager::ERasterizerState rasterizerState,
            RenderResourceManager::EDepthStencilState depthStencilState );
        static void GetStatesFromIndex(
            size_t stateIndex, RenderResourceManager::ERasterizerState& rRasterizerState,
            RenderResourceManager::EDepthStencilState& rDepthStencilState );
        //@}
    };
}
