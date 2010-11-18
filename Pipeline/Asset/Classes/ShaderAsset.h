#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Classes/Texture.h"

#include "Foundation/TUID.h"

namespace Helium
{
    namespace Asset
    {
        class AlphaType
        {
        public:
            enum Enum
            {
                Opaque,
                Overlay,
                Additive,
                Scunge,
                CutOut,
                SoftEdge,
                Blended,
            };

            REFLECT_DECLARE_ENUMERATION( AlphaType );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement( Opaque,    TXT( "Opaque" ) );
                info.AddElement( Overlay,   TXT( "Overlay" ) );
                info.AddElement( Additive,  TXT( "Additive" ) );
                info.AddElement( Scunge,    TXT( "Scunge" ) );
                info.AddElement( CutOut,    TXT( "CutOut" ) );
                info.AddElement( SoftEdge,  TXT( "SoftEdge" ) );
                info.AddElement( Blended,   TXT( "Blended" ) );
            }
        };

        // Base class for all shader types
        class PIPELINE_API ShaderAsset : public AssetClass
        {
        public:
            Helium::Path m_ColorMapPath;
            Helium::Path m_NormalMapPath;
            Helium::Path m_GPIMapPath;

            bool          m_EnableColorMapTint;
            Color3  m_ColorMapTint;

            float32_t m_NormalMapScaling;

            bool m_GlossMapEnabled;
            float32_t  m_GlossMapScaling;
            Color3 m_GlossMapTint;
            float32_t  m_GlossMapDirtiness;
            float32_t  m_SpecularPower;

            bool m_ParallaxMapEnabled;
            float32_t  m_ParallaxMapScaling;

            bool m_IncandescentMapEnabled;
            float32_t  m_IncandescentMapScaling;

            bool m_DoubleSided;
            TextureCoordinateWrapMode m_WrapModeU;
            TextureCoordinateWrapMode m_WrapModeV;
            AlphaType m_AlphaMode;

            ShaderAsset()
                : m_EnableColorMapTint( false )
                , m_NormalMapScaling( 1.0f )
                , m_GlossMapEnabled( false )
                , m_GlossMapScaling( 1.0f )
                , m_GlossMapDirtiness( 0.0f )
                , m_SpecularPower( 1.0f )
                , m_ParallaxMapEnabled( false )
                , m_ParallaxMapScaling( 1.0f )
                , m_IncandescentMapEnabled( false )
                , m_IncandescentMapScaling( 1.0f )
                , m_DoubleSided( false )
                , m_WrapModeU( TextureCoordinateWrapMode::Wrap )
                , m_WrapModeV( TextureCoordinateWrapMode::Wrap )
                , m_AlphaMode ( AlphaType::Opaque )
            {
            }

            REFLECT_DECLARE_CLASS( ShaderAsset, AssetClass );

            static void EnumerateClass( Reflect::Compositor<ShaderAsset>& comp );
        };

        typedef Helium::SmartPtr< ShaderAsset > ShaderAssetPtr;
        typedef std::vector< ShaderAssetPtr > V_ShaderAsset;
    }
}