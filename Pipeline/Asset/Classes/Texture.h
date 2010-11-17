#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"

#include "Pipeline/Asset/TextureEnums.h"
#include "Pipeline/Image/MipSet.h"

namespace Helium
{
    namespace Asset
    {
        namespace TextureFormats
        {
            enum TextureFormat
            {
                DXT1     = TextureColorFormats::DXT1,
                DXT5     = TextureColorFormats::DXT5,
                ARGB4444 = TextureColorFormats::ARGB4444,
                ARGB8888 = TextureColorFormats::ARGB8888,
                AL88     = TextureColorFormats::AL88,
                RGB565   = TextureColorFormats::RGB565,
            };
            static void TextureFormatEnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( DXT1,     TXT( "DXT1" ),     TXT( "DXT1" ) );
                info.AddElement( DXT5,     TXT( "DXT5" ),     TXT( "DXT5" ) );
                info.AddElement( ARGB4444, TXT( "ARGB4444" ), TXT( "ARGB4444" ) );
                info.AddElement( ARGB8888, TXT( "ARGB8888" ), TXT( "ARGB8888" ) );
                info.AddElement( AL88,     TXT( "AL88" ),     TXT( "AL88" ) );
                info.AddElement( RGB565,   TXT( "RGB565" ),   TXT( "RGB565" ) );
            }
        }
        typedef TextureFormats::TextureFormat TextureFormat;

        namespace TextureFilters
        {
            enum TextureFilter
            {
                Point = Helium::FILTER_POINT_SELECT_MIP,
                Bilinear = Helium::FILTER_LINEAR_SELECT_MIP,
                Trilinear = Helium::FILTER_LINEAR_LINEAR_MIP,
            };
            static void TextureFilterEnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( Point,     TXT( "Point" ),     TXT( "Point" ) );
                info.AddElement( Bilinear,  TXT( "Bilinear" ),  TXT( "Bilinear" ) );
                info.AddElement( Trilinear, TXT( "Trilinear" ), TXT( "Trilinear" ) );
            }
        }
        typedef TextureFilters::TextureFilter TextureFilter;

        namespace WrapModes
        {
            enum WrapMode
            {
                Wrap,
                Clamp,
            };
            static void WrapModeEnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( Wrap,  TXT( "Wrap" ),  TXT( "Wrap" ) );
                info.AddElement( Clamp, TXT( "Clamp" ), TXT( "Clamp" ) );
            }
        }
        typedef WrapModes::WrapMode WrapMode;

        class PIPELINE_API Texture : public AssetClass
        {
        private:
            TextureFormat   m_Format;
            TextureFilter   m_Filter;
            WrapMode        m_WrapModeU;
            WrapMode        m_WrapModeV;

        public:
            REFLECT_DECLARE_CLASS( Texture, AssetClass );

            static void EnumerateClass( Reflect::Compositor< Texture >& comp );

        public:
            Texture()
                : m_Format( TextureFormats::ARGB8888 )
                , m_Filter( TextureFilters::Point )
                , m_WrapModeU( WrapModes::Wrap )
                , m_WrapModeV( WrapModes::Wrap )
            {
            }

            TextureFormat GetFormat() const
            {
                return m_Format;
            }
            void SetFormat( const TextureFormat& format )
            {
                m_Format = format;
            }

            TextureFilter GetFilter() const
            {
                return m_Filter;
            }
            void SetFilter( const TextureFilter& filter )
            {
                m_Filter = filter;
            }

            WrapMode GetWrapModeU() const
            {
                return m_WrapModeU;
            }
            void SetWrapModeU( const WrapMode& wrapMode )
            {
                m_WrapModeU = wrapMode;
            }

            WrapMode GetWrapModeV() const
            {
                return m_WrapModeV;
            }
            void SetWrapModeV( const WrapMode& wrapMode )
            {
                m_WrapModeV = wrapMode;
            }
        };

        typedef Helium::SmartPtr< Texture > TexturePtr;
        typedef std::vector< TexturePtr > V_Texture;
    }
}