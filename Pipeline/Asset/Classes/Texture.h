#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"

#include "Pipeline/Asset/TextureEnums.h"
#include "Pipeline/Image/MipSet.h"

namespace Helium
{
    namespace Asset
    {
        class TextureFormat
        {
        public:
            enum Enum
            {
                DXT1        = TextureColorFormat::DXT1,
                DXT5        = TextureColorFormat::DXT5,
                ARGB4444    = TextureColorFormat::ARGB4444,
                ARGB8888    = TextureColorFormat::ARGB8888,
                AL88        = TextureColorFormat::AL88,
                RGB565      = TextureColorFormat::RGB565,
            };
            
            REFLECT_DECLARE_ENUMERATION( TextureFormat );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( DXT1,     TXT( "DXT1" ) );
                info.AddElement( DXT5,     TXT( "DXT5" ) );
                info.AddElement( ARGB4444, TXT( "ARGB4444" ) );
                info.AddElement( ARGB8888, TXT( "ARGB8888" ) );
                info.AddElement( AL88,     TXT( "AL88" ) );
                info.AddElement( RGB565,   TXT( "RGB565" ) );
            }
        };

        class TextureFilter
        {
        public:
            enum Enum
            {
                Point       = FILTER_POINT_SELECT_MIP,
                Bilinear    = FILTER_LINEAR_SELECT_MIP,
                Trilinear   = FILTER_LINEAR_LINEAR_MIP,
            };

            REFLECT_DECLARE_ENUMERATION( TextureFilter );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( Point,     TXT( "Point" ) );
                info.AddElement( Bilinear,  TXT( "Bilinear" ) );
                info.AddElement( Trilinear, TXT( "Trilinear" ) );
            }
        };

        class TextureCoordinateWrapMode
        {
        public:
            enum Enum
            {
                Wrap,
                Clamp,
            };

            REFLECT_DECLARE_ENUMERATION( TextureCoordinateWrapMode );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( Wrap,  TXT( "Wrap" ),  TXT( "Wrap" ) );
                info.AddElement( Clamp, TXT( "Clamp" ), TXT( "Clamp" ) );
            }
        };

        class PIPELINE_API Texture : public AssetClass
        {
        private:
            TextureFormat               m_Format;
            TextureFilter               m_Filter;
            TextureCoordinateWrapMode   m_WrapModeU;
            TextureCoordinateWrapMode   m_WrapModeV;

        public:
            REFLECT_DECLARE_CLASS( Texture, AssetClass );

            static void EnumerateClass( Reflect::Compositor< Texture >& comp );

        public:
            Texture()
                : m_Format( TextureFormat::ARGB8888 )
                , m_Filter( TextureFilter::Point )
                , m_WrapModeU( TextureCoordinateWrapMode::Wrap )
                , m_WrapModeV( TextureCoordinateWrapMode::Wrap )
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

            TextureCoordinateWrapMode GetWrapModeU() const
            {
                return m_WrapModeU;
            }
            void SetWrapModeU( const TextureCoordinateWrapMode& wrapMode )
            {
                m_WrapModeU = wrapMode;
            }

            TextureCoordinateWrapMode GetWrapModeV() const
            {
                return m_WrapModeV;
            }
            void SetWrapModeV( const TextureCoordinateWrapMode& wrapMode )
            {
                m_WrapModeV = wrapMode;
            }
        };

        typedef Helium::StrongPtr< Texture > TexturePtr;
        typedef std::vector< TexturePtr > V_Texture;
    }
}