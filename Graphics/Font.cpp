#include "GraphicsPch.h"
#include "Graphics/Font.h"

#include "Rendering/RendererUtil.h"
#include "Rendering/Renderer.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;

REFLECT_DEFINE_ENUMERATION( Font::ECompression );

HELIUM_IMPLEMENT_OBJECT( Font, Graphics, 0 );  // We allow templating of fonts to generate resources for different font sizes.

REFLECT_DEFINE_BASE_STRUCTURE( Font::Character );

void Font::Character::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &Character::codePoint,       TXT( "codePoint" ) );
    comp.AddField( &Character::imageX,          TXT( "imageX" ) );
    comp.AddField( &Character::imageY,          TXT( "imageY" ) );
    comp.AddField( &Character::imageWidth,      TXT( "imageWidth" ) );
    comp.AddField( &Character::imageHeight,     TXT( "imageHeight" ) );
    comp.AddField( &Character::width,           TXT( "width" ) );
    comp.AddField( &Character::height,          TXT( "height" ) );
    comp.AddField( &Character::bearingX,        TXT( "bearingX" ) );
    comp.AddField( &Character::bearingY,        TXT( "bearingY" ) );
    comp.AddField( &Character::advance,         TXT( "advance" ) );
    comp.AddField( &Character::texture,         TXT( "texture" ) );
}


REFLECT_DEFINE_OBJECT( Font::PersistentResourceData );

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

void Font::PersistentResourceData::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &PersistentResourceData::m_ascender,         TXT( "m_ascender" ) );
    comp.AddField( &PersistentResourceData::m_descender,        TXT( "m_descender" ) );
    comp.AddField( &PersistentResourceData::m_height,           TXT( "m_height" ) );
    comp.AddField( &PersistentResourceData::m_maxAdvance,       TXT( "m_maxAdvance" ) );
    comp.AddStructureField( &PersistentResourceData::m_characters,       TXT( "m_characters" ) );
    comp.AddField( &PersistentResourceData::m_textureCount,     TXT( "m_textureCount" ) );
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

void Font::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &Font::m_pointSize,            TXT( "m_pointSize" ) );
    comp.AddField( &Font::m_dpi,                  TXT( "m_dpi" ) );
    comp.AddField( &Font::m_textureSheetWidth,    TXT( "m_textureSheetWidth" ) );
    comp.AddField( &Font::m_textureSheetHeight,   TXT( "m_textureSheetHeight" ) );
    comp.AddEnumerationField( &Font::m_textureCompression,   TXT( "m_textureCompression" ) );
    comp.AddField( &Font::m_bAntialiased,         TXT( "m_bAntialiased" ) );
}

/// @copydoc GameObject::NeedsPrecacheResourceData()
bool Font::NeedsPrecacheResourceData() const
{
    return true;
}

/// @copydoc GameObject::BeginPrecacheResourceData()
bool Font::BeginPrecacheResourceData()
{
    uint_fast8_t textureCount = m_persistentResourceData.m_textureCount;
    HELIUM_ASSERT( m_persistentResourceData.m_pspTextures || textureCount == 0 );

    // If we have don't have a renderer, we don't need to load the texture sheets.
    Renderer* pRenderer = Renderer::GetStaticInstance();
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

        m_persistentResourceData.m_pspTextures[ textureIndex ] = pTexture;
        m_persistentResourceData.m_pTextureLoadIds[ textureIndex ] = loadId;
    }

    return true;
}

/// @copydoc GameObject::TryFinishPrecacheResourceData()
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
    static Name cacheName( TXT( "Font" ) );

    return cacheName;
}
