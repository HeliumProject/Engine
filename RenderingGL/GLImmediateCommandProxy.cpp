#include "Precompile.h"
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
	GLRasterizerState *pGLState = static_cast< GLRasterizerState* >( pState );
	HELIUM_ASSERT( pGLState != NULL );

	// TODO: don't make redundant state changes.

	glPolygonMode( GL_FRONT_AND_BACK, pGLState->m_fillMode );

	if( pGLState->m_cullEnable )
	{
		glEnable( GL_CULL_FACE );
	}
	else
	{
		glDisable( GL_CULL_FACE );
	}

	glCullFace( pGLState->m_cullMode );

	glFrontFace( pGLState->m_winding );

	glDisable( GL_POLYGON_OFFSET_LINE );
	glDisable( GL_POLYGON_OFFSET_FILL );
	if( pGLState->m_depthBiasEnable )
	{
		glEnable( pGLState->m_depthBiasMode );
		glPolygonOffset( pGLState->m_slopeScaledDepthBias, pGLState->m_depthBias );
	}
}

/// @copydoc RRenderCommandProxy::SetBlendState()
void GLImmediateCommandProxy::SetBlendState( RBlendState* pState )
{
	GLBlendState *pGLState = static_cast< GLBlendState* >( pState );
	HELIUM_ASSERT( pGLState != NULL );

	// TODO: don't make redundant state changes.

	glColorMask(
		pGLState->m_redWriteMaskEnable,
		pGLState->m_greenWriteMaskEnable,
		pGLState->m_blueWriteMaskEnable,
		pGLState->m_alphaWriteMaskEnable );

	if( pGLState->m_blendEnable )
	{
		glEnable( GL_BLEND );
	}
	else
	{
		glDisable( GL_BLEND );
	}

	glBlendEquation( pGLState->m_function );

	glBlendFunc( pGLState->m_sourceFactor, pGLState->m_destinationFactor );
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
	GLint maxActiveTextures = 0;
	glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxActiveTextures );

	HELIUM_ASSERT( startIndex < maxActiveTextures );
	if( startIndex >= maxActiveTextures )
	{
		HELIUM_TRACE( TraceLevels::Warning, "GLImmediateCommandProxy: Maximum number of active textures exceeded.\n" );
		if( startIndex < maxActiveTextures )
		{
			// Clamp the number of texture units that we configure.
			samplerCount = maxActiveTextures - startIndex;
		}
		else
		{
			// Don't even bother, no valid texture units were provided.
			return;
		}
	}

	for( size_t i = 0; i < samplerCount; ++i )
	{
		GLSamplerState *pGLState = static_cast< GLSamplerState* >( ppStates[ i ] );
		HELIUM_ASSERT( pGLState != NULL );

		// TODO: don't make redundant state changes.
		
		const GLenum activeTexture = GL_TEXTURE0 + static_cast< GLenum >( i );
		glActiveTexture( activeTexture );

		glTexParameteri( pGLState->m_texParameterTarget, GL_TEXTURE_MIN_FILTER, pGLState->m_minFilter );
		glTexParameteri( pGLState->m_texParameterTarget, GL_TEXTURE_MAG_FILTER, pGLState->m_magFilter );
		
		glTexParameterf( pGLState->m_texParameterTarget, GL_TEXTURE_LOD_BIAS, pGLState->m_mipLodBias );

		glTexParameterf( pGLState->m_texParameterTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, pGLState->m_maxAnisotropy );

		glTexParameteri( pGLState->m_texParameterTarget, GL_TEXTURE_WRAP_S, pGLState->m_addressModeU );
		glTexParameteri( pGLState->m_texParameterTarget, GL_TEXTURE_WRAP_T, pGLState->m_addressModeV );
		glTexParameteri( pGLState->m_texParameterTarget, GL_TEXTURE_WRAP_R, pGLState->m_addressModeW );
	}
}

/// @copydoc RRenderCommandProxy::SetRenderSurfaces()
void GLImmediateCommandProxy::SetRenderSurfaces( RSurface* pRenderTargetSurface, RSurface* pDepthStencilSurface )
{
	GLSurface *pGLRenderTargetSurface = static_cast< GLSurface* >( pRenderTargetSurface );
	HELIUM_ASSERT( pGLRenderTargetSurface );

	GLuint colorTarget = pGLRenderTargetSurface->GetGLSurface();
	GLenum colorAttachment = pGLRenderTargetSurface->GetGLAttachmentType();
	bool colorIsTexture = pGLRenderTargetSurface->GetIsTexture();
	HELIUM_ASSERT( colorTarget != 0 );
	if( colorIsTexture )
	{
		glFramebufferTexture2D( GL_FRAMEBUFFER, colorAttachment, GL_TEXTURE_2D, colorTarget, 0 );
	}
	else
	{
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, colorAttachment, GL_RENDERBUFFER, colorTarget );
	}

	GLuint depthStencilTarget = pGLRenderTargetSurface->GetGLSurface();
	GLenum depthStencilAttachment = pGLRenderTargetSurface->GetGLAttachmentType();
	bool depthStencilIsTexture = pGLRenderTargetSurface->GetIsTexture();
	HELIUM_ASSERT( depthStencilTarget != 0 );
	if( depthStencilIsTexture )
	{
		glFramebufferTexture2D( GL_FRAMEBUFFER, depthStencilAttachment, GL_TEXTURE_2D, depthStencilTarget, 0 );
	}
	else
	{
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, depthStencilAttachment, GL_RENDERBUFFER, depthStencilTarget );
	}

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
	glViewport( x, y, width, height );
}

/// @copydoc RRenderCommandProxy::BeginScene()
void GLImmediateCommandProxy::BeginScene()
{
	// TODO: GLFW Set context active here?
}

/// @copydoc RRenderCommandProxy::EndScene()
void GLImmediateCommandProxy::EndScene()
{
	// TODO: GLFW swap buffers here?
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
	// TODO: Implement later. HELIUM_BREAK();
}

/// @copydoc RRenderCommandProxy::SetPixelConstantBuffers()
void GLImmediateCommandProxy::SetPixelConstantBuffers(
	size_t startIndex,
	size_t bufferCount,
	RConstantBuffer* const* ppBuffers,
	const size_t* pLimitSizes )
{
	// TODO: Implement later. HELIUM_BREAK();
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
	// TODO: Implement later.  HELIUM_BREAK();
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
