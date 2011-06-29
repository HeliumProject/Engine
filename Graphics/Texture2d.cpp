//----------------------------------------------------------------------------------------------------------------------
// Texture2d.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Graphics/Texture2d.h"

#include "Rendering/RendererUtil.h"
#include "Rendering/Renderer.h"
#include "Rendering/RTexture2d.h"

using namespace Helium;

L_IMPLEMENT_OBJECT( Texture2d, Graphics, GameObjectType::FLAG_NO_TEMPLATE );

/// Constructor.
Texture2d::Texture2d()
{
}

/// Destructor.
Texture2d::~Texture2d()
{
}

/// @copydoc GameObject::NeedsPrecacheResourceData()
bool Texture2d::NeedsPrecacheResourceData() const
{
    return true;
}

/// @copydoc GameObject::BeginPrecacheResourceData()
bool Texture2d::BeginPrecacheResourceData()
{
    HELIUM_ASSERT( m_renderResourceLoadIds.IsEmpty() );

    // Don't load any resources if we have no texture resource (texture resource should already be allocated in
    // SerializePersistentResourceData()).
    RTexture2d* pTexture2d = static_cast< RTexture2d* >( m_spTexture.Get() );
    if( !pTexture2d )
    {
        return true;
    }

    // Begin loading each mip level.
    uint32_t mipCount = pTexture2d->GetMipCount();
    if( mipCount == 0 )
    {
        return true;
    }

    m_renderResourceLoadIds.Reserve( mipCount );
    m_renderResourceLoadIds.Resize( mipCount );
    m_renderResourceLoadIds.Trim();

    ERendererPixelFormat format = pTexture2d->GetPixelFormat();
    HELIUM_ASSERT( static_cast< size_t >( format ) < static_cast< size_t >( RENDERER_PIXEL_FORMAT_MAX ) );

    for( uint32_t mipIndex = 0; mipIndex < mipCount; ++mipIndex )
    {
        SetInvalid( m_renderResourceLoadIds[ mipIndex ] );

        size_t pitch;
        void* pMipData = pTexture2d->Map( mipIndex, pitch );
        HELIUM_ASSERT( pMipData );
        if( !pMipData )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "Texture2d::BeginPrecacheResourceData(): Failed to lock mip level %" ) TPRIu32 TXT( ".\n" ),
                mipIndex );

            continue;
        }

        uint32_t mipLevelHeight = pTexture2d->GetHeight( mipIndex );
        size_t rowCount = RendererUtil::PixelToBlockRowCount( mipLevelHeight, format );
        size_t mipLevelSize = pitch * rowCount;

        HELIUM_ASSERT( mipLevelSize == GetSubDataSize( mipIndex ) );

        size_t loadId = BeginLoadSubData( pMipData, mipIndex, mipLevelSize );
        HELIUM_ASSERT( IsValid( loadId ) );
        if( IsInvalid( loadId ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Texture2d::BeginPrecacheResourceData(): Failed to begin loading of cached data for mip " )
                TXT( "level %" ) TPRIu32 TXT( ".\n" ) ),
                mipIndex );

            pTexture2d->Unmap( mipIndex );

            continue;
        }

        m_renderResourceLoadIds[ mipIndex ] = loadId;
    }

    return true;
}

/// @copydoc GameObject::TryFinishPrecacheResourceData()
bool Texture2d::TryFinishPrecacheResourceData()
{
    // Check all pending load requests.
    size_t loadRequestCount = m_renderResourceLoadIds.GetSize();
    if( loadRequestCount == 0 )
    {
        return true;
    }

    RTexture2d* pTexture2d = static_cast< RTexture2d* >( m_spTexture.Get() );
    HELIUM_ASSERT( pTexture2d );
    HELIUM_ASSERT( loadRequestCount == pTexture2d->GetMipCount() );

    bool bHaveUnfinishedLoad = false;

    for( size_t loadRequestIndex = 0; loadRequestIndex < loadRequestCount; ++loadRequestIndex )
    {
        size_t loadId = m_renderResourceLoadIds[ loadRequestIndex ];
        if( IsInvalid( loadId ) )
        {
            continue;
        }

        if( !TryFinishLoadSubData( loadId ) )
        {
            bHaveUnfinishedLoad = true;

            continue;
        }

        SetInvalid( m_renderResourceLoadIds[ loadRequestIndex ] );
        pTexture2d->Unmap( static_cast< uint32_t >( loadRequestIndex ) );
    }

    if( bHaveUnfinishedLoad )
    {
        return false;
    }

    m_renderResourceLoadIds.Clear();

    return true;
}

/// @copydoc Resource::SerializePersistentResourceData()
void Texture2d::SerializePersistentResourceData( Serializer& s )
{
    uint32_t baseLevelWidth = 0;
    uint32_t baseLevelHeight = 0;
    uint32_t mipCount = 0;
    int32_t pixelFormatIndex = RENDERER_PIXEL_FORMAT_INVALID;

    RTexture2d* pTexture2d = static_cast< RTexture2d* >( m_spTexture.Get() );
    if( pTexture2d )
    {
        baseLevelWidth = pTexture2d->GetWidth();
        baseLevelHeight = pTexture2d->GetHeight();
        mipCount = pTexture2d->GetMipCount();
        pixelFormatIndex = pTexture2d->GetPixelFormat();
    }

    s << baseLevelWidth;
    s << baseLevelHeight;
    s << mipCount;
    s << pixelFormatIndex;

    if( s.GetMode() == Serializer::MODE_LOAD )
    {
        m_spTexture.Release();

        Renderer* pRenderer = Renderer::GetStaticInstance();
        if( pRenderer &&
            baseLevelWidth != 0 &&
            baseLevelHeight != 0 &&
            mipCount != 0 &&
            static_cast< uint32_t >( pixelFormatIndex ) < static_cast< uint32_t >( RENDERER_PIXEL_FORMAT_MAX ) )
        {
            m_spTexture = pRenderer->CreateTexture2d(
                baseLevelWidth,
                baseLevelHeight,
                mipCount,
                static_cast< ERendererPixelFormat >( pixelFormatIndex ),
                RENDERER_BUFFER_USAGE_STATIC );
            if( !m_spTexture )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Texture2d::SerializePersistentResourceData(): Failed to create texture render " )
                    TXT( "resource (width: %" ) TPRIu32 TXT( "; height: %" ) TPRIu32 TXT( "; mip count: %" )
                    TPRIu32 TXT( "; pixel format index: %" ) TPRId32 TXT( ").\n" ) ),
                    baseLevelWidth,
                    baseLevelHeight,
                    mipCount,
                    pixelFormatIndex );
            }
        }
    }
}

/// @copydoc Texture::GetRenderResource2d()
RTexture2d* Texture2d::GetRenderResource2d() const
{
    return static_cast< RTexture2d* >( m_spTexture.Get() );
}
