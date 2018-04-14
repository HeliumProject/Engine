#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9StaticTexture2d.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] pD3DTexture  Direct3D texture to wrap.  Its reference count will be incremented when this object is
///                         constructed and decremented back when this object is destroyed.
/// @param[in] bSrgb        True if the texture is in sRGB color space, false if not.
D3D9StaticTexture2d::D3D9StaticTexture2d( IDirect3DTexture9* pD3DTexture, bool bSrgb )
: D3D9Texture2d( pD3DTexture, bSrgb )
, m_pMappedTexture( NULL )
, m_baseMipLevel( 0 )
, m_mapCount( 0 )
, m_bIsMappedTexturePooled( false )
{
}

/// Destructor.
D3D9StaticTexture2d::~D3D9StaticTexture2d()
{
    // Texture should not be mapped when its reference count reaches zero.
    HELIUM_ASSERT( !m_pMappedTexture );
}

/// @copydoc RTexture2d::Map()
void* D3D9StaticTexture2d::Map( uint32_t mipLevel, size_t& rPitch, ERendererBufferMapHint hint )
{
    // Whole-resource mapping is not supported with Direct3D 9 on the PC.
    HELIUM_ASSERT( IsValid( mipLevel ) );

    // Acquire a staging texture if necessary.
    D3DSURFACE_DESC surfaceDesc;
    HELIUM_D3D9_VERIFY( m_pTexture->GetLevelDesc( 0, &surfaceDesc ) );

    HELIUM_ASSERT( m_pMappedTexture ? m_mapCount != 0 : m_mapCount == 0 );
    if( !m_pMappedTexture )
    {
        D3D9Renderer* pRenderer = static_cast< D3D9Renderer* >( Renderer::GetInstance() );
        HELIUM_ASSERT( pRenderer );

        uint32_t startMipLevel = 0;
        m_pMappedTexture = pRenderer->GetPooledStaticTextureMapTarget(
            surfaceDesc.Width,
            surfaceDesc.Height,
            D3D9Renderer::D3DFormatToPixelFormat( surfaceDesc.Format, m_bSrgb ),
            startMipLevel );
        if( m_pMappedTexture )
        {
            m_bIsMappedTexturePooled = true;
        }
        else
        {
            m_bIsMappedTexturePooled = false;

            IDirect3DDevice9* pD3DDevice = pRenderer->GetD3DDevice();
            HELIUM_ASSERT( pD3DDevice );

            HRESULT result = pD3DDevice->CreateTexture(
                surfaceDesc.Width,
                surfaceDesc.Height,
                m_pTexture->GetLevelCount(),
                D3DUSAGE_DYNAMIC,
                surfaceDesc.Format,
                D3DPOOL_SYSTEMMEM,
                &m_pMappedTexture,
                NULL );
            if( FAILED( result ) )
            {
                HELIUM_TRACE(
                    TraceLevels::Error,
                    "D3D9StaticTexture2d::Map(): Failed to acquire a staging texture from the renderer pool or allocate our own texture.\n" );

                return NULL;
            }
        }

        m_baseMipLevel = static_cast< uint8_t >( startMipLevel );
    }

    ++m_mapCount;
    HELIUM_ASSERT( m_mapCount != 0 );

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

    HRESULT result = m_pMappedTexture->LockRect(
        static_cast< uint32_t >( m_baseMipLevel ) + mipLevel,
        &lockedRect,
        NULL,
        flags );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9StaticTexture2d::Map(): Failed to lock mip level %" PRIu32 " (error code 0x%x).\n",
            mipLevel,
            result );

        DecrementMapCount();

        return NULL;
    }

    HELIUM_ASSERT( lockedRect.pBits );

    rPitch = static_cast< size_t >( lockedRect.Pitch );

    return lockedRect.pBits;
}

/// @copydoc RTexture2d::Unmap()
void D3D9StaticTexture2d::Unmap( uint32_t mipLevel )
{
    // Make sure the mip level is valid (whole-resource mapping is not supported with Direct3D 9 on the PC, so we
    // know an invalid index value should not be provided either).
    HELIUM_ASSERT( mipLevel < m_pTexture->GetLevelCount() );

    // Unlock the mapped texture.
    HELIUM_ASSERT( m_pMappedTexture );
    HELIUM_ASSERT( m_mapCount != 0 );

    uint32_t mappedMipLevel = static_cast< uint32_t >( m_baseMipLevel ) + mipLevel;
    HRESULT result = m_pMappedTexture->UnlockRect( mappedMipLevel );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            "D3D9StaticTexture2d::Unmap(): Failed to unlock texture mip level %" PRIu32 " (error code 0x%x).\n",
            mipLevel,
            result );
    }

    // Copy over the mip level contents.
    IDirect3DSurface9* pSourceSurface;
    HELIUM_D3D9_VERIFY( m_pMappedTexture->GetSurfaceLevel( mappedMipLevel, &pSourceSurface ) );
    HELIUM_ASSERT( pSourceSurface );

    IDirect3DSurface9* pDestSurface;
    HELIUM_D3D9_VERIFY( m_pTexture->GetSurfaceLevel( mipLevel, &pDestSurface ) );
    HELIUM_ASSERT( pDestSurface );

    D3D9Renderer* pRenderer = static_cast< D3D9Renderer* >( Renderer::GetInstance() );
    HELIUM_ASSERT( pRenderer );

    IDirect3DDevice9* pD3DDevice = pRenderer->GetD3DDevice();
    HELIUM_ASSERT( pD3DDevice );

    result = pD3DDevice->UpdateSurface( pSourceSurface, NULL, pDestSurface, NULL );
    if( FAILED( result ) )
    {
        HELIUM_TRACE( TraceLevels::Error, "D3D9StaticTexture2d::Unmap(): Failed to update unmapped texture data.\n" );
    }

    pSourceSurface->Release();
    pDestSurface->Release();

    DecrementMapCount();
}

/// Handle decrementing the resource map count, releasing any allocated texture resources as necessary.
void D3D9StaticTexture2d::DecrementMapCount()
{
    --m_mapCount;

    // Release the mapped texture if we have no more locks on it.
    if( m_mapCount == 0 )
    {
        if( m_bIsMappedTexturePooled )
        {
            D3D9Renderer* pRenderer = static_cast< D3D9Renderer* >( Renderer::GetInstance() );
            HELIUM_ASSERT( pRenderer );

            pRenderer->ReleasePooledStaticTextureMapTarget( m_pMappedTexture, m_bSrgb );
            m_pMappedTexture = NULL;
        }
        else
        {
            m_pMappedTexture->Release();
        }
    }
}
