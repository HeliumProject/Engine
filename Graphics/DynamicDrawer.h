//----------------------------------------------------------------------------------------------------------------------
// DynamicDrawer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_GRAPHICS_DYNAMIC_DRAWER_H
#define LUNAR_GRAPHICS_DYNAMIC_DRAWER_H

#include "Graphics/Graphics.h"

#include "Platform/Math/Float16.h"
#include "Platform/Math/Simd/Vector2.h"
#include "Platform/Math/Simd/Vector3.h"
#include "Rendering/Color.h"
#include "Rendering/RRenderResource.h"

namespace Lunar
{
    class Renderer;
    class RRenderCommandProxy;

    L_DECLARE_RPTR( RFence );
    L_DECLARE_RPTR( RIndexBuffer );
    L_DECLARE_RPTR( RPixelShader );
    L_DECLARE_RPTR( RTexture2d );
    L_DECLARE_RPTR( RVertexBuffer );
    L_DECLARE_RPTR( RVertexDescription );
    L_DECLARE_RPTR( RVertexShader );

    /// Dynamic drawing interface.
    class LUNAR_GRAPHICS_API DynamicDrawer : NonCopyable
    {
    public:
        /// Number of vertices per dynamic buffer division.
        static const uint32_t BUFFER_DIVISION_VERTEX_COUNT = 128 * 4;
        /// Number of vertex indices per dynamic buffer division.
        static const uint32_t BUFFER_DIVISION_INDEX_COUNT = 128 * 6;
        /// Number of dynamic buffer divisions.
        static const uint32_t BUFFER_DIVISION_COUNT = 8;

        /// Number of simultaneous textured triangle buffers.
        static const size_t TEXTURED_TRIANGLE_BUFFER_COUNT = 4;

        /// Untextured vertex.
        struct Vertex
        {
            /// Vertex position.
            float32_t position[ 3 ];
            /// Packed color (see Color class for more information).
            uint32_t color;

            /// @name Construction/Destruction
            //@{
            inline Vertex();
            inline Vertex(
                float32_t positionX, float32_t positionY, float32_t positionZ, uint32_t packedColor = 0xffffffff );
            inline Vertex( const Simd::Vector3& rPosition, const Color& rColor = Color( 0xffffffff ) );
            //@}
        };

        /// Textured vertex.
        struct VertexTextured
        {
            /// Vertex position.
            float32_t position[ 3 ];
            /// Packed color (see Color class for more information).
            uint32_t color;
            /// Texture coordinates.
            Float16 texCoords[ 2 ];

            /// @name Construction/Destruction
            //@{
            inline VertexTextured();
            inline VertexTextured(
                float32_t positionX, float32_t positionY, float32_t positionZ, Float16 texCoordU, Float16 texCoordV,
                uint32_t packedColor = 0xffffffff );
            inline VertexTextured(
                const Simd::Vector3& rPosition, const Simd::Vector2& rTexCoords,
                const Color& rColor = Color( 0xffffffff ) );
            //@}
        };

        /// @name Initialization
        //@{
        bool Initialize();
        void Shutdown();
        //@}

        /// @name Drawing
        //@{
        void Begin();

        void DrawScreenSpaceQuad(
            const Vertex& rVertex0, const Vertex& rVertex1, const Vertex& rVertex2, const Vertex& rVertex3,
            bool bFlush = false );
        void DrawScreenSpaceQuad(
            const VertexTextured& rVertex0, const VertexTextured& rVertex1, const VertexTextured& rVertex2,
            const VertexTextured& rVertex3, RTexture2d* pTexture, bool bFlush = false );

        void Flush();
        //@}

        /// @name Static Access
        //@{
        static DynamicDrawer& GetStaticInstance();
        static void DestroyStaticInstance();
        //@}

    private:
        /// Dynamic primitive buffer information.
        template<
            typename VertexType, typename Functions, uint32_t DivisionCount, uint32_t DivisionVertexCount,
            uint32_t DivisionIndexCount >
        class BufferData
        {
        public:
            /// Vertex buffer.
            RVertexBufferPtr m_spVertices;
            /// Index buffer.
            RIndexBufferPtr m_spIndices;
            /// Mapped pointer for the vertex buffer data.
            uint8_t* m_pMappedVertices;
            /// Mapped pointer for the index buffer data.
            uint16_t* m_pMappedIndices;

            /// Pending fences for draw calls for each buffer division.
            RFencePtr m_divisionFences[ DivisionCount ];

            /// Current buffer division.
            uint32_t m_divisionIndex;
            /// Total number of vertices in the current division.
            uint32_t m_vertexCountTotal;
            /// Total number of indices in the current division.
            uint32_t m_indexCountTotal;
            /// Unflushed number of vertices in the current division.
            uint32_t m_vertexCountPending;
            /// Unflushed number of indices in the current division.
            uint32_t m_indexCountPending;

            /// Buffer utility functions.
            Functions m_functions;

            /// @name Construction/Destruction
            //@{
            BufferData();
            //@}

            /// @name Buffer Management
            //@{
            bool Initialize();
            void Shutdown();

            void Map( Renderer* pRenderer, uint8_t*& rpMappedVertices, uint16_t*& rpMappedIndices );
            void FlushTriangles(
                DynamicDrawer* pDynamicDrawer, Renderer* pRenderer, RRenderCommandProxy* pCommandProxy,
                bool bAdvanceDivision );
            //@}
        };

        /// Untextured primitive buffer management utility functions.
        class UntexturedBufferFunctions
        {
        public:
            /// @name Buffer Management Functions
            //@{
            RVertexDescription* GetVertexDescription( DynamicDrawer* pDynamicDrawer ) const;
            RVertexShader* GetVertexShader( DynamicDrawer* pDynamicDrawer ) const;
            RPixelShader* GetPixelShader( DynamicDrawer* pDynamicDrawer ) const;

            void PrepareDraw(
                DynamicDrawer* pDynamicDrawer, RRenderCommandProxy* pCommandProxy,
                BufferData<
                    Vertex, UntexturedBufferFunctions, BUFFER_DIVISION_COUNT, BUFFER_DIVISION_VERTEX_COUNT,
                    BUFFER_DIVISION_INDEX_COUNT >* pBufferData ) const;
            //@}
        };

        /// Textured primitive buffer management utility functions.
        class TexturedBufferFunctions
        {
        public:
            /// @name Buffer Management Functions
            //@{
            RVertexDescription* GetVertexDescription( DynamicDrawer* pDynamicDrawer ) const;
            RVertexShader* GetVertexShader( DynamicDrawer* pDynamicDrawer ) const;
            RPixelShader* GetPixelShader( DynamicDrawer* pDynamicDrawer ) const;

            void PrepareDraw(
                DynamicDrawer* pDynamicDrawer, RRenderCommandProxy* pCommandProxy,
                BufferData<
                    VertexTextured, TexturedBufferFunctions, BUFFER_DIVISION_COUNT, BUFFER_DIVISION_VERTEX_COUNT,
                    BUFFER_DIVISION_INDEX_COUNT >* pBufferData ) const;
            //@}
        };

        /// Untextured triangle buffer.
        BufferData<
            Vertex, UntexturedBufferFunctions, BUFFER_DIVISION_COUNT, BUFFER_DIVISION_VERTEX_COUNT,
            BUFFER_DIVISION_INDEX_COUNT > m_untexturedTriangles;

        /// Textured triangle buffers.
        BufferData<
            VertexTextured, TexturedBufferFunctions, BUFFER_DIVISION_COUNT, BUFFER_DIVISION_VERTEX_COUNT,
            BUFFER_DIVISION_INDEX_COUNT > m_texturedTriangles[ TEXTURED_TRIANGLE_BUFFER_COUNT ];
        /// Textured triangle buffer textures.
        RTexture2dPtr m_texturedTriangleTextures[ TEXTURED_TRIANGLE_BUFFER_COUNT ];

        /// Untextured vertex description.
        RVertexDescriptionPtr m_spVertexDescription;
        /// Textured vertex description.
        RVertexDescriptionPtr m_spVertexTexturedDescription;
        /// Active vertex description.
        RVertexDescription* m_pActiveDescription;

        /// Vertex shader for untextured screen-space rendering.
        RVertexShaderPtr m_spUntexturedVertexShader;
        /// Pixel shader for untextured screen-space rendering.
        RPixelShaderPtr m_spUntexturedPixelShader;
        /// Vertex shader for textured screen-space rendering.
        RVertexShaderPtr m_spTexturedVertexShader;
        /// Pixel shader for textured screen-space rendering.
        RPixelShaderPtr m_spTexturedPixelShader;

        /// Singleton instance.
        static DynamicDrawer* sm_pInstance;

        /// @name Construction/Destruction
        //@{
        DynamicDrawer();
        ~DynamicDrawer();
        //@}

        /// @name Static Buffer Management
        //@{
        void FlushUntexturedTriangles( Renderer* pRenderer, RRenderCommandProxy* pCommandProxy, bool bAdvanceDivision );
        void FlushTexturedTriangles(
            Renderer* pRenderer, RRenderCommandProxy* pCommandProxy, size_t bufferSetIndex, bool bAdvanceDivision );
        //@}
    };
}

#include "Graphics/DynamicDrawer.inl"

#endif  // LUNAR_GRAPHICS_DYNAMIC_DRAWER_H
