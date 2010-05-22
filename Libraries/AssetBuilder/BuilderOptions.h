#pragma once

#include "API.h"

#include "TUID/tuid.h"
#include "Common/Types.h"

#include "Reflect/Serializers.h"

#include "Content/Zone.h"
#include "Content/Region.h"

namespace AssetBuilder
{
    class ASSETBUILDER_API BuilderOptions : public Reflect::Element
    {
    public:
        bool m_SkipNested;

        REFLECT_DECLARE_CLASS( BuilderOptions, Reflect::Element );

        static void EnumerateClass( Reflect::Compositor<BuilderOptions>& comp );

        BuilderOptions()
            : m_SkipNested( false )
        {

        }

        // force a vtable
        virtual ~BuilderOptions()
        {

        }

        virtual std::string GetAffectorString()
        {
            // force and force nested don't change builder output
            return "";
        }
    };
    typedef Nocturnal::SmartPtr< BuilderOptions > BuilderOptionsPtr;

    class ASSETBUILDER_API BuildRequest : public Reflect::Element
    {
    public:
        File::S_Reference m_Assets;
        BuilderOptionsPtr m_Options;

        BuildRequest()
        {

        }

        REFLECT_DECLARE_CLASS( BuildRequest, Reflect::Element );

        static void EnumerateClass( Reflect::Compositor<BuildRequest>& comp );
    };
    typedef Nocturnal::SmartPtr< BuildRequest > BuildRequestPtr;

    class ASSETBUILDER_API ZoneBuilderOptions : public BuilderOptions
    {
    public:
        Content::ZonePtr        m_Zone;
        bool                    m_GameplayOnly; 

        REFLECT_DECLARE_CLASS( ZoneBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<ZoneBuilderOptions>& comp );

        ZoneBuilderOptions() 
            : m_GameplayOnly(false)
        {

        }
    };
    typedef Nocturnal::SmartPtr<ZoneBuilderOptions> ZoneBuilderOptionsPtr;

    class ASSETBUILDER_API LevelBuilderOptions : public BuilderOptions
    {
    public:
        bool        m_NoMobys;
        bool        m_NoConduit;
        bool        m_NoCinematics;
        bool        m_NoFonts;
        bool        m_NoTexStream;
        bool        m_NoReqs;
        bool        m_NoWad;
        bool        m_NoLocalization;

        bool        m_Viewer;

        bool        m_LightmapTweak;
        bool        m_LightmapHigh;

        bool        m_Occlusion;
        bool        m_CollPatch;

        bool        m_PackOnly;
        bool        m_BuildOnly;

        bool        m_SelectZones;

        bool        m_NoNewNav;

        V_tuid      m_Zones;
        V_string    m_ZoneList;

        V_string    m_Regions;

        bool        m_GameplayOnly;
        bool        m_PackAllRegions;

        u32         m_LightmapTweakSubset;      // 0 - pack all, 1 - pack first half of lightmaps in tweak formap, 2 - pack second half

        std::string m_Language;

        bool        m_PatchAssets;
        std::string m_PatchRefDir;

        REFLECT_DECLARE_CLASS( LevelBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<LevelBuilderOptions>& comp );

        LevelBuilderOptions()
            : m_NoMobys( false )
            , m_NoConduit( false )
            , m_NoCinematics( false )
            , m_NoFonts( false )
            , m_NoReqs( false )
            , m_NoWad( false )
            , m_NoLocalization( false )
            , m_NoTexStream( false )
            , m_Viewer( false )
            , m_LightmapTweak( false )
            , m_LightmapHigh( true )
            , m_Occlusion( false )
            , m_CollPatch( false )
            , m_PackOnly( false )
            , m_BuildOnly ( false )
            , m_SelectZones( false )
            , m_GameplayOnly( false )
            , m_PackAllRegions( false )
            , m_NoNewNav (false)
            , m_LightmapTweakSubset( 0 )
            , m_Language( "english" )
            , m_PatchAssets( false )
        {

        }
    };
    typedef Nocturnal::SmartPtr< LevelBuilderOptions > LevelBuilderOptionsPtr;

    class ASSETBUILDER_API RegionBuilderOptions : public BuilderOptions
    {
    public:
        std::string             m_RegionName;
        Content::RegionPtr      m_Region; 
        Content::V_Zone         m_Zones;

        // something of a hack
        LevelBuilderOptionsPtr  m_LevelBuilderOptions;

        REFLECT_DECLARE_CLASS( RegionBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<RegionBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<RegionBuilderOptions> RegionBuilderOptionsPtr;

    class ASSETBUILDER_API ShrubBuilderOptions : public BuilderOptions
    {
    public:
        REFLECT_DECLARE_CLASS( ShrubBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<ShrubBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<ShrubBuilderOptions> ShrubBuilderOptionsPtr;

    class ASSETBUILDER_API TieBuilderOptions : public BuilderOptions
    {
        REFLECT_DECLARE_CLASS( TieBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<TieBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<TieBuilderOptions> TieBuilderOptionsPtr;

    class ASSETBUILDER_API UfragBuilderOptions : public BuilderOptions
    {
        REFLECT_DECLARE_CLASS( UfragBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<UfragBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<UfragBuilderOptions> UfragBuilderOptionsPtr;

    class ASSETBUILDER_API ShaderBuilderOptions : public BuilderOptions
    {
    public:
        std::string m_DumpPath;
        bool m_Verbose;

        ShaderBuilderOptions()
            : m_Verbose( false )
        {

        }

        REFLECT_DECLARE_CLASS( ShaderBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<ShaderBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<ShaderBuilderOptions> ShaderBuilderOptionsPtr;

    class ASSETBUILDER_API TexturePackBuilderOptions : public BuilderOptions
    {
        REFLECT_DECLARE_CLASS( TexturePackBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<TexturePackBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<TexturePackBuilderOptions> TexturePackBuilderOptionsPtr;

    class ASSETBUILDER_API FoliageBuilderOptions : public BuilderOptions
    {
        REFLECT_DECLARE_CLASS( FoliageBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<FoliageBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<FoliageBuilderOptions> FoliageBuilderOptionsPtr;

    class ASSETBUILDER_API FontBuilderOptions : public BuilderOptions
    {
        REFLECT_DECLARE_CLASS( FontBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<FontBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<FontBuilderOptions> FontBuilderOptionsPtr;

    class ASSETBUILDER_API MobyBuilderOptions : public BuilderOptions
    {
    public:
        bool        m_Conduit;
        bool        m_AnimDump;
        std::string m_AnimDumpFilename;
        bool        m_DumpJoints;

        REFLECT_DECLARE_CLASS( MobyBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<MobyBuilderOptions>& comp );

        MobyBuilderOptions()
            : m_Conduit( false )
            , m_AnimDump(false)
            , m_DumpJoints( false )
        {

        }
    };
    typedef Nocturnal::SmartPtr<MobyBuilderOptions> MobyBuilderOptionsPtr;

    class ASSETBUILDER_API MovieBuilderOptions : public BuilderOptions
    {
        REFLECT_DECLARE_CLASS( MovieBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<MovieBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<MovieBuilderOptions> MovieBuilderOptionsPtr;

    class ASSETBUILDER_API AnimationBuilderOptions : public BuilderOptions
    {
    public:
        bool m_DumpAnims;
        bool m_DumpJoints;

        AnimationBuilderOptions()
            : m_DumpAnims( false )
            , m_DumpJoints( false )
        {

        }

        REFLECT_DECLARE_CLASS( AnimationBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<AnimationBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<AnimationBuilderOptions> AnimationBuilderOptionsPtr;

    class ASSETBUILDER_API SkyBuilderOptions : public BuilderOptions
    {
        REFLECT_DECLARE_CLASS( SkyBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<SkyBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<SkyBuilderOptions> SkyBuilderOptionsPtr;

    class ASSETBUILDER_API CinematicBuilderOptions : public BuilderOptions
    {
        REFLECT_DECLARE_CLASS( CinematicBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<CinematicBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<CinematicBuilderOptions> CinematicBuilderOptionsPtr;

    class ASSETBUILDER_API CubeMapBuilderOptions : public BuilderOptions
    {
        REFLECT_DECLARE_CLASS( CubeMapBuilderOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<CubeMapBuilderOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<CubeMapBuilderOptions> CubeMapBuilderOptionsPtr;

    class ASSETBUILDER_API SymbolEnumAutoFixupOptions : public BuilderOptions
    {
    public:
        std::string             m_InputFile;
        std::string             m_InputFileSpecName;

        std::string             m_ManifestFile;
        std::string             m_ManifestFileSpecName;

        std::string             m_OutputFile;

        REFLECT_DECLARE_CLASS( SymbolEnumAutoFixupOptions, BuilderOptions );

        static void EnumerateClass( Reflect::Compositor<SymbolEnumAutoFixupOptions>& comp );
    };
    typedef Nocturnal::SmartPtr<SymbolEnumAutoFixupOptions> SymbolEnumAutoFixupOptionsPtr;
}
