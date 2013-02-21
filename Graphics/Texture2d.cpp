#include "GraphicsPch.h"
#include "Graphics/Texture2d.h"

#include "Rendering/RendererUtil.h"
#include "Rendering/Renderer.h"
#include "Rendering/RTexture2d.h"
#include "Reflect/Data/DataDeduction.h"

HELIUM_IMPLEMENT_OBJECT( Helium::Texture2d, Graphics, AssetType::FLAG_NO_TEMPLATE );
REFLECT_DEFINE_OBJECT( Helium::Texture2d::PersistentResourceData );

using namespace Helium;

Texture2d::PersistentResourceData::PersistentResourceData()
: m_baseLevelWidth(0)
, m_baseLevelHeight(0)
, m_mipCount(0)
, m_pixelFormatIndex(RENDERER_PIXEL_FORMAT_INVALID)
{

}

void Texture2d::PersistentResourceData::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &PersistentResourceData::m_baseLevelWidth,     TXT( "m_baseLevelWidth" ) );
    comp.AddField( &PersistentResourceData::m_baseLevelHeight,    TXT( "m_baseLevelHeight" ) );
    comp.AddField( &PersistentResourceData::m_mipCount,           TXT( "m_mipCount" ) );
    comp.AddField( &PersistentResourceData::m_pixelFormatIndex,   TXT( "m_pixelFormatIndex" ) );
}

/// Constructor.
Texture2d::Texture2d()
{
}

/// Destructor.
Texture2d::~Texture2d()
{
}

/// @copydoc Asset::NeedsPrecacheResourceData()
bool Texture2d::NeedsPrecacheResourceData() const
{
    return true;
}

/// @copydoc Asset::BeginPrecacheResourceData()
bool Texture2d::BeginPrecacheResourceData()
{
    HELIUM_ASSERT( m_renderResourceLoadIds.IsEmpty() );

    Renderer* pRenderer = Renderer::GetStaticInstance();
    if ( !pRenderer )
    {
        return true;
    }

    const uint32_t baseLevelWidth = m_persistentResourceData.m_baseLevelWidth;
    const uint32_t baseLevelHeight = m_persistentResourceData.m_baseLevelHeight;
    const uint32_t mipCount = m_persistentResourceData.m_mipCount;
    const int32_t pixelFormatIndex = m_persistentResourceData.m_pixelFormatIndex;

    RTexture2d* pTexture2d = pRenderer->CreateTexture2d(
        baseLevelWidth,
        baseLevelHeight,
        mipCount,
        static_cast< ERendererPixelFormat >( pixelFormatIndex ),
        RENDERER_BUFFER_USAGE_STATIC );

    if ( !pTexture2d )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            ( TXT( "Texture2d::BeginPrecacheResourceData(): Failed to create texture render " )
            TXT( "resource (width: %" ) TPRIu32 TXT( "; height: %" ) TPRIu32 TXT( "; mip count: %" )
            TPRIu32 TXT( "; pixel format index: %" ) TPRId32 TXT( ").\n" ) ),
            baseLevelWidth,
            baseLevelHeight,
            mipCount,
            pixelFormatIndex );

        return false;
    }

    m_spTexture = pTexture2d;

    m_renderResourceLoadIds.Reserve( mipCount );
    m_renderResourceLoadIds.Resize( mipCount );
    m_renderResourceLoadIds.Trim();

    const ERendererPixelFormat format = static_cast< ERendererPixelFormat >( pixelFormatIndex );
    HELIUM_ASSERT( static_cast< size_t >( format ) < static_cast< size_t >( RENDERER_PIXEL_FORMAT_MAX ) );

    for ( uint32_t mipIndex = 0; mipIndex < mipCount; ++mipIndex )
    {
        SetInvalid( m_renderResourceLoadIds[ mipIndex ] );

        size_t pitch;
        void* pMipData = pTexture2d->Map( mipIndex, pitch );
        HELIUM_ASSERT( pMipData );
        if ( !pMipData )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
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
        if ( IsInvalid( loadId ) )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
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

/// @copydoc Asset::TryFinishPrecacheResourceData()
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

bool Texture2d::LoadPersistentResourceObject( Reflect::ObjectPtr& _object )
{
    m_spTexture.Release();

    HELIUM_ASSERT(_object.ReferencesObject());
    if (!_object.ReferencesObject())
    {
        return false;
    }

    _object->CopyTo(&m_persistentResourceData);

    return true;
}

/// @copydoc Texture::GetRenderResource2d()
RTexture2d* Texture2d::GetRenderResource2d() const
{
    return static_cast< RTexture2d* >( m_spTexture.Get() );
}
