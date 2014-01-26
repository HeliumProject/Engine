#include "RenderingGLPch.h"
#include "RenderingGL/GLTexture2d.h"

#include "RenderingGL/GLSurface.h"

#include "Rendering/RendererUtil.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] texture  OpenGL texture to wrap.
/// @param[in] format   Format of the provided texture object.
GLTexture2d::GLTexture2d( GLuint texture, uint32_t mipCount, ERendererPixelFormat format )
: m_texture( texture )
, m_mipCount( mipCount )
, m_format( format )
{
	HELIUM_ASSERT( texture );
	HELIUM_ASSERT( mipCount );
	HELIUM_ASSERT( format < ERendererPixelFormat::RENDERER_PIXEL_FORMAT_MAX );
}

/// Destructor.
GLTexture2d::~GLTexture2d()
{
	if( m_texture )
	{
		glDeleteBuffers( 1, &m_texture );
		m_texture = 0;
	}
}

/// @copydoc RTexture::GetMipCount()
uint32_t GLTexture2d::GetMipCount() const
{
	return m_mipCount;
}

/// @copydoc RTexture2d::Map()
void* GLTexture2d::Map( uint32_t mipLevel, size_t& rPitch, ERendererBufferMapHint hint )
{
	/*
    // Whole-resource mapping is not supported with Direct3D 9 on the PC.
    HELIUM_ASSERT( IsValid( mipLevel ) );

    // Attempt to lock the requested mip level (we intentionally skip manual mip index validation for performance in
    // favor of just checking for a successful result when calling IDirect3DTexture9::LockRect()).
    D3DLOCKED_RECT lockedRect;
    DWORD flags = 0;
    if( hint == RENDERER_BUFFER_MAP_HINT_DISCARD )
    {
        flags = D3DLOCK_DISCARD;
    }
    else if( hint == RENDERER_BUFFER_MAP_HINT_NO_OVERWRITE )
    {
        flags = D3DLOCK_NOOVERWRITE;
    }

    HRESULT result = m_pTexture->LockRect( mipLevel, &lockedRect, NULL, flags );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "D3D9Texture2d::Map(): Failed to lock mip level %" ) PRIu32 TXT( " (error code 0x%x).\n" ),
            mipLevel,
            result );

        return NULL;
    }

    HELIUM_ASSERT( lockedRect.pBits );

    rPitch = static_cast< size_t >( lockedRect.Pitch );

    return lockedRect.pBits;
	*/

	HELIUM_BREAK();
	return NULL;
}

/// @copydoc RTexture2d::Unmap()
void GLTexture2d::Unmap( uint32_t mipLevel )
{
	/*
    // Make sure the mip level is valid (whole-resource mapping is not supported with Direct3D 9 on the PC, so we
    // know an invalid index value should not be provided either).
    HELIUM_ASSERT( mipLevel < m_mipCount );

    HRESULT result = m_pTexture->UnlockRect( mipLevel );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "D3D9Texture2d::Unmap(): Failed to unlock texture mip level %" ) PRIu32 TXT( " (error code " )
            TXT( "0x%x).\n" ) ),
            mipLevel,
            result );
    }
	*/

	HELIUM_BREAK();
}

/// @copydoc RTexture2d::CanMapWholeResource()
bool GLTexture2d::CanMapWholeResource() const
{
	// Revisit this once the OpenGL renderer is working.
	return false;
}

/// @copydoc RTexture2d::GetWidth()
uint32_t GLTexture2d::GetWidth( uint32_t mipLevel ) const
{
	HELIUM_ASSERT( mipLevel < m_mipCount );
	if( mipLevel >= m_mipCount )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLTexture2d::GetWidth(): Provided mipLevel is out of bounds." );
		return 0;
	}

	GLint width = 0;
	glBindTexture( GL_TEXTURE_2D, m_texture );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, mipLevel, GL_TEXTURE_WIDTH, &width );

	return width;
}

/// @copydoc RTexture2d::GetHeight()
uint32_t GLTexture2d::GetHeight( uint32_t mipLevel ) const
{
	HELIUM_ASSERT( mipLevel < m_mipCount );
	if( mipLevel >= m_mipCount )
	{
		HELIUM_TRACE( TraceLevels::Error, "GLTexture2d::GetHeight(): Provided mipLevel is out of bounds." );
		return 0;
	}

	GLint height = 0;
	glBindTexture( GL_TEXTURE_2D, m_texture );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, mipLevel, GL_TEXTURE_HEIGHT, &height );

	return height;
}

/// @copydoc RTexture2d::GetPixelFormat()
ERendererPixelFormat GLTexture2d::GetPixelFormat() const
{
	return m_format;
}

/// Get whether this texture is in sRGB color space.
///
/// @return  True if texture samples are stored in sRGB color space, false if they are in linear space.
bool GLTexture2d::IsSrgb() const
{
	return RendererUtil::IsSrgbPixelFormat( m_format );
}

/// @copydoc RTexture2d::GetSurface()
RSurface* GLTexture2d::GetSurface( uint32_t mipLevel )
{
	// TODO: Implementing this will open a big can of worms.  Currently, a GLSurface encapsulates
	// a GLFWWindow, which is essentially a GL context.  This is incorrect.  A surface should represent
	// an OpenGL render target.

	HELIUM_BREAK();

	return NULL;
}
