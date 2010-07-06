#pragma once

#include "Pipeline/API.h"
#include "Foundation/Component/Component.h"

namespace Asset
{
    static u64 s_MaxTextureResolution;

    namespace TextureDataFormats
    {
        enum TextureDataFormat
        {
            DXT1,
            DXT5,
            RGB16,
            RGB24,
            RGB32,
            RGBA16,
            RGBA24,
            RGBA32,
            Alpha8
        };
        static void TextureDataFormatEnumerateEnumeration( Reflect::Enumeration* info )
        {
            info->AddElement(DXT1,   TXT( "DXT1" ),   TXT( "DXT1" ) );
            info->AddElement(DXT5,   TXT( "DXT5" ),   TXT( "DXT5" ) );
            info->AddElement(RGB16,  TXT( "RGB16" ),  TXT( "RGB16" ) );
            info->AddElement(RGB24,  TXT( "RGB24" ),  TXT( "RGB24" ) );
            info->AddElement(RGB32,  TXT( "RGB32" ),  TXT( "RGB32" ) );
            info->AddElement(RGBA16, TXT( "RGBA16" ), TXT( "RGBA16" ) );
            info->AddElement(RGBA24, TXT( "RGBA24" ), TXT( "RGBA24" ) );
            info->AddElement(RGBA32, TXT( "RGBA32" ), TXT( "RGBA32" ) );
            info->AddElement(Alpha8, TXT( "Alpha8" ), TXT( "Alpha8" ) );
        }
    }
    typedef TextureDataFormats::TextureDataFormat TextureDataFormat;

    class PIPELINE_API TextureProcessingComponent : public Component::ComponentBase
    {
    private:
        u64               m_MaxTextureResolution;
        TextureDataFormat m_TextureDataFormat;
        bool              m_GenerateMipMaps;
        bool              m_GenerateBumpMap;

    public:

        REFLECT_DECLARE_CLASS( TextureProcessingComponent, Component::ComponentBase );

        static void EnumerateClass( Reflect::Compositor< TextureProcessingComponent >& comp );

    public:
        TextureProcessingComponent()
            : m_MaxTextureResolution( s_MaxTextureResolution >> 2 )
            , m_TextureDataFormat( TextureDataFormats::RGBA32 )
            , m_GenerateMipMaps( false )
            , m_GenerateBumpMap( false )
        {
        }

        virtual ~TextureProcessingComponent()
        {
        }

    public:
        virtual Component::ComponentUsage GetComponentUsage() const NOC_OVERRIDE;

    public:

        u64 GetMaxTextureResolution() const
        {
            return m_MaxTextureResolution;
        }
        void SetMaxTextureResolution( u64 maxTextureResolution )
        {
            m_MaxTextureResolution = maxTextureResolution;
        }

        TextureDataFormat GetTextureDataFormat() const
        {
            return m_TextureDataFormat;
        }
        void SetTextureDataFormat( const TextureDataFormat& textureDataFormat )
        {
            m_TextureDataFormat = textureDataFormat;
        }

        bool GetGenerateMipMaps() const
        {
            return m_GenerateMipMaps;
        }
        void SetGenerateMipMaps( bool generateMipMaps )
        {
            m_GenerateMipMaps = generateMipMaps;
        }

        bool GetGenerateBumpMap() const
        {
            return m_GenerateBumpMap;
        }
        void SetGenerateBumpMap( bool generateBumpMap )
        {
            m_GenerateBumpMap = generateBumpMap;
        }
    };
}
