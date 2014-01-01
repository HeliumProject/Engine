#pragma once

#include "RenderingGL/RenderingGL.h"
#include "RenderingGL/GLRasterizerState.h"
#include "RenderingGL/GLBlendState.h"
#include "Rendering/RRenderCommandProxy.h"

struct GLFWwindow;

namespace Helium
{
	HELIUM_DECLARE_RPTR( GLRasterizerState );
	HELIUM_DECLARE_RPTR( GLBlendState );

	/// Render command proxy for immediate issuing of rendering commands to the GPU command buffer.
	class GLImmediateCommandProxy : public RRenderCommandProxy
	{
	public:
		/// @name Construction/Destruction
		//@{
		GLImmediateCommandProxy( GLFWwindow* pGlfwWindow );
		//@}

		/// @name State Management
		//@{
		void SetRasterizerState( RRasterizerState* pState );
		void SetBlendState( RBlendState* pState );
		void SetDepthStencilState( RDepthStencilState* pState, uint8_t stencilReferenceValue );
		void SetSamplerStates( size_t startIndex, size_t samplerCount, RSamplerState* const* ppStates );
		//@}

		/// @name Render Target Management
		//@{
		void SetRenderSurfaces( RSurface* pRenderTargetSurface, RSurface* pDepthStencilSurface );
		void SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height );
		//@}

		/// @name Command Generation
		//@{
		void BeginScene();
		void EndScene();

		void Clear( uint32_t clearFlags, const Color& rColor, float32_t depth, uint8_t stencil );

		void SetIndexBuffer( RIndexBuffer* pBuffer );
		void SetVertexBuffers(
			size_t startIndex, size_t bufferCount, RVertexBuffer* const* ppBuffers, uint32_t* pStrides,
			uint32_t* pOffsets );
		void SetVertexInputLayout( RVertexInputLayout* pLayout );

		void SetVertexShader( RVertexShader* pShader );
		void SetPixelShader( RPixelShader* pShader );

		void SetVertexConstantBuffers(
			size_t startIndex, size_t bufferCount, RConstantBuffer* const* ppBuffers,
			const size_t* pLimitSizes = NULL );
		void SetPixelConstantBuffers(
			size_t startIndex, size_t bufferCount, RConstantBuffer* const* ppBuffers,
			const size_t* pLimitSizes = NULL );

		void SetTexture( size_t samplerIndex, RTexture* pTexture );

		void DrawIndexed(
			ERendererPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t minIndex, uint32_t usedVertexCount,
			uint32_t startIndex, uint32_t primitiveCount );
		void DrawUnindexed( ERendererPrimitiveType primitiveType, uint32_t baseVertexIndex, uint32_t primitiveCount );
		//@}

		/// @name Fence Commands
		//@{
		void SetFence( RFence* pFence );
		//@}

		/// @name Miscellaneous Resource Management
		//@{
		void UnbindResources();
		//@}

		/// @name Command List Support
		//@{
		void ExecuteCommandList( RRenderCommandList* pCommandList );
		void FinishCommandList( RRenderCommandListPtr& rspCommandList );
		//@}

	private:
		/// GLFW window / OpenGL context
		GLFWwindow *m_pGlfwWindow;

		/// @name Construction/Destruction
		//@{
		~GLImmediateCommandProxy();
		//@}
	};
}
