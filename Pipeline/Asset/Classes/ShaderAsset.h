#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Classes/Texture.h"

#include "Foundation/TUID.h"

namespace Helium
{
    namespace Asset
    {
        namespace AlphaTypes
        {
            enum AlphaType
            {
                ALPHA_OPAQUE,
                ALPHA_OVERLAY,
                ALPHA_ADDITIVE,
                ALPHA_SCUNGE,
                ALPHA_CUTOUT,
                ALPHA_SOFT_EDGE,
                ALPHA_BLENDED,
            };
            static void AlphaTypeEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement(ALPHA_OPAQUE, TXT( "ALPHA_OPAQUE" ), TXT( "OPAQUE" ) );
                info->AddElement(ALPHA_OVERLAY, TXT( "ALPHA_OVERLAY" ), TXT( "OVERLAY" ) );
                info->AddElement(ALPHA_ADDITIVE, TXT( "ALPHA_ADDITIVE" ), TXT( "ADDITIVE" ) );
                info->AddElement(ALPHA_SCUNGE, TXT( "ALPHA_SCUNGE" ), TXT( "SCUNGE" ) );
                info->AddElement(ALPHA_CUTOUT, TXT( "ALPHA_CUTOUT" ), TXT( "CUTOUT" ) );
                info->AddElement(ALPHA_SOFT_EDGE, TXT( "ALPHA_SOFT_EDGE" ), TXT( "SOFT_EDGE" ) );
                info->AddElement(ALPHA_BLENDED, TXT( "ALPHA_BLENDED" ), TXT( "BLENDED" ) );
            }
        }
        typedef AlphaTypes::AlphaType AlphaType;


        // WetSurface Type
        namespace WetSurfaceTypes
        {
            enum WetSurfaceType
            {
                WET_SURFACE_NONE, 
                WET_SURFACE_SKIN,
                WET_SURFACE_DIRT,
                WET_SURFACE_CLOTH, 
                WET_SURFACE_BRICK, 
                WET_SURFACE_FOLIAGE, 
                WET_SURFACE_GUN,
                WET_SURFACE_METAL,
            };

            static void WetSurfaceTypeEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement(WET_SURFACE_NONE,     TXT( "WET_SURFACE_NONE" ),        TXT( "None" ) );
                info->AddElement(WET_SURFACE_SKIN,     TXT( "WET_SURFACE_SKIN" ),        TXT( "Skin" ) );
                info->AddElement(WET_SURFACE_DIRT,     TXT( "WET_SURFACE_DIRT" ),        TXT( "Dirt" ) );
                info->AddElement(WET_SURFACE_CLOTH,    TXT( "WET_SURFACE_CLOTH" ),       TXT( "Cloth" ) );
                info->AddElement(WET_SURFACE_BRICK,    TXT( "WET_SURFACE_BRICK" ),       TXT( "Brick" ) );
                info->AddElement(WET_SURFACE_FOLIAGE,  TXT( "WET_SURFACE_FOLIAGE" ),     TXT( "Foliage" ) );
                info->AddElement(WET_SURFACE_GUN,      TXT( "WET_SURFACE_GUN" ),         TXT( "Gun" ) );
                info->AddElement(WET_SURFACE_METAL,    TXT( "WET_SURFACE_METAL" ),       TXT( "Metal" ) );
            }
        }
        typedef WetSurfaceTypes::WetSurfaceType WetSurfaceType;

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
            WrapMode m_WrapModeU;
            WrapMode m_WrapModeV;
            AlphaType m_AlphaMode;
            WetSurfaceType m_WetSurfaceMode;

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
                , m_WrapModeU( WrapModes::Wrap )
                , m_WrapModeV( WrapModes::Wrap )
                , m_AlphaMode ( AlphaTypes::ALPHA_OPAQUE )
                , m_WetSurfaceMode( WetSurfaceTypes::WET_SURFACE_BRICK)
            {
            }

            REFLECT_DECLARE_ABSTRACT( ShaderAsset, AssetClass );

            static void EnumerateClass( Reflect::Compositor<ShaderAsset>& comp );
        };

        typedef Helium::SmartPtr< ShaderAsset > ShaderAssetPtr;
        typedef std::vector< ShaderAssetPtr > V_ShaderAsset;
    }
}