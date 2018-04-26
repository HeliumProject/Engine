#include "Precompile.h"
#include "RenderingGL/GLRasterizerState.h"

using namespace Helium;

/// Constructor
GLRasterizerState::GLRasterizerState( )
: m_fillMode( GL_FILL )
, m_cullEnable( true )
, m_cullMode( GL_BACK )
, m_winding( GL_CCW )
, m_depthBiasEnable( false )
, m_depthBiasMode( GL_NONE )
, m_depthBias( 0.0f )
, m_slopeScaledDepthBias( 0.0f )
{}

/// Destructor.
GLRasterizerState::~GLRasterizerState()
{
}

/// Initialize this state object.
///
/// @param[in] rDescription  State description.
///
/// @return  True if initialization was successful, false if not.
bool GLRasterizerState::Initialize( const Description& rDescription )
{
	// Range check arguments.
	HELIUM_ASSERT( static_cast< size_t >( rDescription.fillMode ) < static_cast< size_t >( RENDERER_FILL_MODE_MAX ) );
	HELIUM_ASSERT( static_cast< size_t >( rDescription.cullMode ) < static_cast< size_t >( RENDERER_CULL_MODE_MAX ) );
	HELIUM_ASSERT( static_cast< size_t >( rDescription.winding ) < static_cast< size_t >( RENDERER_WINDING_MAX ) );
	if( static_cast< size_t >( rDescription.fillMode ) >= static_cast< size_t >( RENDERER_FILL_MODE_MAX ) ||
		static_cast< size_t >( rDescription.cullMode ) >= static_cast< size_t >( RENDERER_CULL_MODE_MAX ) ||
		static_cast< size_t >( rDescription.winding ) >= static_cast< size_t >( RENDERER_WINDING_MAX ) )
	{
		return false;
	}

	// Internalize fill mode
	static const GLenum fillModes[ RENDERER_FILL_MODE_MAX ] =
	{
		GL_LINE,  // RENDERER_FILL_MODE_WIREFRAME
		GL_FILL   // RENDERER_FILL_MODE_SOLID
	};
	m_fillMode = fillModes[ rDescription.fillMode ];

	// Internalize cull mode.
	m_cullEnable = (rDescription.cullMode == RENDERER_CULL_MODE_NONE) ? false : true;
	static const GLenum cullModes[ RENDERER_CULL_MODE_MAX ] =
	{
		GL_BACK,  // RENDERER_CULL_MODE_NONE
		GL_FRONT, // RENDERER_CULL_MODE_FRONT
		GL_BACK   // RENDERER_CULL_MODE_BACK
	};
	m_cullMode = cullModes[ rDescription.cullMode ];

	// Internalize winding.
	static const GLenum windingModes[ RENDERER_WINDING_MAX ] =
	{
		GL_CCW,  // RENDERER_WINDING_COUNTER_CLOCKWISE
		GL_CW    // RENDERER_WINDING_CLOCKWISE
	};
	m_winding = windingModes[ rDescription.winding ];

	// Internalize depth bias.
	m_depthBias = static_cast< float32_t >( rDescription.depthBias ) / 65536.0f;
	m_slopeScaledDepthBias = rDescription.slopeScaledDepthBias;

	// Internalize values for enabling depth bias mode.
	m_depthBiasEnable = (m_depthBias != 0.0f || m_slopeScaledDepthBias != 0.0f);
	if( !m_depthBiasEnable )
	{
		m_depthBiasMode = GL_NONE;
	}
	else
	{
		static const GLenum depthBiasModes[ RENDERER_FILL_MODE_MAX ] =
		{
			GL_POLYGON_OFFSET_LINE,  // RENDERER_FILL_MODE_WIREFRAME
			GL_POLYGON_OFFSET_FILL   // RENDERER_FILL_MODE_SOLID
		};
		m_depthBiasMode = depthBiasModes[ rDescription.fillMode ];
	}

	return true;
}

/// @copydoc RRasterizerState::GetDescription()
void GLRasterizerState::GetDescription( Description& rDescription ) const
{
	// Get fill mode.
	switch( m_fillMode )
	{
	case GL_LINE:
		rDescription.fillMode = RENDERER_FILL_MODE_WIREFRAME;
		break;
	case GL_FILL:
		rDescription.fillMode = RENDERER_FILL_MODE_SOLID;
		break;
	default:
		rDescription.fillMode = RENDERER_FILL_MODE_INVALID;
	}

	// Get winding.
	switch( m_winding )
	{
	case GL_CCW:
		rDescription.winding = RENDERER_WINDING_COUNTER_CLOCKWISE;
		break;
	case GL_CW:
		rDescription.winding = RENDERER_WINDING_CLOCKWISE;
		break;
	default:
		rDescription.winding = RENDERER_WINDING_INVALID;
	}

	// Get cull mode.
	if( !m_cullEnable )
	{
		rDescription.cullMode = RENDERER_CULL_MODE_NONE;
	}
	else
	{
		switch( m_cullMode )
		{
		case GL_FRONT:
			rDescription.cullMode = RENDERER_CULL_MODE_FRONT;
			break;
		case GL_BACK:
			rDescription.cullMode = RENDERER_CULL_MODE_BACK;
			break;
		default:
			rDescription.cullMode = RENDERER_CULL_MODE_INVALID;
		} 
	}

	// Get depth bias.
	rDescription.depthBias = static_cast< int32_t >( m_depthBias * 65536.0f + 0.5f );
	rDescription.slopeScaledDepthBias = m_slopeScaledDepthBias;
}
