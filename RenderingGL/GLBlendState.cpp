#include "RenderingGLPch.h"
#include "RenderingGL/GLBlendState.h"

using namespace Helium;

// Translate a OpenGL blend factor to an engine blend factor.
static ERendererBlendFactor TranslateGLBlendFactor( GLenum blendFactor )
{
	switch( blendFactor )
	{
	case GL_ZERO:
		return RENDERER_BLEND_FACTOR_ZERO;
	case GL_ONE:
		return RENDERER_BLEND_FACTOR_ONE;
	case GL_SRC_COLOR:
		return RENDERER_BLEND_FACTOR_SRC_COLOR;
	case GL_ONE_MINUS_SRC_COLOR:
		return RENDERER_BLEND_FACTOR_INV_SRC_COLOR;
	case GL_SRC_ALPHA:
		return RENDERER_BLEND_FACTOR_SRC_ALPHA;
	case GL_ONE_MINUS_SRC_ALPHA:
		return RENDERER_BLEND_FACTOR_INV_SRC_ALPHA;
	case GL_DST_COLOR:
		return RENDERER_BLEND_FACTOR_DEST_COLOR;
	case GL_ONE_MINUS_DST_COLOR:
		return RENDERER_BLEND_FACTOR_INV_DEST_COLOR;
	case GL_DST_ALPHA:
		return RENDERER_BLEND_FACTOR_DEST_ALPHA;
	case GL_ONE_MINUS_DST_ALPHA:
		return RENDERER_BLEND_FACTOR_INV_DEST_ALPHA;
	case GL_SRC_ALPHA_SATURATE:
		return RENDERER_BLEND_FACTOR_SRC_ALPHA_SATURATE;
	default:
		return RENDERER_BLEND_FACTOR_INVALID;
	}
}

/// Constructor
GLBlendState::GLBlendState()
: m_redWriteMaskEnable( GL_TRUE )
, m_greenWriteMaskEnable( GL_TRUE )
, m_blueWriteMaskEnable( GL_TRUE )
, m_alphaWriteMaskEnable( GL_TRUE )
, m_blendEnable( false )
, m_function( GL_FUNC_ADD )
, m_sourceFactor( GL_SRC_ALPHA )
, m_destinationFactor( GL_ONE_MINUS_SRC_ALPHA )
{}

/// Destructor.
GLBlendState::~GLBlendState()
{}

/// Initialize this state object.
///
/// @param[in] rDescription  State description.
///
/// @return  True if initialization was successful, false if not.
bool GLBlendState::Initialize( const Description& rDescription )
{
	// Range check arguments.
	HELIUM_ASSERT( 
		static_cast< size_t >( rDescription.sourceFactor ) <
		static_cast< size_t >( RENDERER_BLEND_FACTOR_MAX ) );
	HELIUM_ASSERT(
		static_cast< size_t >( rDescription.destinationFactor ) <
		static_cast< size_t >( RENDERER_BLEND_FACTOR_MAX ) );
	HELIUM_ASSERT(
		static_cast< size_t >( rDescription.function ) < static_cast< size_t >( RENDERER_BLEND_FUNCTION_MAX ) );
	if( ( static_cast< size_t >( rDescription.sourceFactor ) >= static_cast< size_t >( RENDERER_BLEND_FACTOR_MAX ) ) ||
		( static_cast< size_t >( rDescription.destinationFactor ) >= static_cast< size_t >( RENDERER_BLEND_FACTOR_MAX ) ) ||
		( static_cast< size_t >( rDescription.function ) >= static_cast< size_t >( RENDERER_BLEND_FUNCTION_MAX ) ) )
	{
		return false;
	}

	// Internalize color write mask.
	m_redWriteMaskEnable = static_cast<GLboolean>( rDescription.colorWriteMask & RENDERER_COLOR_WRITE_MASK_FLAG_RED );
	m_greenWriteMaskEnable = static_cast<GLboolean>( rDescription.colorWriteMask & RENDERER_COLOR_WRITE_MASK_FLAG_GREEN );
	m_blueWriteMaskEnable = static_cast<GLboolean>( rDescription.colorWriteMask & RENDERER_COLOR_WRITE_MASK_FLAG_BLUE );
	m_alphaWriteMaskEnable = static_cast<GLboolean>( rDescription.colorWriteMask & RENDERER_COLOR_WRITE_MASK_FLAG_ALPHA );

	// Internalize blend enable flag.
	m_blendEnable = rDescription.bBlendEnable;

	// Internalize blend equation.
	static const GLenum blendFunctions[ RENDERER_BLEND_FUNCTION_MAX ] =
	{
		GL_FUNC_ADD,              // RENDERER_BLEND_FUNCTION_ADD
		GL_FUNC_SUBTRACT,         // RENDERER_BLEND_FUNCTION_SUBTRACT
		GL_FUNC_REVERSE_SUBTRACT, // RENDERER_BLEND_FUNCTION_REVERSE_SUBTRACT
		GL_MIN,                   // RENDERER_BLEND_FUNCTION_MINIMUM
		GL_MAX                    // RENDERER_BLEND_FUNCTION_MAXIMUM
	};
	m_function = blendFunctions[ rDescription.function ];

	// Internalize source and destination blend factors.
	static const GLenum blendFactors[ RENDERER_BLEND_FACTOR_MAX ] =
	{
		GL_ZERO,                 // RENDERER_BLEND_FACTOR_ZERO
		GL_ONE,                  // RENDERER_BLEND_FACTOR_ONE
		GL_SRC_COLOR,            // RENDERER_BLEND_FACTOR_SRC_COLOR
		GL_ONE_MINUS_SRC_COLOR,  // RENDERER_BLEND_FACTOR_INV_SRC_COLOR
		GL_SRC_ALPHA,            // RENDERER_BLEND_FACTOR_SRC_ALPHA
		GL_ONE_MINUS_SRC_ALPHA,  // RENDERER_BLEND_FACTOR_INV_SRC_ALPHA
		GL_DST_COLOR,            // RENDERER_BLEND_FACTOR_DEST_COLOR
		GL_ONE_MINUS_DST_COLOR,  // RENDERER_BLEND_FACTOR_INV_DEST_COLOR
		GL_DST_ALPHA,            // RENDERER_BLEND_FACTOR_DEST_ALPHA
		GL_ONE_MINUS_DST_ALPHA,  // RENDERER_BLEND_FACTOR_INV_DEST_ALPHA
		GL_SRC_ALPHA_SATURATE    // RENDERER_BLEND_FACTOR_SRC_ALPHA_SATURATE
	};
	m_sourceFactor = blendFactors[ rDescription.sourceFactor ];
	m_destinationFactor = blendFactors[ rDescription.destinationFactor ];

	return true;
}

/// @copydoc RBlendState::GetDescription()
void GLBlendState::GetDescription( Description& rDescription ) const
{
	// Get source and destination blend factor.
	rDescription.sourceFactor = TranslateGLBlendFactor( m_sourceFactor );
	rDescription.destinationFactor = TranslateGLBlendFactor( m_destinationFactor );

	// Get blend equation.
	switch( m_function )
	{
	case GL_FUNC_ADD:
		rDescription.function = RENDERER_BLEND_FUNCTION_ADD;
		break;
	case GL_FUNC_SUBTRACT:
		rDescription.function = RENDERER_BLEND_FUNCTION_SUBTRACT;
		break;
	case GL_FUNC_REVERSE_SUBTRACT:
		rDescription.function = RENDERER_BLEND_FUNCTION_REVERSE_SUBTRACT;
		break;
	case GL_MIN:
		rDescription.function = RENDERER_BLEND_FUNCTION_MINIMUM;
		break;
	case GL_MAX:
		rDescription.function = RENDERER_BLEND_FUNCTION_MAXIMUM;
		break;
	default:
		rDescription.function = RENDERER_BLEND_FUNCTION_INVALID;
	}

	// Get color write mask.
	rDescription.colorWriteMask = 0;
	rDescription.colorWriteMask |= m_redWriteMaskEnable ? RENDERER_COLOR_WRITE_MASK_FLAG_RED : 0;
	rDescription.colorWriteMask |= m_greenWriteMaskEnable ? RENDERER_COLOR_WRITE_MASK_FLAG_GREEN : 0;
	rDescription.colorWriteMask |= m_blueWriteMaskEnable ? RENDERER_COLOR_WRITE_MASK_FLAG_BLUE : 0;
	rDescription.colorWriteMask |= m_alphaWriteMaskEnable ? RENDERER_COLOR_WRITE_MASK_FLAG_ALPHA : 0;

	// Get blend enable flag.
	rDescription.bBlendEnable = m_blendEnable;
}
