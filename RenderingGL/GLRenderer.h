#pragma once

#include "RenderingGL/RenderingGL.h"
#include "Rendering/Renderer.h"

#include "GL/glew.h"

struct GLFWwindow;

namespace Helium
{
	HELIUM_DECLARE_RPTR( GLImmediateCommandProxy );
	HELIUM_DECLARE_RPTR( GLMainContext );

	/// OpenGL Renderer Implementation
	class GLRenderer : public Renderer
	{
	public:
		/// @name Initialization
		//@{
		bool Initialize();
		void Shutdown();
		//@}

		/// @name Display Initialization
		//@{
		bool CreateMainContext( const ContextInitParameters& rInitParameters );
		bool ResetMainContext( const ContextInitParameters& rInitParameters );
		RRenderContext* GetMainContext();

		RRenderContext* CreateSubContext( const ContextInitParameters& rInitParameters );

		EStatus GetStatus();
		EStatus Reset();
		//@}

		/// @name State Object Creation
		//@{
		RRasterizerState* CreateRasterizerState( const RRasterizerState::Description& rDescription );
		RBlendState* CreateBlendState( const RBlendState::Description& rDescription );
		RDepthStencilState* CreateDepthStencilState( const RDepthStencilState::Description& rDescription );
		RSamplerState* CreateSamplerState( const RSamplerState::Description& rDescription );
		//@}

		/// @name Resource Allocation
		//@{
		RSurface* CreateDepthStencilSurface(
			uint32_t width, uint32_t height, ERendererSurfaceFormat format, uint32_t multisampleCount );

		RVertexShader* CreateVertexShader( size_t size, const void* pData );
		RPixelShader* CreatePixelShader( size_t size, const void* pData );

		RVertexBuffer* CreateVertexBuffer( size_t size, ERendererBufferUsage usage, const void* pData );
		RIndexBuffer* CreateIndexBuffer(
			size_t size, ERendererBufferUsage usage, ERendererIndexFormat format, const void* pData );
		RConstantBuffer* CreateConstantBuffer( size_t size, ERendererBufferUsage usage, const void* pData );

		RVertexDescription* CreateVertexDescription( const RVertexDescription::Element* pElements, size_t elementCount );
		RVertexInputLayout* CreateVertexInputLayout( RVertexDescription* pDescription, RVertexShader* pShader );

		RTexture2d* CreateTexture2d(
			uint32_t width, uint32_t height, uint32_t mipCount, ERendererPixelFormat format, ERendererBufferUsage usage,
			const RTexture2d::CreateData* pData );
		//@}

		/// @name Deferred Query Allocation
		//@{
		RFence* CreateFence();
		void SyncFence( RFence* pFence );
		bool TrySyncFence( RFence* pFence );
		//@}

		/// @name Command Interfaces
		//@{
		RRenderCommandProxy* GetImmediateCommandProxy();
		RRenderCommandProxy* CreateDeferredCommandProxy();

		void Flush();
		//@}

		/// @name Utility Functions
		//@{
		GLenum PixelFormatToGLFormat( ERendererPixelFormat format ) const;
		//@}

		/// @name Static Initialization
		//@{
		HELIUM_RENDERING_GL_API static bool CreateStaticInstance();
		//@}

	private:
		/// GLFW window / OpenGL context
		GLFWwindow *m_pGlfwWindow;

		/// Immediate render command proxy.
		GLImmediateCommandProxyPtr m_spImmediateCommandProxy;
		/// Main rendering context.
		GLMainContextPtr m_spMainContext;

		/// Depth buffer format
		GLenum m_depthTextureFormat;

		/// S3TC availability.
		bool m_bHasS3tcExt;
		/// sRGB availability.
		bool m_bHasSRGBExt;
		/// Anisotropic filtering availability.
		bool m_bHasAnisotropicExt;

		/// @name Construction/Destruction
		//@{
		GLRenderer();
		virtual ~GLRenderer();
		//@}
	};
}

#include "RenderingGL/GLRenderer.inl"
