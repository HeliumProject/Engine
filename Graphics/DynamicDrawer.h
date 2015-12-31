#pragma once

#include "Graphics/Graphics.h"

#include "Rendering/RRenderResource.h"
#include "GraphicsTypes/VertexTypes.h"

namespace Helium
{
	class Renderer;
	class RRenderCommandProxy;

	class RenderResourceManager;

	HELIUM_DECLARE_RPTR( RFence );
	HELIUM_DECLARE_RPTR( RIndexBuffer );
	HELIUM_DECLARE_RPTR( RPixelShader );
	HELIUM_DECLARE_RPTR( RTexture2d );
	HELIUM_DECLARE_RPTR( RVertexBuffer );
	HELIUM_DECLARE_RPTR( RVertexDescription );
	HELIUM_DECLARE_RPTR( RVertexShader );

	/// Dynamic drawing interface.
	class HELIUM_GRAPHICS_API DynamicDrawer : NonCopyable
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

		/// @name Initialization
		//@{
		bool Initialize();
		void Cleanup();
		//@}

		/// @name Drawing
		//@{
		void Begin();

		void DrawScreenSpaceQuad(
			const SimpleVertex& rVertex0, const SimpleVertex& rVertex1, const SimpleVertex& rVertex2,
			const SimpleVertex& rVertex3, bool bFlush = false );
		void DrawScreenSpaceQuad(
			const SimpleTexturedVertex& rVertex0, const SimpleTexturedVertex& rVertex1,
			const SimpleTexturedVertex& rVertex2, const SimpleTexturedVertex& rVertex3, RTexture2d* pTexture,
			bool bFlush = false );

		void Flush();
		//@}

		/// @name Static Access
		//@{
		static DynamicDrawer* GetInstance();
		static void Startup();
		static void Shutdown();
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
			RFencePtr m_divisionFences[DivisionCount];

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
			void Cleanup();

			void Map( Renderer* pRenderer, uint8_t*& rpMappedVertices, uint16_t*& rpMappedIndices );
			void FlushTriangles(
				DynamicDrawer* pDynamicDrawer, RenderResourceManager* pRenderResourceManager, Renderer* pRenderer,
				RRenderCommandProxy* pCommandProxy, bool bAdvanceDivision );
			//@}
		};

		/// Untextured primitive buffer management utility functions.
		class UntexturedBufferFunctions
		{
		public:
			/// @name Buffer Management Functions
			//@{
			RVertexDescription* GetVertexDescription( RenderResourceManager* pRenderResourceManager ) const;
			RVertexShader* GetVertexShader( DynamicDrawer* pDynamicDrawer ) const;
			RPixelShader* GetPixelShader( DynamicDrawer* pDynamicDrawer ) const;

			void PrepareDraw(
				DynamicDrawer* pDynamicDrawer, RRenderCommandProxy* pCommandProxy,
				BufferData< SimpleVertex, UntexturedBufferFunctions, BUFFER_DIVISION_COUNT, BUFFER_DIVISION_VERTEX_COUNT, BUFFER_DIVISION_INDEX_COUNT >* pBufferData ) const;
			//@}
		};

		/// Textured primitive buffer management utility functions.
		class TexturedBufferFunctions
		{
		public:
			/// @name Buffer Management Functions
			//@{
			RVertexDescription* GetVertexDescription( RenderResourceManager* pRenderResourceManager ) const;
			RVertexShader* GetVertexShader( DynamicDrawer* pDynamicDrawer ) const;
			RPixelShader* GetPixelShader( DynamicDrawer* pDynamicDrawer ) const;

			void PrepareDraw(
				DynamicDrawer* pDynamicDrawer, RRenderCommandProxy* pCommandProxy,
				BufferData< SimpleTexturedVertex, TexturedBufferFunctions, BUFFER_DIVISION_COUNT, BUFFER_DIVISION_VERTEX_COUNT, BUFFER_DIVISION_INDEX_COUNT >* pBufferData ) const;
			//@}
		};

		/// Untextured triangle buffer.
		BufferData< SimpleVertex, UntexturedBufferFunctions, BUFFER_DIVISION_COUNT, BUFFER_DIVISION_VERTEX_COUNT, BUFFER_DIVISION_INDEX_COUNT > m_untexturedTriangles;
		/// Textured triangle buffers.
		BufferData< SimpleTexturedVertex, TexturedBufferFunctions, BUFFER_DIVISION_COUNT, BUFFER_DIVISION_VERTEX_COUNT, BUFFER_DIVISION_INDEX_COUNT > m_texturedTriangles[TEXTURED_TRIANGLE_BUFFER_COUNT];
		/// Textured triangle buffer textures.
		RTexture2dPtr m_texturedTriangleTextures[TEXTURED_TRIANGLE_BUFFER_COUNT];

		/// Active vertex description.
		RVertexDescription* m_pActiveDescription;

		/// Vertex shader for untextured screen-space rendering.
		RVertexShaderPtr m_spUntexturedScreenVertexShader;
		/// Pixel shader for untextured screen-space rendering.
		RPixelShaderPtr m_spUntexturedScreenPixelShader;
		/// Vertex shader for textured screen-space rendering.
		RVertexShaderPtr m_spTexturedScreenVertexShader;
		/// Pixel shader for textured screen-space rendering.
		RPixelShaderPtr m_spTexturedScreenPixelShader;

		/// Singleton instance.
		static DynamicDrawer* sm_pInstance;

		/// @name Construction/Destruction
		//@{
		DynamicDrawer();
		~DynamicDrawer();
		//@}

		/// @name Static Buffer Management
		//@{
		void FlushUntexturedTriangles(
			RenderResourceManager* pRenderResourceManager, Renderer* pRenderer, RRenderCommandProxy* pCommandProxy,
			bool bAdvanceDivision );
		void FlushTexturedTriangles(
			RenderResourceManager* pRenderResourceManager, Renderer* pRenderer, RRenderCommandProxy* pCommandProxy,
			size_t bufferSetIndex, bool bAdvanceDivision );
		//@}
	};
}
