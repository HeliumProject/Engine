#include "RenderingGLPch.h"
#include "RenderingGL/GLRenderer.h"

#include "RenderingGL/GLImmediateCommandProxy.h"
#include "RenderingGL/GLMainContext.h"
#include "RenderingGL/GLRasterizerState.h"
#include "RenderingGL/GLBlendState.h"
#include "RenderingGL/GLDepthStencilState.h"
#include "RenderingGL/GLVertexBuffer.h"
#include "RenderingGL/GLIndexBuffer.h"
#include "RenderingGL/GLConstantBuffer.h"
#include "RenderingGL/GLVertexDescription.h"
#include "RenderingGL/GLTexture2d.h"
#include "RenderingGL/GLSurface.h"

#include "Rendering/RendererUtil.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

using namespace Helium;

/// Get the OpenGL format identifier for the specified pixel format.
///
/// @param[in]  format  Pixel format.
/// @param[out] internalFormat  Texture object format.
/// @param[out] elementType  Element data type for client texture data.
/// @param[out] isCompressed  True if this is a compressed texture format.
void GLRenderer::PixelFormatToGLFormat(
	ERendererPixelFormat format, GLenum &internalFormat, GLenum &pixelFormat, GLenum &elementType ) const
{
	// Convert the format to the corresponding OpenGL format.
	static const GLenum glFormats[ RENDERER_PIXEL_FORMAT_MAX ][ 3 ] =
	{
		// { INTERNAL_FORMAT,                     PIXEL_FORMAT, ELEMENT_TYPE }
		{ GL_RGBA8,                               GL_RGBA, GL_UNSIGNED_BYTE }, // RENDERER_PIXEL_FORMAT_R8G8B8A8
		{ GL_SRGB8_ALPHA8,                        GL_RGBA, GL_UNSIGNED_BYTE }, // RENDERER_PIXEL_FORMAT_R8G8B8A8_SRGB
		{ GL_R8,                                  GL_RED,  GL_UNSIGNED_BYTE }, // RENDERER_PIXEL_FORMAT_R8
		{ GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,       GL_RGBA, GL_UNSIGNED_BYTE }, // RENDERER_PIXEL_FORMAT_BC1
		{ GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_RGBA, GL_UNSIGNED_BYTE }, // RENDERER_PIXEL_FORMAT_BC1_SRGB
		{ GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,       GL_RGBA, GL_UNSIGNED_BYTE }, // RENDERER_PIXEL_FORMAT_BC2
		{ GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, GL_RGBA, GL_UNSIGNED_BYTE }, // RENDERER_PIXEL_FORMAT_BC2_SRGB
		{ GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,       GL_RGBA, GL_UNSIGNED_BYTE }, // RENDERER_PIXEL_FORMAT_BC3
		{ GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_RGBA, GL_UNSIGNED_BYTE }, // RENDERER_PIXEL_FORMAT_BC3_SRGB
		{ GL_RGBA16F,                             GL_RGBA, GL_HALF_FLOAT    }, // RENDERER_PIXEL_FORMAT_R16G16B16A16_FLOAT
		{ GL_NONE,                                GL_NONE, GL_NONE          }  // RENDERER_PIXEL_FORMAT_DEPTH (dummy entry; depth formats handled manually)
	};

	// Handle depth formats manually.
	if( format == RENDERER_PIXEL_FORMAT_DEPTH )
	{
		internalFormat = m_depthTextureFormat;
		pixelFormat = GL_DEPTH_COMPONENT;
		switch( m_depthTextureFormat )
		{
		case GL_DEPTH_COMPONENT16:
			elementType = GL_UNSIGNED_INT;
			break;
		case GL_DEPTH_COMPONENT24:
			elementType = GL_UNSIGNED_BYTE;
			break;
		case GL_DEPTH_COMPONENT32:
			elementType = GL_UNSIGNED_INT;
			break;
		case GL_DEPTH_COMPONENT32F:
			elementType = GL_FLOAT;
			break;
		default:
			elementType = GL_NONE;
			break;
		}
	}
	else
	{
		HELIUM_ASSERT( static_cast< size_t >( format ) < static_cast< size_t >( RENDERER_PIXEL_FORMAT_MAX ) );
		internalFormat = glFormats[ format ][ 0 ];
		pixelFormat = glFormats[ format ][ 1 ];
		elementType = glFormats[ format ][ 2 ];
	}

	HELIUM_ASSERT( internalFormat != GL_NONE && pixelFormat != GL_NONE && elementType != GL_NONE );
	if( internalFormat == GL_NONE || pixelFormat == GL_NONE || elementType == GL_NONE )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLRenderer::PixelFormatToGLType(): Unknown pixel format encountered" );
	}
}

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

	// Set our depth texture format to 24 bits.
	m_depthTextureFormat = GL_DEPTH_COMPONENT24;
	m_featureFlags |= RENDERER_FEATURE_FLAG_DEPTH_TEXTURE;

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
	glfwMakeContextCurrent( m_pGlfwWindow );
	m_spMainContext = new GLMainContext( m_pGlfwWindow );
	HELIUM_ASSERT( m_spMainContext );

	// Initialize GLEW before any GL calls are made.
	glewExperimental = GL_TRUE;
	HELIUM_ASSERT( GLEW_OK == glewInit() );

	// Collect availability of OpenGL extensions.
	m_bHasS3tcExt = GLEW_EXT_texture_compression_s3tc != 0;
	if( !m_bHasS3tcExt )
	{
		HELIUM_TRACE( TraceLevels::Warning, "GLRenderer: OpenGL S3TC extension not available.  Some textures may fail to load.\n" );
	}
	m_bHasSRGBExt = GLEW_EXT_texture_sRGB != 0;
	if( !m_bHasSRGBExt )
	{
		HELIUM_TRACE( TraceLevels::Warning, "GLRenderer: OpenGL sRGB Texture extension not available.  Some textures may fail to load.\n" );
	}
	m_bHasAnisotropicExt = GLEW_EXT_texture_filter_anisotropic != 0;
	if( !m_bHasAnisotropicExt )
	{
		HELIUM_TRACE( TraceLevels::Warning, "GLRenderer: OpenGL anisotropic filtering extension not available.  Anisotropy level will be set to 1.\n" );
	}

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
	// TODO!

	return STATUS_READY;
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
	GLRasterizerState* pState = new GLRasterizerState;
	HELIUM_ASSERT( pState );

	if( !pState->Initialize( rDescription ) )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLRenderer: Failed to create rasterizer state.\n" );
		return NULL;
	}
	return pState;
}

/// @copydoc Renderer::CreateBlendState()
RBlendState* GLRenderer::CreateBlendState( const RBlendState::Description& rDescription )
{
	GLBlendState* pState = new GLBlendState;
	HELIUM_ASSERT( pState );

	if( !pState->Initialize( rDescription ) )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLRenderer: Failed to create blend state.\n" );
		return NULL;
	}
	return pState;
}

/// @copydoc Renderer::CreateDepthStencilState()
RDepthStencilState* GLRenderer::CreateDepthStencilState( const RDepthStencilState::Description& rDescription )
{
	GLDepthStencilState* pState = new GLDepthStencilState;
	HELIUM_ASSERT( pState );

	if( !pState->Initialize( rDescription ) )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLRenderer: Failed to create depth/stencil state.\n" );
		return NULL;
	}
	return pState;
}

/// @copydoc Renderer::CreateSamplerState()
RSamplerState* GLRenderer::CreateSamplerState( const RSamplerState::Description& rDescription )
{
	GLSamplerState* pState = new GLSamplerState;
	HELIUM_ASSERT( pState );

	if( !pState->Initialize( rDescription ) )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLRenderer: Failed to create sampler state.\n" );
		return NULL;
	}
	return pState;
}

/// @copydoc Renderer::CreateDepthStencilSurface()
RSurface* GLRenderer::CreateDepthStencilSurface(
	uint32_t width,
	uint32_t height,
	ERendererSurfaceFormat format,
	uint32_t multisampleCount )
{
	HELIUM_ASSERT( static_cast< size_t >( format ) < static_cast< size_t >( RENDERER_SURFACE_FORMAT_MAX ) );

	static const GLenum glFormats[ RENDERER_SURFACE_FORMAT_MAX ][ 2 ] =
	{
		{ GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT },         // RENDERER_SURFACE_FORMAT_DEPTH_ONLY
		{ GL_DEPTH24_STENCIL8,  GL_DEPTH_STENCIL_ATTACHMENT }, // RENDERER_SURFACE_FORMAT_DEPTH_STENCIL
	};

	if( multisampleCount > 16 )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			"GLRenderer::CreateDepthStencilSurface(): Multisample count cannot be more than 16.  Value will be clamped.\n" );
		multisampleCount = 16;
	}

	GLuint newDepthStencil = 0;
	glGenRenderbuffers( 1, &newDepthStencil );
	HELIUM_ASSERT( newDepthStencil != 0 );
	if( newDepthStencil == 0 )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"GLRenderer: Failed to generate GL back renderbuffer surface.\n" );
		return NULL;
	}

	// Store off previously bound renderbuffer.
	GLint curRenderbuffer = 0;
	glGetIntegerv( GL_RENDERBUFFER_BINDING, &curRenderbuffer );

	// Create a new renderbuffer object for the back buffer.
	HELIUM_ASSERT( m_pGlfwWindow != NULL );
	glBindRenderbuffer( GL_RENDERBUFFER, newDepthStencil );
	glRenderbufferStorageMultisample(
		GL_RENDERBUFFER, multisampleCount, glFormats[ format ][ 0 ], width, height );

	// Restore previous renderbuffer.
	glBindRenderbuffer( GL_RENDERBUFFER, curRenderbuffer );

	// Construct the GLSurface object.
	GLSurface *depthStencilSurface = new GLSurface( newDepthStencil, glFormats[ format ][ 1 ], false );
	HELIUM_ASSERT( depthStencilSurface != NULL );

	return depthStencilSurface;
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
	// Vertex and index buffers can only be created with static or dynamic usage semantics.
	HELIUM_ASSERT( usage == RENDERER_BUFFER_USAGE_STATIC || usage == RENDERER_BUFFER_USAGE_DYNAMIC );
	if( usage != RENDERER_BUFFER_USAGE_STATIC && usage != RENDERER_BUFFER_USAGE_DYNAMIC )
	{
		HELIUM_TRACE(TraceLevels::Error,
			"GLRenderer::CreateVertexBuffer(): Buffers can only be created with static or dynamic usage semantics.\n" );
		return NULL;
	}

	// Determine buffer usage.
	const bool bDynamic = ( usage == RENDERER_BUFFER_USAGE_DYNAMIC );
	const GLenum usageGl = bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	// Create vertex buffer object.
	unsigned buffer = 0;
	glBindVertexArray( 0 );
	glGenBuffers( 1, &buffer );
	HELIUM_ASSERT( buffer != 0 );

	// Optionally copy provided vertex data into the buffer.  Note
	// that if pData is NULL, the buffer will be allocated but contents undefined.
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, size, pData, usageGl );
	const bool bValidData = (pData != NULL);

	// Create Helium GL vertex buffer object.
	GLVertexBuffer *vertexBuffer = new GLVertexBuffer( buffer );
	if( !vertexBuffer )
	{
		HELIUM_TRACE(TraceLevels::Error,
			"GLRenderer::CreateVertexBuffer(): Failed to create GLBuffer instance.\n" );
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
	// Vertex and index buffers can only be created with static or dynamic usage semantics.
	HELIUM_ASSERT( usage == RENDERER_BUFFER_USAGE_STATIC || usage == RENDERER_BUFFER_USAGE_DYNAMIC );
	if( usage != RENDERER_BUFFER_USAGE_STATIC && usage != RENDERER_BUFFER_USAGE_DYNAMIC )
	{
		HELIUM_TRACE(TraceLevels::Error,
			"GLRenderer::CreateIndexBuffer(): Buffers can only be created with static or dynamic usage semantics.\n" );
		return NULL;
	}

	// Determine buffer usage.
	const bool bDynamic = ( usage == RENDERER_BUFFER_USAGE_DYNAMIC );
	const GLenum usageGl = bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	// Create buffer object.
	unsigned buffer = 0;
	glBindVertexArray( 0 );
	glGenBuffers( 1, &buffer );
	HELIUM_ASSERT( buffer != 0 );
	
	// Optionally copy provided vertex data into the buffer.  Note
	// that if pData is NULL, the buffer will be allocated but contents undefined.
	glBindBuffer( GL_ARRAY_BUFFER, buffer );
	glBufferData( GL_ARRAY_BUFFER, size, pData, usageGl );
	const bool bValidData = (pData != NULL);

	// Determine index element type.
	const GLenum elementType = (format == RENDERER_INDEX_FORMAT_UINT32) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;

	// Create Helium GL vertex buffer object.
	GLIndexBuffer *indexBuffer = new GLIndexBuffer( elementType, buffer );
	if( !indexBuffer )
	{
		HELIUM_TRACE(TraceLevels::Error,
			"GLRenderer::CreateIndexBuffer(): Failed to create GLBuffer instance.\n" );
		return NULL;
	}

	return indexBuffer;
}

/// @copydoc Renderer::CreateConstantBuffer()
RConstantBuffer* GLRenderer::CreateConstantBuffer(
	size_t size,
	ERendererBufferUsage /*usage*/,
	const void* pData )
{
	HELIUM_ASSERT( size != 0 );

	// Pad the buffer size to be a multiple of the size of a single float vector register.
	size_t actualSize = Align( size, sizeof( float32_t ) * 4 );

	// Compute the number of registers covered by the buffer and test for any possible overflow when casting to a
	// 16-bit integer.
	size_t registerCount = actualSize / ( sizeof( float32_t ) * 4 );
	if( registerCount > UINT16_MAX )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"GLRenderer::CreateConstantBuffer(): Buffer size (%" PRIuSZ ") is larger than the maximum size supported (%" PRIuSZ ").\n",
			size,
			sizeof( float32_t ) * 4 * UINT16_MAX );
		return NULL;
	}

	// Allocate the buffer memory.
	void* pBufferMemory = DefaultAllocator().Allocate( actualSize );
	HELIUM_ASSERT( pBufferMemory );
	if( !pBufferMemory )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"GLRenderer::CreateConstantBuffer(): Failed to allocate %" PRIuSZ " bytes for constant buffer data.\n",
			actualSize );
		return NULL;
	}

	// Initialize the buffer contents if a data pointer was provided.
	if( pData )
	{
		MemoryCopy( pBufferMemory, pData, size );
	}

	// Create the buffer interface.
	GLConstantBuffer* pBuffer = new GLConstantBuffer( pBufferMemory, static_cast< uint16_t >( registerCount ) );
	
	HELIUM_ASSERT( pBuffer );
	return pBuffer;
}

/// @copydoc Renderer::CreateVertexDescription()
RVertexDescription* GLRenderer::CreateVertexDescription(
	const RVertexDescription::Element* pElements,
	size_t elementCount )
{
	HELIUM_ASSERT( pElements );
	HELIUM_ASSERT( elementCount != 0 );

	// Make sure we have vertex elements from which to create a description object.
	if( elementCount == 0 )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			"GLRenderer::CreateVertexDescription(): Cannot create a vertex description with no elements.\n" );
		return NULL;
	}

	GLVertexDescription* pDescription = new GLVertexDescription;
	HELIUM_ASSERT( pDescription );

	if( !pDescription->Initialize( pElements, elementCount ) )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLRenderer: Failed to create vertex description.\n" );
		return NULL;
	}
	return pDescription;
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
	HELIUM_ASSERT( static_cast< size_t >( format ) < static_cast< size_t >( RENDERER_PIXEL_FORMAT_MAX ) );
	HELIUM_ASSERT( static_cast< size_t >( usage ) < static_cast< size_t >( RENDERER_BUFFER_USAGE_MAX ) );

	// Create texture buffer object.
	GLuint buffer = 0;
	glGenTextures( 1, &buffer );
	HELIUM_ASSERT( buffer != 0 );
	if( buffer == 0 )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLRenderer::CreateTexture2d(): Failed to create texture buffer.\n" );
		return NULL;
	}

	// Convert Helium texture format to an OpenGL texture format.
	GLenum internalFormat = GL_NONE;
	GLenum pixelFormat = GL_NONE;
	GLenum elementType = GL_NONE;
	const bool isCompressed = RendererUtil::IsCompressedFormat( format );
	PixelFormatToGLFormat( format, internalFormat, pixelFormat, elementType );
	HELIUM_ASSERT( internalFormat != GL_NONE && pixelFormat != GL_NONE && elementType != GL_NONE );
	if( internalFormat == GL_NONE || pixelFormat == GL_NONE || elementType == GL_NONE )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLRenderer::CreateTexture2d(): Failed to identify OpenGL texture format.\n" );
		glDeleteTextures( 1, &buffer );
		return NULL;
	}

	// Store the currently bound 2D texture.
	GLint curTexture2D;
	glGetIntegerv( GL_TEXTURE_BINDING_2D, &curTexture2D );

	// Specify and allocate a two-dimensional texture and all mip levels using the given parameters.
	glBindTexture( GL_TEXTURE_2D, buffer );
	uint32_t mipWidth = width;
	uint32_t mipHeight = height;
	for( uint32_t mipIndex = 0; mipIndex < mipCount; ++mipIndex )
	{
		glTexImage2D( GL_TEXTURE_2D, mipIndex, internalFormat, mipWidth, mipHeight, 0, pixelFormat, elementType, NULL );
		mipWidth = ( mipWidth + 1 ) / 2;
		mipHeight = ( mipHeight + 1 ) / 2;
	}

	// Optionally populate the texture with data if provided.
	if( pData )
	{
		mipWidth = width;
		mipHeight = height;
		for( uint32_t mipIndex = 0; mipIndex < mipCount; ++mipIndex )
		{
			// Get width of a row of pixels/blocks, in bytes.
			const RTexture2d::CreateData& rCreateData = pData[ mipIndex ];
			const uint8_t* pSource = static_cast< const uint8_t* >( rCreateData.pData );
			HELIUM_ASSERT( pSource );
			const uint32_t sourcePitch = static_cast< uint32_t >( rCreateData.pitch );

			// Calculate and set the pixel pack alignment for this data.
			const GLint pixelPackAlign = static_cast< GLint >( RendererUtil::PixelPitchToPackAlignment( sourcePitch, 8 ) );
			glTexParameteri( GL_TEXTURE_2D, GL_PACK_ALIGNMENT, pixelPackAlign );

			// Upload texture data.
			if( !isCompressed )
			{
				glTexSubImage2D( GL_TEXTURE_2D, mipIndex, 0, 0, mipWidth, mipHeight, pixelFormat, elementType, pSource );
			}
			else
			{
				const GLsizei imageSize = sourcePitch * RendererUtil::PixelToBlockRowCount( mipHeight, format );
				glCompressedTexSubImage2D( GL_TEXTURE_2D, mipIndex, 0, 0, mipWidth, mipHeight, internalFormat, imageSize, pSource );
			}

			mipWidth = ( mipWidth + 1 ) / 2;
			mipHeight = ( mipHeight + 1 ) / 2;
		}
	}

	GLTexture2d *pTexture = new GLTexture2d( buffer, mipCount, format );
	HELIUM_ASSERT( pTexture );

	glBindTexture( GL_TEXTURE_2D, curTexture2D );

	return pTexture;
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

	return NULL;
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
