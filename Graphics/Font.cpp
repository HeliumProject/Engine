#include "GraphicsPch.h"
#include "Graphics/Font.h"

#include "Rendering/PixelUtil.h"
#include "Rendering/Renderer.h"

using namespace Lunar;

L_IMPLEMENT_OBJECT( Font, Graphics, 0 );  // We allow templating of fonts to generate resources for different font sizes.

/// Constructor.
Font::Font()
    : m_pointSize( static_cast< float32_t >( DEFAULT_POINT_SIZE ) )
    , m_dpi( DEFAULT_DPI )
    , m_textureSheetWidth( DEFAULT_TEXTURE_SHEET_WIDTH )
    , m_textureSheetHeight( DEFAULT_TEXTURE_SHEET_HEIGHT )
    , m_textureCompression( DEFAULT_TEXTURE_COMPRESSION )
    , m_ascender( 0 )
    , m_descender( 0 )
    , m_height( 0 )
    , m_maxAdvance( 0 )
    , m_pCharacters( NULL )
    , m_characterCount( 0 )
    , m_pspTextures( NULL )
    , m_pTextureLoadIds( NULL )
    , m_textureCount( 0 )
    , m_bAntialiased( true )
{
}

/// Destructor.
Font::~Font()
{
    delete [] m_pCharacters;
    delete [] m_pspTextures;
    delete [] m_pTextureLoadIds;
}

/// @copydoc GameObject::Serialize()
void Font::Serialize( Serializer& s )
{
    L_SERIALIZE_SUPER( s );

    s << L_TAGGED( m_pointSize );
    s << L_TAGGED( m_dpi );
    s << L_TAGGED( m_textureSheetWidth );
    s << L_TAGGED( m_textureSheetHeight );
    s << L_TAGGED( m_textureCompression );
    s << L_TAGGED( m_bAntialiased );
}

/// @copydoc GameObject::NeedsPrecacheResourceData()
bool Font::NeedsPrecacheResourceData() const
{
    return true;
}

/// @copydoc GameObject::BeginPrecacheResourceData()
bool Font::BeginPrecacheResourceData()
{
    uint_fast8_t textureCount = m_textureCount;
    HELIUM_ASSERT( m_pspTextures || textureCount == 0 );

    // If we have don't have a renderer, we don't need to load the texture sheets.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        for( uint_fast8_t textureIndex = 0; textureIndex < textureCount; ++textureIndex )
        {
            m_pspTextures[ textureIndex ].Release();
            SetInvalid( m_pTextureLoadIds[ textureIndex ] );
        }

        return true;
    }

    // Allocate and begin loading texture resources.
    ERendererPixelFormat format =
        ( m_textureCompression == COMPRESSION_COLOR_COMPRESSED ? RENDERER_PIXEL_FORMAT_BC1 : RENDERER_PIXEL_FORMAT_R8 );
    size_t blockRowCount = PixelUtil::PixelToBlockRowCount( m_textureSheetHeight, format );

    uint16_t textureSheetWidth = Max< uint16_t >( m_textureSheetWidth, 1 );
    uint16_t textureSheetHeight = Max< uint16_t >( m_textureSheetHeight, 1 );

    for( uint_fast8_t textureIndex = 0; textureIndex < textureCount; ++textureIndex )
    {
        RTexture2d* pTexture = NULL;

        size_t loadId;
        SetInvalid( loadId );

        size_t textureSize = GetSubDataSize( textureIndex );
        if( IsInvalid( textureSize ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Font::BeginPrecacheResourceData(): Unable to locate cached texture data for texture sheet %" )
                  TPRIuFAST8 TXT( " of font \"%s\".\n" ) ),
                textureIndex,
                *GetPath().ToString() );
        }
        else
        {
            pTexture = pRenderer->CreateTexture2d(
                textureSheetWidth,
                textureSheetHeight,
                1,
                format,
                RENDERER_BUFFER_USAGE_STATIC );
            if( !pTexture )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Font::BeginPrecacheResourceData(): Failed to allocate %" ) TPRIu16 TXT( "x%") TPRIu16
                      TXT( " texture for texture sheet %" ) TPRIuFAST8 TXT( " of font \"%s\".\n" ) ),
                    textureSheetWidth,
                    textureSheetHeight,
                    textureIndex,
                    *GetPath().ToString() );
            }
            else
            {
                size_t pitch = 0;
                void* pMappedData = pTexture->Map( 0, pitch );
                HELIUM_ASSERT( pMappedData );

                textureSize = Min( textureSize, pitch * blockRowCount );

                loadId = BeginLoadSubData( pMappedData, textureIndex, textureSize );
                if( IsInvalid( loadId ) )
                {
                    HELIUM_TRACE(
                        TRACE_ERROR,
                        ( TXT( "Font::BeginPrecacheResourceData(): Failed to begin loading texture sheet %" ) TPRIuFAST8
                          TXT( " of font \"%s\".\n" ) ),
                        textureIndex,
                        *GetPath().ToString() );

                    pTexture->Unmap( 0 );
                }
            }
        }

        m_pspTextures[ textureIndex ] = pTexture;
        m_pTextureLoadIds[ textureIndex ] = loadId;
    }

    return true;
}

/// @copydoc GameObject::TryFinishPrecacheResourceData()
bool Font::TryFinishPrecacheResourceData()
{
    uint_fast8_t textureCount = m_textureCount;
    HELIUM_ASSERT( m_pspTextures || textureCount == 0 );

    bool bLoadComplete = true;
    for( uint_fast8_t textureIndex = 0; textureIndex < textureCount; ++textureIndex )
    {
        size_t& rLoadId = m_pTextureLoadIds[ textureIndex ];
        if( IsValid( rLoadId ) )
        {
            if( TryFinishLoadSubData( rLoadId ) )
            {
                RTexture2d* pTexture = m_pspTextures[ textureIndex ];
                HELIUM_ASSERT( pTexture );
                pTexture->Unmap( 0 );

                SetInvalid( rLoadId );
            }
            else
            {
                bLoadComplete = false;
            }
        }
    }

    return bLoadComplete;
}

/// @copydoc Resource::SerializePersistentResourceData()
void Font::SerializePersistentResourceData( Serializer& s )
{
    s << m_ascender;
    s << m_descender;
    s << m_height;
    s << m_maxAdvance;

    s << m_characterCount;
    s << m_textureCount;

    uint_fast32_t characterCount = m_characterCount;
    uint_fast8_t textureCount = m_textureCount;

    if( s.GetMode() == Serializer::MODE_LOAD )
    {
        delete [] m_pCharacters;
        m_pCharacters = NULL;

        delete [] m_pspTextures;
        m_pspTextures = NULL;

        delete [] m_pTextureLoadIds;
        m_pTextureLoadIds = NULL;

        if( characterCount != 0 )
        {
            m_pCharacters = new Character [ characterCount ];
            HELIUM_ASSERT( m_pCharacters );
        }

        if( textureCount != 0 )
        {
            m_pspTextures = new RTexture2dPtr [ textureCount ];
            HELIUM_ASSERT( m_pspTextures );

            if( m_pspTextures )
            {
                m_pTextureLoadIds = new size_t [ textureCount ];
                HELIUM_ASSERT( m_pTextureLoadIds );

                if( m_pTextureLoadIds )
                {
                    MemorySet(
                        m_pTextureLoadIds,
                        0xff,
                        static_cast< size_t >( textureCount ) * sizeof( m_pTextureLoadIds[ 0 ] ) );
                }
                else
                {
                    delete [] m_pspTextures;
                    m_pspTextures = NULL;
                }
            }

            if( !m_pspTextures )
            {
                m_textureCount = 0;
            }
        }
    }

    if( m_pCharacters )
    {
        for( uint_fast32_t characterIndex = 0; characterIndex < characterCount; ++characterIndex )
        {
            m_pCharacters[ characterIndex ].Serialize( s );
        }
    }
    else
    {
        Character dummy;
        for( uint_fast32_t characterIndex = 0; characterIndex < characterCount; ++characterIndex )
        {
            dummy.Serialize( s );
        }

        m_characterCount = 0;
    }
}

/// @copydoc Resource::GetCacheName()
Name Font::GetCacheName() const
{
    static Name cacheName( TXT( "Font" ) );

    return cacheName;
}

/// Serialize this struct.
///
/// @param[in] s  Serializer with which to serialize.
void Font::Character::Serialize( Serializer& s )
{
    s << codePoint;

    s << imageX;
    s << imageY;
    s << imageWidth;
    s << imageHeight;

    s << width;
    s << height;
    s << bearingX;
    s << bearingY;
    s << advance;

    s << texture;
}
