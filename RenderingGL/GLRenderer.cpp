#include "RenderingGLPch.h"
#include "RenderingGL/GLRenderer.h"

#include "RenderingGL/GLImmediateCommandProxy.h"
#include "RenderingGL/GLMainContext.h"
#include "RenderingGL/GLVertexBuffer.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

using namespace Helium;

/// Constructor.
GLRenderer::GLRenderer()
: m_pGlfwWindow(NULL)
{
}

/// Destructor.
GLRenderer::~GLRenderer()
{
}

/// @copydoc Renderer::Initialize()
bool GLRenderer::Initialize()
{
	HELIUM_TRACE( TraceLevels::Info, "Initializing OpenGL rendering support.\n" );

	m_featureFlags = 0;

	// TODO: Select best depth texture format and enable depth texture feature flags.

	HELIUM_TRACE( TraceLevels::Info, "OpenGL initialized successfully.\n" );

	return true;
}

/// @copydoc Renderer::Shutdown()
void GLRenderer::Shutdown()
{
	HELIUM_TRACE( TraceLevels::Info, TXT( "Shutting down OpenGL rendering support.\n" ) );

	m_spMainContext.Release();
	m_spImmediateCommandProxy.Release();

	m_featureFlags = 0;

	HELIUM_TRACE( TraceLevels::Info, TXT( "OpenGL renderer shutdown complete.\n" ) );
}


/// @copydoc Renderer::CreateMainContext()
bool GLRenderer::CreateMainContext( const ContextInitParameters& rInitParameters )
{
	m_pGlfwWindow = static_cast<GLFWwindow*>( rInitParameters.pWindow );
	HELIUM_ASSERT( m_pGlfwWindow );

	// Create the immediate render command proxy interface.
	m_spImmediateCommandProxy = new GLImmediateCommandProxy( m_pGlfwWindow );
	HELIUM_ASSERT( m_spImmediateCommandProxy );

	// Create the main rendering context interface.
	m_spMainContext = new GLMainContext( m_pGlfwWindow );
	HELIUM_ASSERT( m_spMainContext );
	m_spMainContext->Initialize();

	return true;
}

/// @copydoc Renderer::ResetMainContext()
bool GLRenderer::ResetMainContext( const ContextInitParameters& rInitParameters )
{
	HELIUM_TRACE( TraceLevels::Info, "GLRenderer: Resetting main display context; this has no effect for a GL renderer.\n" );

	return true;
}

/// @copydoc Renderer::GetMainContext()
RRenderContext* GLRenderer::GetMainContext()
{
	return m_spMainContext;
}

/// @copydoc Renderer::CreateSubContext()
RRenderContext* GLRenderer::CreateSubContext( const ContextInitParameters& rInitParameters )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::GetStatus()
Renderer::EStatus GLRenderer::GetStatus()
{
	HELIUM_BREAK();

	return STATUS_INVALID;
}

/// @copydoc Renderer::Reset()
Renderer::EStatus GLRenderer::Reset()
{
	HELIUM_BREAK();

	return STATUS_INVALID;
}

/// @copydoc Renderer::CreateRasterizerState()
RRasterizerState* GLRenderer::CreateRasterizerState( const RRasterizerState::Description& rDescription )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::CreateBlendState()
RBlendState* GLRenderer::CreateBlendState( const RBlendState::Description& rDescription )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::CreateDepthStencilState()
RDepthStencilState* GLRenderer::CreateDepthStencilState( const RDepthStencilState::Description& rDescription )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::CreateSamplerState()
RSamplerState* GLRenderer::CreateSamplerState( const RSamplerState::Description& rDescription )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::CreateDepthStencilSurface()
RSurface* GLRenderer::CreateDepthStencilSurface(
	uint32_t width,
	uint32_t height,
	ERendererSurfaceFormat format,
	uint32_t multisampleCount )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::CreateVertexShader()
RVertexShader* GLRenderer::CreateVertexShader( size_t size, const void* pData )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::CreatePixelShader()
RPixelShader* GLRenderer::CreatePixelShader( size_t size, const void* pData )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::CreateVertexBuffer()
RVertexBuffer* GLRenderer::CreateVertexBuffer( size_t size, ERendererBufferUsage usage, const void* pData )
{
	// Verify the size is valid for valid truncation due to casting to a UINT for the parameters passed to
	// CreateVertexBuffer().
	HELIUM_ASSERT( size <= UINT32_MAX );

	// Vertex and index buffers can only be created with static or dynamic usage semantics.
	HELIUM_ASSERT( usage == RENDERER_BUFFER_USAGE_STATIC || usage == RENDERER_BUFFER_USAGE_DYNAMIC );
	if( usage != RENDERER_BUFFER_USAGE_STATIC && usage != RENDERER_BUFFER_USAGE_DYNAMIC )
	{
		HELIUM_TRACE(TraceLevels::Error,
			"GLRenderer::CreateVertexBuffer(): Vertex buffers can only be created with static or dynamic usage semantics.\n" );
		return NULL;
	}

	// Determine buffer usage.
	const bool bDynamic = ( usage == RENDERER_BUFFER_USAGE_DYNAMIC );
	const GLenum usageGl = bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	// Create vertex buffer object.
	unsigned vbo;
	glBindVertexArray( 0 );
	glGenBuffers( 1, &vbo );

	// Optionally copy provided vertex data into the buffer.  Note
	// that if pData is NULL, the buffer will be allocated but contents undefined.
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, size, pData, usageGl );
	const bool bValidData = (pData != NULL);

	// Create Helium GL vertex buffer object.
	GLVertexBuffer *vertexBuffer = new GLVertexBuffer(vbo);
	if( !vertexBuffer )
	{
		HELIUM_TRACE(TraceLevels::Error,
			"GLRenderer::CreateVertexBuffer(): Failed to create GLVertexBuffer instance.\n" );
		return NULL;
	}

	return vertexBuffer;
}

/// @copydoc Renderer::CreateIndexBuffer()
RIndexBuffer* GLRenderer::CreateIndexBuffer(
	size_t size,
	ERendererBufferUsage usage,
	ERendererIndexFormat format,
	const void* pData )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::CreateConstantBuffer()
RConstantBuffer* GLRenderer::CreateConstantBuffer(
	size_t size,
	ERendererBufferUsage /*usage*/,
	const void* pData )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::CreateVertexDescription()
RVertexDescription* GLRenderer::CreateVertexDescription(
	const RVertexDescription::Element* pElements,
	size_t elementCount )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::CreateVertexInputLayout()
RVertexInputLayout* GLRenderer::CreateVertexInputLayout(
	RVertexDescription* pDescription,
	RVertexShader* /*pShader*/ )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::CreateTexture2d()
RTexture2d* GLRenderer::CreateTexture2d(
	uint32_t width,
	uint32_t height,
	uint32_t mipCount,
	ERendererPixelFormat format,
	ERendererBufferUsage usage,
	const RTexture2d::CreateData* pData )
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::CreateFence()
RFence* GLRenderer::CreateFence()
{
	HELIUM_BREAK();

	return NULL;
}

/// @copydoc Renderer::SyncFence()
void GLRenderer::SyncFence( RFence* pFence )
{
	HELIUM_BREAK();
}

/// @copydoc Renderer::TrySyncFence()
bool GLRenderer::TrySyncFence( RFence* pFence )
{
	HELIUM_BREAK();

	return false;
}

/// @copydoc Renderer::GetImmediateCommandProxy()
RRenderCommandProxy* GLRenderer::GetImmediateCommandProxy()
{
	return m_spImmediateCommandProxy;
}

/// @copydoc Renderer::CreateDeferredCommandProxy()
RRenderCommandProxy* GLRenderer::CreateDeferredCommandProxy()
{
	HELIUM_BREAK();

	return false;
}

/// @copydoc Renderer::Flush()
void GLRenderer::Flush()
{
	HELIUM_BREAK();
}

/// Create the static renderer instance.
///
/// @return  True if the renderer was created successfully, false if not or another renderer instance already
///          exists.
bool GLRenderer::CreateStaticInstance()
{
    if( sm_pInstance )
    {
        return false;
    }

    sm_pInstance = new GLRenderer;
    HELIUM_ASSERT( sm_pInstance );

    return ( sm_pInstance != NULL );
}
