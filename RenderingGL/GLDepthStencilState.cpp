#include "RenderingGLPch.h"
#include "RenderingGL/GLDepthStencilState.h"

using namespace Helium;

// Translate an OpenGL compare function to an engine compare function.
static ERendererCompareFunction TranslateGLCompareFunction( GLenum compareFunction )
{
	switch( compareFunction )
	{
	case GL_NEVER:
		return RENDERER_COMPARE_FUNCTION_NEVER;
	case GL_LESS:
		return RENDERER_COMPARE_FUNCTION_LESS;
	case GL_EQUAL:
		return RENDERER_COMPARE_FUNCTION_EQUAL;
	case GL_LEQUAL:
		return RENDERER_COMPARE_FUNCTION_LESS_EQUAL;
	case GL_GREATER:
		return RENDERER_COMPARE_FUNCTION_GREATER;
	case GL_NOTEQUAL:
		return RENDERER_COMPARE_FUNCTION_NOT_EQUAL;
	case GL_GEQUAL:
		return RENDERER_COMPARE_FUNCTION_GREATER_EQUAL;
	case GL_ALWAYS:
		return RENDERER_COMPARE_FUNCTION_ALWAYS;
	default:
		return RENDERER_COMPARE_FUNCTION_INVALID;
	}
}

// Translate an OpenGL stencil operation to an engine stencil operation.
static ERendererStencilOperation TranslateGLStencilOperation( GLenum stencilOperation )
{
	switch( stencilOperation )
	{
	case GL_KEEP:
		return RENDERER_STENCIL_OPERATION_KEEP;
	case GL_ZERO:
		return RENDERER_STENCIL_OPERATION_ZERO;
	case GL_REPLACE:
		return RENDERER_STENCIL_OPERATION_REPLACE;
	case GL_INCR:
		return RENDERER_STENCIL_OPERATION_INCREMENT;
	case GL_INCR_WRAP:
		return RENDERER_STENCIL_OPERATION_INCREMENT_WRAP;
	case GL_DECR:
		return RENDERER_STENCIL_OPERATION_DECREMENT;
	case GL_DECR_WRAP:
		return RENDERER_STENCIL_OPERATION_DECREMENT_WRAP;
	case GL_INVERT:
		return RENDERER_STENCIL_OPERATION_INVERT;
	default:
		return RENDERER_STENCIL_OPERATION_INVALID;
	}
}

/// Constructor.
GLDepthStencilState::GLDepthStencilState()
: m_depthTestEnable( true )
, m_depthWriteEnable( true )
, m_depthFunction( GL_LEQUAL )
, m_stencilTestEnable( false )
, m_stencilFunction( GL_ALWAYS )
, m_stencilFailOperation( GL_KEEP )
, m_stencilDepthFailOperation( GL_KEEP )
, m_stencilDepthPassOperation( GL_REPLACE )
, m_stencilReadMask( 0 )
, m_stencilWriteMask( 0 )
{}

/// Destructor.
GLDepthStencilState::~GLDepthStencilState()
{
}

/// Initialize this state object.
///
/// @param[in] rDescription  State description.
///
/// @return  True if initialization was successful, false if not.
bool GLDepthStencilState::Initialize( const Description& rDescription )
{
	// Range check arguments.
	HELIUM_ASSERT(
		static_cast< size_t >( rDescription.depthFunction ) <
		static_cast< size_t >( RENDERER_COMPARE_FUNCTION_MAX ) );
	HELIUM_ASSERT(
		static_cast< size_t >( rDescription.stencilFailOperation ) <
		static_cast< size_t >( RENDERER_STENCIL_OPERATION_MAX ) );
	HELIUM_ASSERT(
		static_cast< size_t >( rDescription.stencilDepthFailOperation ) <
		static_cast< size_t >( RENDERER_STENCIL_OPERATION_MAX ) );
	HELIUM_ASSERT(
		static_cast< size_t >( rDescription.stencilDepthPassOperation ) <
		static_cast< size_t >( RENDERER_STENCIL_OPERATION_MAX ) );
	HELIUM_ASSERT(
		static_cast< size_t >( rDescription.stencilFunction ) <
		static_cast< size_t >( RENDERER_COMPARE_FUNCTION_MAX ) );
	if( ( static_cast< size_t >( rDescription.depthFunction ) >=
		static_cast< size_t >( RENDERER_COMPARE_FUNCTION_MAX ) ) ||
		( static_cast< size_t >( rDescription.stencilFailOperation ) >=
		static_cast< size_t >( RENDERER_STENCIL_OPERATION_MAX ) ) ||
		( static_cast< size_t >( rDescription.stencilDepthFailOperation ) >=
		static_cast< size_t >( RENDERER_STENCIL_OPERATION_MAX ) ) ||
		( static_cast< size_t >( rDescription.stencilDepthPassOperation ) >=
		static_cast< size_t >( RENDERER_STENCIL_OPERATION_MAX ) ) ||
		( static_cast< size_t >( rDescription.stencilFunction ) >=
		static_cast< size_t >( RENDERER_COMPARE_FUNCTION_MAX ) ) )
	{
		return false;
	}

	// Define lookup tables.
	static const GLenum compareFunctions[ RENDERER_COMPARE_FUNCTION_MAX ] =
	{
		GL_NEVER,    // RENDERER_COMPARE_FUNCTION_NEVER
		GL_LESS,     // RENDERER_COMPARE_FUNCTION_LESS,
		GL_EQUAL,    // RENDERER_COMPARE_FUNCTION_EQUAL
		GL_LEQUAL,   // RENDERER_COMPARE_FUNCTION_LESS_EQUAL
		GL_GREATER,  // RENDERER_COMPARE_FUNCTION_GREATER
		GL_NOTEQUAL, // RENDERER_COMPARE_FUNCTION_NOT_EQUAL
		GL_GEQUAL,   // RENDERER_COMPARE_FUNCTION_GREATER_EQUAL
		GL_ALWAYS    // RENDERER_COMPARE_FUNCTION_ALWAYS
	};
	static const GLenum stencilOperations[ RENDERER_STENCIL_OPERATION_MAX ] =
	{
		GL_KEEP,      // RENDERER_STENCIL_OPERATION_KEEP
		GL_ZERO,      // RENDERER_STENCIL_OPERATION_ZERO
		GL_REPLACE,   // RENDERER_STENCIL_OPERATION_REPLACE
		GL_INCR,      // RENDERER_STENCIL_OPERATION_INCREMENT
		GL_INCR_WRAP, // RENDERER_STENCIL_OPERATION_INCREMENT_WRAP
		GL_DECR,      // RENDERER_STENCIL_OPERATION_DECREMENT
		GL_DECR_WRAP, // RENDERER_STENCIL_OPERATION_DECREMENT_WRAP,
		GL_INVERT,    // RENDERER_STENCIL_OPERATION_INVERT
	};

	// Internalize depth test/write and stencil test/write flags.
	m_depthTestEnable = rDescription.bDepthTestEnable;
	m_depthWriteEnable = rDescription.bDepthWriteEnable;
	m_stencilTestEnable = rDescription.bStencilTestEnable;

	// Internalize depth and stencil function and operation values.
	m_depthFunction = compareFunctions[ rDescription.depthFunction ];
	m_stencilFunction = compareFunctions[ rDescription.stencilFunction ];
	m_stencilFailOperation = stencilOperations[ rDescription.stencilFailOperation ];
	m_stencilDepthFailOperation = stencilOperations[ rDescription.stencilDepthFailOperation ];
	m_stencilDepthPassOperation = stencilOperations[ rDescription.stencilDepthPassOperation ];

	// Internalize stencil mask values.
	m_stencilReadMask = static_cast<GLuint>(rDescription.stencilReadMask);
	m_stencilWriteMask = static_cast<GLuint>(rDescription.stencilWriteMask);

	return true;
}

/// @copydoc RDepthStencilState::GetDescription()
void GLDepthStencilState::GetDescription( Description& rDescription ) const
{
	// Get depth and stencil function and operation values.
	rDescription.depthFunction = TranslateGLCompareFunction( m_depthFunction );
	rDescription.stencilFailOperation = TranslateGLStencilOperation( m_stencilFailOperation );
	rDescription.stencilDepthFailOperation = TranslateGLStencilOperation( m_stencilDepthFailOperation );
	rDescription.stencilDepthPassOperation = TranslateGLStencilOperation( m_stencilDepthPassOperation );
	rDescription.stencilFunction = TranslateGLCompareFunction( m_stencilFunction );

	// Get stencil mask values.
	rDescription.stencilReadMask = static_cast< uint8_t >( m_stencilReadMask );
	rDescription.stencilWriteMask = static_cast< uint8_t >( m_stencilWriteMask );

	// Get depth/stencil enable flags.
	rDescription.bDepthTestEnable = m_depthTestEnable;
	rDescription.bDepthWriteEnable = m_depthWriteEnable;
	rDescription.bStencilTestEnable = m_stencilTestEnable;
}
