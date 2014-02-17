#include "RenderingGLPch.h"
#include "RenderingGL/GLImmediateCommandProxy.h"

#include "RenderingGL/GLSurface.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

using namespace Helium;

/// Constructor.
GLImmediateCommandProxy::GLImmediateCommandProxy( GLFWwindow* pGlfwWindow )
: m_pGlfwWindow( pGlfwWindow )
{
    HELIUM_ASSERT( pGlfwWindow );
}

/// Destructor.
GLImmediateCommandProxy::~GLImmediateCommandProxy()
{
	m_pGlfwWindow = NULL;
}

/// @copydoc RRenderCommandProxy::SetRasterizerState()
void GLImmediateCommandProxy::SetRasterizerState( RRasterizerState* pState )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::SetBlendState()
void GLImmediateCommandProxy::SetBlendState( RBlendState* pState )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::SetDepthStencilState()
void GLImmediateCommandProxy::SetDepthStencilState( RDepthStencilState* pState, uint8_t stencilReferenceValue )
{
	GLDepthStencilState *pGLState = static_cast< GLDepthStencilState* >( pState );
	HELIUM_ASSERT( pGLState != NULL );

	// TODO: don't make redundant state changes.

	if( pGLState->m_depthTestEnable )
	{
		glEnable( GL_DEPTH_TEST );
	}
	else
	{
		glDisable( GL_DEPTH_TEST );
	}

	if( pGLState->m_depthWriteEnable )
	{
		glDepthMask( GL_TRUE );
	}
	else
	{
		glDepthMask( GL_FALSE );
	}

	glDepthFunc( pGLState->m_depthFunction );

	if( pGLState->m_stencilTestEnable )
	{
		glEnable( GL_STENCIL_TEST );
	}
	else
	{
		glDisable( GL_STENCIL_TEST );
	}

	glStencilFunc(
		pGLState->m_stencilFunction, stencilReferenceValue, pGLState->m_stencilReadMask );
	
	glStencilOp(
		pGLState->m_stencilFailOperation, pGLState->m_stencilDepthFailOperation, pGLState->m_stencilDepthPassOperation );

	glStencilMask( pGLState->m_stencilWriteMask );
}

/// @copydoc RRenderCommandProxy::SetSamplerStates()
void GLImmediateCommandProxy::SetSamplerStates(
	size_t startIndex,
	size_t samplerCount,
	RSamplerState* const* ppStates )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::SetRenderSurfaces()
void GLImmediateCommandProxy::SetRenderSurfaces( RSurface* pRenderTargetSurface, RSurface* pDepthStencilSurface )
{
	HELIUM_ASSERT( pRenderTargetSurface );

	GLuint colorRenderbuffer = static_cast< GLSurface* >( pRenderTargetSurface )->GetGLSurface();
	GLenum colorAttachment = static_cast< GLSurface* >( pRenderTargetSurface )->GetGLAttachmentType();
	HELIUM_ASSERT( colorRenderbuffer != 0 );
	glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, colorAttachment, GL_RENDERBUFFER, colorRenderbuffer );

	GLuint depthStencilRenderbuffer = static_cast< GLSurface* >( pDepthStencilSurface )->GetGLSurface();
	GLenum depthStencilAttachment = static_cast< GLSurface* >( pDepthStencilSurface )->GetGLAttachmentType();
	HELIUM_ASSERT( depthStencilRenderbuffer != 0 );
	glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, depthStencilAttachment, GL_RENDERBUFFER, depthStencilRenderbuffer );

	GLenum framebufferStatus = glCheckFramebufferStatus( GL_DRAW_FRAMEBUFFER );
	HELIUM_ASSERT( framebufferStatus == GL_FRAMEBUFFER_COMPLETE );
	if( framebufferStatus != GL_FRAMEBUFFER_COMPLETE )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLImmediateCommandProxy: Incomplete framebuffer object created." );
	}
}

/// @copydoc RRenderCommandProxy::SetViewport()
void GLImmediateCommandProxy::SetViewport( uint32_t x, uint32_t y, uint32_t width, uint32_t height )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::BeginScene()
void GLImmediateCommandProxy::BeginScene()
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::EndScene()
void GLImmediateCommandProxy::EndScene()
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::Clear()
void GLImmediateCommandProxy::Clear( uint32_t clearFlags, const Color& rColor, float32_t depth, uint8_t stencil )
{
	GLbitfield glClearFlags = 0;
	if( clearFlags & RENDERER_CLEAR_FLAG_TARGET )
	{
		glClearFlags |= GL_COLOR_BUFFER_BIT;
		glClearColor(
			(GLclampf)rColor.GetFloatR(),
			(GLclampf)rColor.GetFloatG(),
			(GLclampf)rColor.GetFloatB(),
			(GLclampf)rColor.GetFloatA() );
	}
	if( clearFlags & RENDERER_CLEAR_FLAG_DEPTH )
	{
		glClearFlags |= GL_DEPTH_BUFFER_BIT;
		glClearDepth( (GLclampd)depth );
	}
	if( clearFlags & RENDERER_CLEAR_FLAG_STENCIL )
	{
		glClearFlags |= GL_STENCIL_BUFFER_BIT;
		glClearStencil( (GLint)stencil );
	}

	glClear( glClearFlags );
}

/// @copydoc RRenderCommandProxy::SetIndexBuffer()
void GLImmediateCommandProxy::SetIndexBuffer( RIndexBuffer* pBuffer )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::SetVertexBuffers()
void GLImmediateCommandProxy::SetVertexBuffers(
	size_t startIndex,
	size_t bufferCount,
	RVertexBuffer* const* ppBuffers,
	uint32_t* pStrides,
	uint32_t* pOffsets )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::SetVertexInputLayout()
void GLImmediateCommandProxy::SetVertexInputLayout( RVertexInputLayout* pLayout )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::SetVertexShader()
void GLImmediateCommandProxy::SetVertexShader( RVertexShader* pShader )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::SetPixelShader()
void GLImmediateCommandProxy::SetPixelShader( RPixelShader* pShader )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::SetVertexConstantBuffers()
void GLImmediateCommandProxy::SetVertexConstantBuffers(
	size_t startIndex,
	size_t bufferCount,
	RConstantBuffer* const* ppBuffers,
	const size_t* pLimitSizes )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::SetPixelConstantBuffers()
void GLImmediateCommandProxy::SetPixelConstantBuffers(
	size_t startIndex,
	size_t bufferCount,
	RConstantBuffer* const* ppBuffers,
	const size_t* pLimitSizes )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::SetTexture()
void GLImmediateCommandProxy::SetTexture( size_t samplerIndex, RTexture* pTexture )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::DrawIndexed()
void GLImmediateCommandProxy::DrawIndexed(
	ERendererPrimitiveType primitiveType,
	uint32_t baseVertexIndex,
	uint32_t minIndex,
	uint32_t usedVertexCount,
	uint32_t startIndex,
	uint32_t primitiveCount )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::DrawUnindexed()
void GLImmediateCommandProxy::DrawUnindexed(
	ERendererPrimitiveType primitiveType,
	uint32_t baseVertexIndex,
	uint32_t primitiveCount )
{
	HELIUM_BREAK();}

/// @copydoc RRenderCommandProxy::SetFence()
void GLImmediateCommandProxy::SetFence( RFence* pFence )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::UnbindResources()
void GLImmediateCommandProxy::UnbindResources()
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::ExecuteCommandList()
void GLImmediateCommandProxy::ExecuteCommandList( RRenderCommandList* pCommandList )
{
	HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::FinishCommandList()
void GLImmediateCommandProxy::FinishCommandList( RRenderCommandListPtr& rspCommandList )
{
	HELIUM_BREAK();
}
