#include "Precompile.h"
#include "RenderingGL/GLSamplerState.h"

using namespace Helium;

// Translate an OpenGL texture address mode to an engine texture address mode.
static ERendererTextureAddressMode TranslateGLTextureAddressMode( GLenum addressMode )
{
	switch( addressMode )
	{
	case GL_REPEAT:
		return RENDERER_TEXTURE_ADDRESS_MODE_WRAP;
	case GL_CLAMP_TO_EDGE:
		return RENDERER_TEXTURE_ADDRESS_MODE_CLAMP;
	default:
		return RENDERER_TEXTURE_ADDRESS_MODE_INVALID;
	}
}

/// Constructor.
GLSamplerState::GLSamplerState()
: m_texParameterTarget( GL_TEXTURE_2D )
, m_minFilter( GL_LINEAR_MIPMAP_LINEAR )
, m_magFilter( GL_LINEAR )
, m_mipLodBias( 0.0f )
, m_maxAnisotropy( 1.0f )
, m_addressModeU( GL_REPEAT )
, m_addressModeV( GL_REPEAT )
, m_addressModeW( GL_REPEAT )
{}

/// Destructor.
GLSamplerState::~GLSamplerState()
{}

/// Initialize this state object.
///
/// @param[in] rDescription  State description.
///
/// @return  True if initialization was successful, false if not.
bool GLSamplerState::Initialize( const Description& rDescription )
{
	// Range check arguments.
	HELIUM_ASSERT(
		static_cast< size_t >( rDescription.filter ) <
		static_cast< size_t >( RENDERER_TEXTURE_FILTER_MAX ) );
	HELIUM_ASSERT(
		static_cast< size_t >( rDescription.addressModeU ) <
		static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) );
	HELIUM_ASSERT(
		static_cast< size_t >( rDescription.addressModeV ) <
		static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) );
	HELIUM_ASSERT(
		static_cast< size_t >( rDescription.addressModeW ) <
		static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) );
	if( ( static_cast< size_t >( rDescription.filter ) >= static_cast< size_t >( RENDERER_TEXTURE_FILTER_MAX ) ) ||
		( static_cast< size_t >( rDescription.addressModeU ) >= static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) ) ||
		( static_cast< size_t >( rDescription.addressModeV ) >= static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) ) ||
		( static_cast< size_t >( rDescription.addressModeW ) >= static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) ) )
	{
		return false;
	}

	// Set texture parameter target.  TODO: Should we always assume GL_TEXTURE_2D, or apply these values
	// to all targets?  Our reference D3D implementation implicitly applies to all targets.
	m_texParameterTarget = GL_TEXTURE_2D;

	// Internalize minification and magnification filters.
	static const GLenum filterTypes[ RENDERER_TEXTURE_FILTER_MAX ][ 2 ] =
	{
		// RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_POINT_MIP_POINT
		{ GL_NEAREST_MIPMAP_NEAREST,  GL_NEAREST},
		// RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_POINT_MIP_LINEAR
		{ GL_NEAREST_MIPMAP_LINEAR,   GL_NEAREST},
		// RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT
		{ GL_NEAREST_MIPMAP_NEAREST,  GL_LINEAR},
		// RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_LINEAR_MIP_LINEAR
		{ GL_NEAREST_MIPMAP_LINEAR,   GL_LINEAR},
		// RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_POINT_MIP_POINT
		{ GL_LINEAR_MIPMAP_NEAREST,   GL_NEAREST},
		// RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR
		{ GL_LINEAR_MIPMAP_LINEAR,    GL_NEAREST},
		// RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_POINT
		{ GL_LINEAR_MIPMAP_NEAREST,   GL_LINEAR},
		// RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_LINEAR
		{ GL_LINEAR_MIPMAP_LINEAR,    GL_LINEAR},
		// RENDERER_TEXTURE_FILTER_ANISOTROPIC
		{ GL_LINEAR_MIPMAP_LINEAR,    GL_LINEAR }
	};
	m_minFilter = filterTypes[ rDescription.filter ][ 0 ];
	m_magFilter = filterTypes[ rDescription.filter ][ 1 ];

	// Internalize Mip LOD bias and anisotropy level.
	m_mipLodBias = static_cast<GLfloat>( rDescription.mipLodBias );
	m_maxAnisotropy = (rDescription.filter == RENDERER_TEXTURE_FILTER_ANISOTROPIC) ? static_cast<GLfloat>( rDescription.maxAnisotropy ) : 1.0f;

	// Internalize texture coordinate addressing modes.
	static const GLenum addressModes[ RENDERER_TEXTURE_ADDRESS_MODE_MAX ] =
	{
		GL_REPEAT,        // RENDERER_TEXTURE_ADDRESS_MODE_WRAP
		GL_CLAMP_TO_EDGE  // RENDERER_TEXTURE_ADDRESS_MODE_CLAMP
	};
	m_addressModeU = addressModes[ rDescription.addressModeU ];
	m_addressModeV = addressModes[ rDescription.addressModeV ];
	m_addressModeW = addressModes[ rDescription.addressModeW ];

	return true;
}

/// @copydoc RSamplerState::GetDescription()
void GLSamplerState::GetDescription( Description& rDescription ) const
{
	// Get filter mode.
	if( m_maxAnisotropy > 1.0f )
	{
		rDescription.filter = RENDERER_TEXTURE_FILTER_ANISOTROPIC;
	}
	else
	{
		if( m_magFilter == GL_NEAREST )
		{
			switch( m_minFilter )
			{
			case GL_NEAREST_MIPMAP_NEAREST:
				rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_POINT_MIP_POINT;
				break;
			case GL_NEAREST_MIPMAP_LINEAR:
				rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_POINT_MIP_LINEAR;
				break;
			case GL_LINEAR_MIPMAP_NEAREST:
				rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_POINT_MIP_POINT;
				break;
			case GL_LINEAR_MIPMAP_LINEAR:
				rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
				break;
			default:
				rDescription.filter = RENDERER_TEXTURE_FILTER_INVALID;
			}
		}
		else if (m_magFilter == GL_LINEAR)
		{
			switch( m_minFilter )
			{
			case GL_NEAREST_MIPMAP_NEAREST:
				rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
				break;
			case GL_NEAREST_MIPMAP_LINEAR:
				rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_LINEAR_MIP_LINEAR;
				break;
			case GL_LINEAR_MIPMAP_NEAREST:
				rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_POINT;
				break;
			case GL_LINEAR_MIPMAP_LINEAR:
				rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_LINEAR;
				break;
			default:
				rDescription.filter = RENDERER_TEXTURE_FILTER_INVALID;
			}
		}
		else
		{
			rDescription.filter = RENDERER_TEXTURE_FILTER_INVALID;
		}
	}

	// Get texture address mode.
	rDescription.addressModeU = TranslateGLTextureAddressMode( m_addressModeU );
	rDescription.addressModeV = TranslateGLTextureAddressMode( m_addressModeV );
	rDescription.addressModeW = TranslateGLTextureAddressMode( m_addressModeW );

	// Get Mip LOD bias.
	rDescription.mipLodBias = static_cast<uint32_t>(m_mipLodBias);

	// Get anisotropy level.
	rDescription.maxAnisotropy = static_cast<uint32_t>(m_maxAnisotropy);
}
