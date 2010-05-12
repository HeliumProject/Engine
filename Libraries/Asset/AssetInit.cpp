#include "AssetInit.h"

#include "ArtFileAttribute.h"
#include "AssetClass.h"
#include "AssetFile.h"
#include "AssetFolder.h"
#include "AssetTemplate.h"
#include "AllowedDirParser.h"
#include "AssetVersion.h"
#include "CacheDB.h"
#include "ColorMapAttribute.h"
#include "DependenciesAttribute.h"
#include "DetailMapAttribute.h"
#include "EngineTypeInfo.h"
#include "Entity.h"
#include "EntityAsset.h"
#include "EntityManifest.h"
#include "ExpensiveMapAttribute.h"
#include "ExporterJob.h"
#include "GraphShaderAsset.h"
#include "LevelAsset.h"
#include "Manager.h"
#include "ManifestVersion.h"
#include "NormalMapAttribute.h"
#include "RequiredListAsset.h"
#include "StandardColorMapAttribute.h"
#include "StandardDetailMapAttribute.h"
#include "StandardExpensiveMapAttribute.h"
#include "StandardNormalMapAttribute.h"
#include "StandardShaderAsset.h"
#include "TextureEntry.h"
#include "TextureMapAttribute.h"
#include "TexturePackBase.h"
#include "TexturePackAsset.h"
#include "WorldFileAttribute.h"
#include "SceneManifest.h"

#include "Attribute/AttributeInit.h"
#include "Attribute/AttributeCategories.h"
#include "Common/InitializerStack.h"
#include "Content/ContentInit.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ExtensionSpecs.h"
#include "Reflect/Registry.h"

#include "Common/InitializerStack.h"

using namespace Reflect;

#define ASSET_BEGIN_REGISTER_ENGINE_TYPES                                       \
  Nocturnal::Insert<M_EngineTypeInfo>::Result et_inserted;

#define ASSET_REGISTER_ENGINETYPE( __EngineTypeName )                           \
  et_inserted = g_EngineTypeInfos.insert( M_EngineTypeInfo::value_type( EngineTypes::__EngineTypeName, EngineTypeInfo( #__EngineTypeName, #__EngineTypeName"Builder.dll" ) ) );

#define _ASSET_REGISTER_ENGINETYPE_SET_MEMBER( __MemberName, __Value )          \
  if ( et_inserted.second ) et_inserted.first->second.__MemberName = __Value;

#define ASSET_REGISTER_ENGINETYPE_BUILDERDLL( __BuilderDLL )                    \
  _ASSET_REGISTER_ENGINETYPE_SET_MEMBER( m_BuilderDLL, __BuilderDLL );

#define ASSET_REGISTER_ENGINETYPE_ICONFILENAME( __IconFilename )                \
  _ASSET_REGISTER_ENGINETYPE_SET_MEMBER( m_IconFilename, __IconFilename );

#define ASSET_REGISTER_ENGINETYPE_TYPECOLOR_ARGB(a,r,g,b)                       \
  _ASSET_REGISTER_ENGINETYPE_SET_MEMBER( m_TypeColor, ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff))) );

#define _ASSET_REGISTER_ENGINETYPE_ENTITYCLASS                                  \
  if ( et_inserted.second && et_inserted.first->second.m_EntityAsset == NULL )  \
  et_inserted.first->second.m_EntityAsset = new EntityAsset();

#define ASSET_REGISTER_ENGINETYPE_ENTITYCLASS_ATTRIB( __Attribute )             \
  _ASSET_REGISTER_ENGINETYPE_ENTITYCLASS                                        \
  if ( et_inserted.second && et_inserted.first->second.m_EntityAsset != NULL )  \
  et_inserted.first->second.m_EntityAsset->SetAttribute( new __Attribute() );

using namespace Asset;

V_i32 Asset::g_AssetClassTypes;

i32 g_InitCount = 0;

Nocturnal::InitializerStack g_InitializerStack;

AllowedDirParser g_AllowedDirParser;

void Asset::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
    g_InitializerStack.Push( Finder::Initialize, Finder::Cleanup );
    g_InitializerStack.Push( File::Initialize, File::Cleanup );
    g_InitializerStack.Push( Attribute::Initialize, Attribute::Cleanup );
    g_InitializerStack.Push( Content::Initialize, Content::Cleanup );
    g_InitializerStack.Push( CacheDB::Initialize, CacheDB::Cleanup );
    g_InitializerStack.Push( Manager::Initialize, Manager::Cleanup );


    // for critial section in cache
    AssetClass::InitializeCache();

    std::string configPath = getenv( NOCTURNAL_STUDIO_PREFIX"PROJECT_CONFIG" );
    configPath += "/AssetCreationAllowedDirs.xml" ;
    if ( !g_AllowedDirParser.Load( configPath ) )
    {
      Console::Warning( "Could not load asset creation allowed directory data\n" );
    }

    //
    // Numbered asset conversion
    //

    g_InitializerStack.Push( Reflect::RegisterClass<ExporterJob>( "ExporterJob" ) );


    //
    // Enums
    //

    g_InitializerStack.Push( Reflect::RegisterEnumeration<RunTimeFilters::RunTimeFilter>( &RunTimeFilters::RunTimeFilterEnumerateEnumeration, "RunTimeFilter" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<WrapModes::WrapMode>( &WrapModes::WrapModeEnumerateEnumeration, "WrapMode" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<AlphaTypes::AlphaType>( &AlphaTypes::AlphaTypeEnumerateEnumeration, "AlphaType" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<WetSurfaceTypes::WetSurfaceType>( &WetSurfaceTypes::WetSurfaceTypeEnumerateEnumeration, "WetSurfaceType" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<MipGenFilterTypes::MipGenFilterType>( &MipGenFilterTypes::MipGenFilterTypeEnumerateEnumeration, "MipGenFilterType" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<PostMipFilterTypes::PostMipFilterType>( &PostMipFilterTypes::PostMipFilterTypeEnumerateEnumeration, "PostMipFilterType" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<ReductionRatios::ReductionRatio>( &ReductionRatios::ReductionRatioEnumerateEnumeration, "ReductionRatio" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<ColorTexFormats::ColorTexFormat>( &ColorTexFormats::ColorTexFormatEnumerateEnumeration, "ColorTexFormat" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<NormalTexFormats::NormalTexFormat>( &NormalTexFormats::NormalTexFormatEnumerateEnumeration, "NormalTexFormat" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<GlossParaIncanTexFormats::GlossParaIncanTexFormat>( &GlossParaIncanTexFormats::GlossParaIncanTexFormatEnumerateEnumeration, "GlossParaIncanTexFormat" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<CubeSpecTypeFormats::CubeSpecTypeFormat>( &CubeSpecTypeFormats::CubeSpecTypeFormatEnumerateEnumeration, "CubeSpecTypeFormat" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<DetailTexFormats::DetailTexFormat>( &DetailTexFormats::DetailTexFormatEnumerateEnumeration, "DetailTexFormat" ) );

    //
    // Basic Types
    //

    g_InitializerStack.Push( Reflect::RegisterClass<AssetTemplate>( "AssetTemplate" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<AssetVersion>( "AssetVersion" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<AssetClass>( "AssetClass" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<AssetFile>( "AssetFile" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<AssetFolder>( "AssetFolder" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<ShaderAsset>( "ShaderAsset" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<TexturePackBase>( "TexturePackBase" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<TextureEntryBase>( "TextureEntryBase" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<FileBackedAttribute>( "FileBackedAttribute" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<Entity>( "Entity" ) );

    g_InitializerStack.Push( Reflect::RegisterClass<ManifestVersion>( "ManifestVersion" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<AssetManifest>( "AssetManifest" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<EntityManifest>( "EntityManifest" ) ); Reflect::Registry::GetInstance()->AliasType( Reflect::GetClass< EntityManifest >(), "AssetManifest" );
    g_InitializerStack.Push( Reflect::RegisterClass<SceneManifest>( "SceneManifest" ) );


    //
    // Misc
    //

    // texture
    g_InitializerStack.Push( Reflect::RegisterEnumeration<TextureWrapModes::TextureWrapMode>( &TextureWrapModes::TextureWrapModeEnumerateEnumeration, "TextureWrapMode" ) );
    g_InitializerStack.Push( Reflect::RegisterEnumeration<TextureColorFormats::TextureColorFormat>( &TextureColorFormats::TextureColorFormatEnumerateEnumeration, "TextureColorFormat" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<StandardTextureEntry>( "StandardTextureEntry" ) );

    // legacy
    g_InitializerStack.Push( Reflect::RegisterClass<TextureEntry>( "TextureEntry" ) );


    //
    // Asset Attributes
    //

    g_InitializerStack.Push( Reflect::RegisterClass<DependenciesAttribute>( "DependenciesAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new DependenciesAttribute );

    g_InitializerStack.Push( Reflect::RegisterClass<TextureMapAttribute>( "TextureMapAttribute" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<ColorMapAttribute>( "ColorMapAttribute" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<NormalMapAttribute>( "NormalMapAttribute" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<ExpensiveMapAttribute>( "ExpensiveMapAttribute" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<DetailMapAttribute>( "DetailMapAttribute" ) );
    g_InitializerStack.Push( Reflect::RegisterClass<StandardColorMapAttribute>( "StandardColorMapAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new StandardColorMapAttribute );
    g_InitializerStack.Push( Reflect::RegisterClass<StandardNormalMapAttribute>( "StandardNormalMapAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new StandardNormalMapAttribute );
    g_InitializerStack.Push( Reflect::RegisterClass<StandardExpensiveMapAttribute>( "StandardExpensiveMapAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new StandardExpensiveMapAttribute );
    g_InitializerStack.Push( Reflect::RegisterClass<StandardDetailMapAttribute>( "StandardDetailMapAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new StandardDetailMapAttribute );

    //
    // Attribute Sets
    //

    // appearance
    g_InitializerStack.Push( Reflect::RegisterClass<ArtFileAttribute>( "ArtFileAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new ArtFileAttribute );
    g_InitializerStack.Push( Reflect::RegisterClass<WorldFileAttribute>( "WorldFileAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new WorldFileAttribute );


    //
    // Asset classes
    //

    g_InitializerStack.Push( Reflect::RegisterClass<EntityAsset>( "EntityAsset" ) );
    g_AssetClassTypes.push_back( Reflect::GetType<EntityAsset>() );
    g_InitializerStack.Push( Reflect::RegisterClass<LevelAsset>( "LevelAsset" ) );
    g_AssetClassTypes.push_back( Reflect::GetType<LevelAsset>() );
    g_InitializerStack.Push( Reflect::RegisterClass<RequiredListAsset>( "RequiredListAsset" ) );
    g_AssetClassTypes.push_back( Reflect::GetType<RequiredListAsset>() );
    g_InitializerStack.Push( Reflect::RegisterClass<TexturePackAsset>( "TexturePackAsset" ) );
    g_AssetClassTypes.push_back( Reflect::GetType<TexturePackAsset>() );

    // Shaders
    g_InitializerStack.Push( Reflect::RegisterClass<StandardShaderAsset>( "StandardShaderAsset" ) );
    g_AssetClassTypes.push_back( Reflect::GetType<StandardShaderAsset>() );
    g_InitializerStack.Push( Reflect::RegisterClass<GraphShaderAsset>( "GraphShaderAsset" ) );
    g_AssetClassTypes.push_back( Reflect::GetType<GraphShaderAsset>() ); 


    /////////////////////////////////////////////////////////////
    // Support for engine types

    // we build up entity classes that consist of all the attributes
    // necessary for the given type.  these are then used in the
    // Classify function to determine what kind of engine type the
    // entity class maps to.  (ie: an entity class maps to the
    // smallest superset)

    ASSET_BEGIN_REGISTER_ENGINE_TYPES;
    {
      // Null == -1
      g_EngineTypeInfos.insert( M_EngineTypeInfo::value_type( EngineTypes::Null, Asset::GetEngineTypeInfo( EngineTypes::Null ) ) );

      // Level
      ASSET_REGISTER_ENGINETYPE( Level );
      ASSET_REGISTER_ENGINETYPE_ICONFILENAME( "enginetype_level_16.png" );
      ASSET_REGISTER_ENGINETYPE_TYPECOLOR_ARGB( 0xff, 142, 234, 251 );

      // Shader
      ASSET_REGISTER_ENGINETYPE( Shader );
      ASSET_REGISTER_ENGINETYPE_ICONFILENAME( "enginetype_shader_16.png" );
      ASSET_REGISTER_ENGINETYPE_TYPECOLOR_ARGB( 0xff, 57, 143, 202 );

      // TexturePack
      ASSET_REGISTER_ENGINETYPE( TexturePack );
      ASSET_REGISTER_ENGINETYPE_BUILDERDLL( "TexturePackBuilder.dll" );
      ASSET_REGISTER_ENGINETYPE_ICONFILENAME( "enginetype_texturepack_16.png" );
      ASSET_REGISTER_ENGINETYPE_TYPECOLOR_ARGB( 0xff, 164, 93, 163 );
    }


    //
    // Enums
    //
    g_InitializerStack.Push( Reflect::RegisterEnumeration<Asset::EngineTypes::EngineType>( &Asset::EngineTypes::EngineTypeEnumerateEnumeration, "EngineType" ) );

    // Above is for supporting engine types
    ///////////////////////////////////////////////////////////////
  }
}

void Asset::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    g_AssetClassTypes.clear();
    g_EngineTypeInfos.clear();

    AssetClass::CleanupCache();

    g_InitializerStack.Cleanup();
  }
}

REFLECT_DEFINE_ENTRY_POINTS(Asset::Initialize, Asset::Cleanup);
