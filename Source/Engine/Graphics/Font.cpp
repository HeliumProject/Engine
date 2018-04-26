#include "Precompile.h"
#include "Graphics/Font.h"

#include "Rendering/RendererUtil.h"
#include "Rendering/Renderer.h"

#include "Reflect/TranslatorDeduction.h"

HELIUM_IMPLEMENT_ASSET( Helium::Font, Graphics, 0 );  // We allow templating of fonts to generate resources for different font sizes.
HELIUM_DEFINE_ENUM( Helium::Font::ECompression );
HELIUM_DEFINE_BASE_STRUCT( Helium::Font::Character );
HELIUM_DEFINE_CLASS( Helium::Font::PersistentResourceData );

using namespace Helium;

const Font::ECompression::Enum Font::DEFAULT_TEXTURE_COMPRESSION = Font::ECompression::COLOR_COMPRESSED;

void Font::Character::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &Character::codePoint,       "codePoint" );
    comp.AddField( &Character::imageX,          "imageX" );
    comp.AddField( &Character::imageY,          "imageY" );
    comp.AddField( &Character::imageWidth,      "imageWidth" );
    comp.AddField( &Character::imageHeight,     "imageHeight" );
    comp.AddField( &Character::width,           "width" );
    comp.AddField( &Character::height,          "height" );
    comp.AddField( &Character::bearingX,        "bearingX" );
    comp.AddField( &Character::bearingY,        "bearingY" );
    comp.AddField( &Character::advance,         "advance" );
    comp.AddField( &Character::texture,         "texture" );
}

Font::PersistentResourceData::PersistentResourceData()
: m_ascender( 0 )
, m_descender( 0 )
, m_height( 0 )
, m_maxAdvance( 0 )
, m_pspTextures( NULL )
, m_pTextureLoadIds( NULL )
, m_textureCount( 0 )
{

}

void Font::PersistentResourceData::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &PersistentResourceData::m_ascender,         "m_ascender" );
    comp.AddField( &PersistentResourceData::m_descender,        "m_descender" );
    comp.AddField( &PersistentResourceData::m_height,           "m_height" );
    comp.AddField( &PersistentResourceData::m_maxAdvance,       "m_maxAdvance" );
    comp.AddField( &PersistentResourceData::m_characters,       "m_characters" );
    comp.AddField( &PersistentResourceData::m_textureCount,     "m_textureCount" );
}

/// Constructor.
Font::Font()
    : m_pointSize( static_cast< float32_t >( DEFAULT_POINT_SIZE ) )
    , m_dpi( DEFAULT_DPI )
    , m_textureSheetWidth( DEFAULT_TEXTURE_SHEET_WIDTH )
    , m_textureSheetHeight( DEFAULT_TEXTURE_SHEET_HEIGHT )
    , m_textureCompression( DEFAULT_TEXTURE_COMPRESSION )
    , m_bAntialiased( true )
{
}

/// Destructor.
Font::~Font()
{
    //delete [] m_pCharacters;
    delete [] m_persistentResourceData.m_pspTextures;
    delete [] m_persistentResourceData.m_pTextureLoadIds;
}

void Font::PopulateMetaType( Reflect::MetaStruct& comp )
{
    comp.AddField( &Font::m_pointSize,            "m_pointSize" );
    comp.AddField( &Font::m_dpi,                  "m_dpi" );
    comp.AddField( &Font::m_textureSheetWidth,    "m_textureSheetWidth" );
    comp.AddField( &Font::m_textureSheetHeight,   "m_textureSheetHeight" );
    comp.AddField( &Font::m_textureCompression,   "m_textureCompression" );
    comp.AddField( &Font::m_bAntialiased,         "m_bAntialiased" );
}

/// @copydoc Asset::NeedsPrecacheResourceData()
bool Font::NeedsPrecacheResourceData() const
{
    return true;
}

/// @copydoc Asset::BeginPrecacheResourceData()
bool Font::BeginPrecacheResourceData()
{
    uint_fast8_t textureCount = m_persistentResourceData.m_textureCount;
    HELIUM_ASSERT( m_persistentResourceData.m_pspTextures || textureCount == 0 );

    // If we have don't have a renderer, we don't need to load the texture sheets.
    Renderer* pRenderer = Renderer::GetInstance();
    if( !pRenderer )
    {
        for( uint_fast8_t textureIndex = 0; textureIndex < textureCount; ++textureIndex )
        {
            m_persistentResourceData.m_pspTextures[ textureIndex ].Release();
            SetInvalid( m_persistentResourceData.m_pTextureLoadIds[ textureIndex ] );
        }

        return true;
    }

    // Allocate and begin loading texture resources.
    ERendererPixelFormat format =
        ( m_textureCompression == ECompression::COLOR_COMPRESSED ? RENDERER_PIXEL_FORMAT_BC1 : RENDERER_PIXEL_FORMAT_R8 );
    size_t blockRowCount = RendererUtil::PixelToBlockRowCount( m_textureSheetHeight, format );

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
                TraceLevels::Error,
                "Font::BeginPrecacheResourceData(): Unable to locate cached texture data for texture sheet %" PRIuFAST8 " of font \"%s\".\n",
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
                    TraceLevels::Error,
                    "Font::BeginPrecacheResourceData(): Failed to allocate %" PRIu16 "x%" PRIu16 " texture for texture sheet %" PRIuFAST8 " of font \"%s\".\n",
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
                        TraceLevels::Error,
                        "Font::BeginPrecacheResourceData(): Failed to begin loading texture sheet %" PRIuFAST8 " of font \"%s\".\n",
                        textureIndex,
                        *GetPath().ToString() );

                    pTexture->Unmap( 0 );
                }
            }
        }

        m_persistentResourceData.m_pspTextures[ textureIndex ] = pTexture;
        m_persistentResourceData.m_pTextureLoadIds[ textureIndex ] = loadId;
    }

    return true;
}

/// @copydoc Asset::TryFinishPrecacheResourceData()
bool Font::TryFinishPrecacheResourceData()
{
    uint_fast8_t textureCount = m_persistentResourceData.m_textureCount;
    HELIUM_ASSERT( m_persistentResourceData.m_pspTextures || textureCount == 0 );

    bool bLoadComplete = true;
    for( uint_fast8_t textureIndex = 0; textureIndex < textureCount; ++textureIndex )
    {
        size_t& rLoadId = m_persistentResourceData.m_pTextureLoadIds[ textureIndex ];
        if( IsValid( rLoadId ) )
        {
            if( TryFinishLoadSubData( rLoadId ) )
            {
                RTexture2d* pTexture = m_persistentResourceData.m_pspTextures[ textureIndex ];
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

bool Helium::Font::LoadPersistentResourceObject( Reflect::ObjectPtr &_object )
{
    HELIUM_ASSERT(_object.ReferencesObject());
    if (!_object.ReferencesObject())
    {
        return false;
    }

    _object->CopyTo(&m_persistentResourceData);

    uint_fast32_t characterCount = static_cast<uint_fast32_t>(m_persistentResourceData.m_characters.GetSize());
    uint_fast8_t textureCount = m_persistentResourceData.m_textureCount;

    delete [] m_persistentResourceData.m_pspTextures;
    m_persistentResourceData.m_pspTextures = NULL;

    delete [] m_persistentResourceData.m_pTextureLoadIds;
    m_persistentResourceData.m_pTextureLoadIds = NULL;

    if( textureCount != 0 )
    {
        m_persistentResourceData.m_pspTextures = new RTexture2dPtr [ textureCount ];
        HELIUM_ASSERT( m_persistentResourceData.m_pspTextures );

        if( m_persistentResourceData.m_pspTextures )
        {
            m_persistentResourceData.m_pTextureLoadIds = new size_t [ textureCount ];
            HELIUM_ASSERT( m_persistentResourceData.m_pTextureLoadIds );

            if( m_persistentResourceData.m_pTextureLoadIds )
            {
                MemorySet(
                    m_persistentResourceData.m_pTextureLoadIds,
                    0xff,
                    static_cast< size_t >( textureCount ) * sizeof( m_persistentResourceData.m_pTextureLoadIds[ 0 ] ) );
            }
            else
            {
                delete [] m_persistentResourceData.m_pspTextures;
                m_persistentResourceData.m_pspTextures = NULL;
            }
        }

        if( !m_persistentResourceData.m_pspTextures )
        {
            m_persistentResourceData.m_textureCount = 0;
        }
    }

    return true;
}

/// @copydoc Resource::GetCacheName()
Name Font::GetCacheName() const
{
    static Name cacheName( "Font" );

    return cacheName;
}
