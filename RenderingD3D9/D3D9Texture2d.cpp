#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9Texture2d.h"

#include "RenderingD3D9/D3D9Surface.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] pD3DTexture  Direct3D texture to wrap.  Its reference count will be incremented when this object is
///                         constructed and decremented back when this object is destroyed.
/// @param[in] bSrgb        True if the texture is in sRGB color space, false if not.
D3D9Texture2d::D3D9Texture2d( IDirect3DTexture9* pD3DTexture, bool bSrgb )
: m_pTexture( pD3DTexture )
, m_bSrgb( bSrgb )
{
    HELIUM_ASSERT( pD3DTexture );
    pD3DTexture->AddRef();
}

/// Destructor.
D3D9Texture2d::~D3D9Texture2d()
{
    m_pTexture->Release();
}

/// @copydoc RTexture::GetMipCount()
uint32_t D3D9Texture2d::GetMipCount() const
{
    DWORD mipCount = m_pTexture->GetLevelCount();

    return mipCount;
}

/// @copydoc RTexture2d::Map()
void* D3D9Texture2d::Map( uint32_t mipLevel, size_t& rPitch, ERendererBufferMapHint hint )
{
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
}

/// @copydoc RTexture2d::Unmap()
void D3D9Texture2d::Unmap( uint32_t mipLevel )
{
    // Make sure the mip level is valid (whole-resource mapping is not supported with Direct3D 9 on the PC, so we
    // know an invalid index value should not be provided either).
    HELIUM_ASSERT( mipLevel < m_pTexture->GetLevelCount() );

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
}

/// @copydoc RTexture2d::CanMapWholeResource()
bool D3D9Texture2d::CanMapWholeResource() const
{
    // Whole-resource mapping is not supported with Direct3D 9 on the PC.
    return false;
}

/// @copydoc RTexture2d::GetWidth()
uint32_t D3D9Texture2d::GetWidth( uint32_t mipLevel ) const
{
    D3DSURFACE_DESC surfaceDesc;
    HRESULT result = m_pTexture->GetLevelDesc( mipLevel, &surfaceDesc );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "D3D9Texture2d::GetWidth(): Failed to get surface description for mip level %" ) PRIu32
            TXT( " (error code 0x%x).\n" ) ),
            mipLevel,
            result );

        return Invalid< uint32_t >();
    }

    return surfaceDesc.Width;
}

/// @copydoc RTexture2d::GetHeight()
uint32_t D3D9Texture2d::GetHeight( uint32_t mipLevel ) const
{
    D3DSURFACE_DESC surfaceDesc;
    HRESULT result = m_pTexture->GetLevelDesc( mipLevel, &surfaceDesc );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "D3D9Texture2d::GetHeight(): Failed to get surface description for mip level %" ) PRIu32
              TXT( " (error code 0x%x).\n" ) ),
            mipLevel,
            result );

        return Invalid< uint32_t >();
    }

    return surfaceDesc.Height;
}

/// @copydoc RTexture2d::GetPixelFormat()
ERendererPixelFormat D3D9Texture2d::GetPixelFormat() const
{
    D3DSURFACE_DESC surfaceDesc;
    L_D3D9_VERIFY( m_pTexture->GetLevelDesc( 0, &surfaceDesc ) );

    return D3D9Renderer::D3DFormatToPixelFormat( surfaceDesc.Format, m_bSrgb );
}

/// @copydoc RTexture2d::GetSurface()
RSurface* D3D9Texture2d::GetSurface( uint32_t mipLevel )
{
    // Get the Direct3D surface for the specified mip level.
    IDirect3DSurface9* pD3DSurface = NULL;
    HRESULT result = m_pTexture->GetSurfaceLevel( mipLevel, &pD3DSurface );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "D3D9Texture2d::GetSurface(): Failed to get surface for mip level %" ) PRIu32 TXT( " (error code " )
              TXT( "0x%x).\n" ) ),
            mipLevel,
            result );

        return NULL;
    }

    HELIUM_ASSERT( pD3DSurface );

    // Get the D3D9Surface object for the Direct3D surface, creating one if necessary.
    D3D9Surface* pSurface = NULL;
    DWORD privateDataSize = static_cast< DWORD >( sizeof( pSurface ) );
    result = pD3DSurface->GetPrivateData( D3D9Renderer::GetPrivateDataGuid(), &pSurface, &privateDataSize );
    if( FAILED( result ) || !pSurface )
    {
        pSurface = new D3D9Surface( pD3DSurface, m_bSrgb );
    }

    HELIUM_ASSERT( pSurface );

    pD3DSurface->Release();

    return pSurface;
}
