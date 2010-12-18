#pragma once

#include "Pipeline/API.h"
#include "Foundation/Component/Component.h"

namespace Helium
{
    namespace Asset
    {
        static uint64_t s_MaxTextureResolution;

        class TextureDataFormat
        {
        public:
            enum Enum
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
            
            REFLECT_DECLARE_ENUMERATION( TextureDataFormat );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( DXT1,   TXT( "DXT1" ) );
                info.AddElement( DXT5,   TXT( "DXT5" ) );
                info.AddElement( RGB16,  TXT( "RGB16" ) );
                info.AddElement( RGB24,  TXT( "RGB24" ) );
                info.AddElement( RGB32,  TXT( "RGB32" ) );
                info.AddElement( RGBA16, TXT( "RGBA16" ) );
                info.AddElement( RGBA24, TXT( "RGBA24" ) );
                info.AddElement( RGBA32, TXT( "RGBA32" ) );
                info.AddElement( Alpha8, TXT( "Alpha8" ) );
            }
        };

        class PIPELINE_API TextureProcessingComponent : public Component::ComponentBase
        {
        private:
            uint64_t            m_MaxTextureResolution;
            TextureDataFormat   m_TextureDataFormat;
            bool                m_GenerateMipMaps;
            bool                m_GenerateBumpMap;

        public:

            REFLECT_DECLARE_CLASS( TextureProcessingComponent, Component::ComponentBase );

            static void AcceptCompositeVisitor( Reflect::Composite& comp );

        public:
            TextureProcessingComponent()
                : m_MaxTextureResolution( s_MaxTextureResolution >> 2 )
                , m_TextureDataFormat( TextureDataFormat::RGBA32 )
                , m_GenerateMipMaps( false )
                , m_GenerateBumpMap( false )
            {
            }

            virtual ~TextureProcessingComponent()
            {
            }

        public:
            virtual Component::ComponentUsage GetComponentUsage() const HELIUM_OVERRIDE;

        public:

            uint64_t GetMaxTextureResolution() const
            {
                return m_MaxTextureResolution;
            }
            void SetMaxTextureResolution( uint64_t maxTextureResolution )
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

		typedef Helium::StrongPtr< TextureProcessingComponent > TextureProcessingComponentPtr;
    }
}