#include "AssetInit.h"

#include "Pipeline/Asset/Attributes/ArtFileAttribute.h"
#include "Pipeline/Asset/AssetType.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/AssetFile.h"
#include "Pipeline/Asset/AssetFolder.h"
#include "Pipeline/Asset/AssetTemplate.h"
#include "Pipeline/Asset/AssetVersion.h"
#include "Pipeline/Asset/Tracker/CacheDB.h"
#include "Pipeline/Asset/Attributes/ColorMapAttribute.h"
#include "Pipeline/Asset/Attributes/DependenciesAttribute.h"
#include "Pipeline/Asset/Attributes/DetailMapAttribute.h"
#include "Pipeline/Asset/Classes/Entity.h"
#include "Pipeline/Asset/Classes/EntityAsset.h"
#include "Pipeline/Asset/Manifests/EntityManifest.h"
#include "Pipeline/Asset/Attributes/ExpensiveMapAttribute.h"
#include "Pipeline/Asset/ExporterJob.h"
#include "Pipeline/Asset/Classes/SceneAsset.h"
#include "Pipeline/Asset/Manifests/ManifestVersion.h"
#include "Pipeline/Asset/Attributes/NormalMapAttribute.h"
#include "Pipeline/Asset/Classes/RequiredListAsset.h"
#include "Pipeline/Asset/Attributes/StandardColorMapAttribute.h"
#include "Pipeline/Asset/Attributes/StandardDetailMapAttribute.h"
#include "Pipeline/Asset/Attributes/StandardExpensiveMapAttribute.h"
#include "Pipeline/Asset/Attributes/StandardNormalMapAttribute.h"
#include "Pipeline/Asset/Classes/StandardShaderAsset.h"
#include "Pipeline/Asset/Attributes/TextureMapAttribute.h"
#include "Pipeline/Asset/Manifests/SceneManifest.h"

#include "Attribute/AttributeInit.h"
#include "Attribute/AttributeCategories.h"
#include "Foundation/InitializerStack.h"
#include "Pipeline/Content/ContentInit.h"
#include "Reflect/Registry.h"

#include "Foundation/InitializerStack.h"

using namespace Reflect;

#define ASSET_BEGIN_REGISTER_ENGINE_TYPES                                       \
  Nocturnal::Insert<M_AssetTypeInfo>::Result et_inserted;

#define ASSET_REGISTER_ENGINETYPE( __AssetTypeName )                           \
  et_inserted = g_AssetTypeInfos.insert( M_AssetTypeInfo::value_type( AssetTypes::__AssetTypeName, AssetTypeInfo( #__AssetTypeName, #__AssetTypeName"Builder.dll" ) ) );

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

i32 g_AssetInitCount = 0;

Nocturnal::InitializerStack g_AssetInitializerStack;

void Asset::Initialize()
{
  if ( ++g_AssetInitCount == 1 )
  {
    g_AssetInitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
    g_AssetInitializerStack.Push( Finder::Initialize, Finder::Cleanup );
    g_AssetInitializerStack.Push( Attribute::Initialize, Attribute::Cleanup );
    g_AssetInitializerStack.Push( Content::Initialize, Content::Cleanup );

    //
    // Numbered asset conversion
    //

    g_AssetInitializerStack.Push( Reflect::RegisterClass<ExporterJob>( "ExporterJob" ) );


    //
    // Enums
    //

    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<RunTimeFilters::RunTimeFilter>( &RunTimeFilters::RunTimeFilterEnumerateEnumeration, "RunTimeFilter" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<WrapModes::WrapMode>( &WrapModes::WrapModeEnumerateEnumeration, "WrapMode" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<AlphaTypes::AlphaType>( &AlphaTypes::AlphaTypeEnumerateEnumeration, "AlphaType" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<WetSurfaceTypes::WetSurfaceType>( &WetSurfaceTypes::WetSurfaceTypeEnumerateEnumeration, "WetSurfaceType" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<MipGenFilterTypes::MipGenFilterType>( &MipGenFilterTypes::MipGenFilterTypeEnumerateEnumeration, "MipGenFilterType" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<PostMipFilterTypes::PostMipFilterType>( &PostMipFilterTypes::PostMipFilterTypeEnumerateEnumeration, "PostMipFilterType" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<ReductionRatios::ReductionRatio>( &ReductionRatios::ReductionRatioEnumerateEnumeration, "ReductionRatio" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<ColorTexFormats::ColorTexFormat>( &ColorTexFormats::ColorTexFormatEnumerateEnumeration, "ColorTexFormat" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<NormalTexFormats::NormalTexFormat>( &NormalTexFormats::NormalTexFormatEnumerateEnumeration, "NormalTexFormat" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<GlossParaIncanTexFormats::GlossParaIncanTexFormat>( &GlossParaIncanTexFormats::GlossParaIncanTexFormatEnumerateEnumeration, "GlossParaIncanTexFormat" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<CubeSpecTypeFormats::CubeSpecTypeFormat>( &CubeSpecTypeFormats::CubeSpecTypeFormatEnumerateEnumeration, "CubeSpecTypeFormat" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<DetailTexFormats::DetailTexFormat>( &DetailTexFormats::DetailTexFormatEnumerateEnumeration, "DetailTexFormat" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<Asset::AssetType>( &AssetTypes::AssetTypeEnumerateEnumeration, "AssetType" ) );

    //
    // Basic Types
    //

    g_AssetInitializerStack.Push( Reflect::RegisterClass<AssetTemplate>( "AssetTemplate" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<AssetVersion>( "AssetVersion" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<AssetClass>( "AssetClass" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<AssetFile>( "AssetFile" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<AssetFolder>( "AssetFolder" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<ShaderAsset>( "ShaderAsset" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<FileBackedAttribute>( "FileBackedAttribute" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<Entity>( "Entity" ) );

    g_AssetInitializerStack.Push( Reflect::RegisterClass<ManifestVersion>( "ManifestVersion" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<AssetManifest>( "AssetManifest" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<EntityManifest>( "EntityManifest" ) ); Reflect::Registry::GetInstance()->AliasType( Reflect::GetClass< EntityManifest >(), "AssetManifest" );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<SceneManifest>( "SceneManifest" ) );


    //
    // Misc
    //

    // texture
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<TextureWrapModes::TextureWrapMode>( &TextureWrapModes::TextureWrapModeEnumerateEnumeration, "TextureWrapMode" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<TextureColorFormats::TextureColorFormat>( &TextureColorFormats::TextureColorFormatEnumerateEnumeration, "TextureColorFormat" ) );

    //
    // Asset Attributes
    //

    g_AssetInitializerStack.Push( Reflect::RegisterClass<DependenciesAttribute>( "DependenciesAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new DependenciesAttribute );

    g_AssetInitializerStack.Push( Reflect::RegisterClass<TextureMapAttribute>( "TextureMapAttribute" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<ColorMapAttribute>( "ColorMapAttribute" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<NormalMapAttribute>( "NormalMapAttribute" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<ExpensiveMapAttribute>( "ExpensiveMapAttribute" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<DetailMapAttribute>( "DetailMapAttribute" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<StandardColorMapAttribute>( "StandardColorMapAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new StandardColorMapAttribute );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<StandardNormalMapAttribute>( "StandardNormalMapAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new StandardNormalMapAttribute );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<StandardExpensiveMapAttribute>( "StandardExpensiveMapAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new StandardExpensiveMapAttribute );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<StandardDetailMapAttribute>( "StandardDetailMapAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new StandardDetailMapAttribute );

    //
    // Attribute Sets
    //

    // appearance
    g_AssetInitializerStack.Push( Reflect::RegisterClass<ArtFileAttribute>( "ArtFileAttribute" ) );
    Attribute::AttributeCategories::GetInstance()->Categorize( new ArtFileAttribute );

    //
    // Asset classes
    //

    g_AssetInitializerStack.Push( Reflect::RegisterClass<EntityAsset>( "EntityAsset" ) );
    g_AssetClassTypes.push_back( Reflect::GetType<EntityAsset>() );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<SceneAsset>( "SceneAsset" ) );
    g_AssetClassTypes.push_back( Reflect::GetType<SceneAsset>() );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<RequiredListAsset>( "RequiredListAsset" ) );
    g_AssetClassTypes.push_back( Reflect::GetType<RequiredListAsset>() );

    // Shaders
    g_AssetInitializerStack.Push( Reflect::RegisterClass<StandardShaderAsset>( "StandardShaderAsset" ) );
    g_AssetClassTypes.push_back( Reflect::GetType<StandardShaderAsset>() );


    /////////////////////////////////////////////////////////////
    // Support for engine types

    // we build up entity classes that consist of all the attributes
    // necessary for the given type.  these are then used in the
    // Classify function to determine what kind of engine type the
    // entity class maps to.  (ie: an entity class maps to the
    // smallest superset)

    /*
    ASSET_BEGIN_REGISTER_ENGINE_TYPES;
    {
      // Null == -1
      g_AssetTypeInfos.insert( M_AssetTypeInfo::value_type( AssetTypes::Null, Asset::GetAssetTypeInfo( AssetTypes::Null ) ) );

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
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<Asset::AssetTypes::AssetType>( &Asset::AssetTypes::AssetTypeEnumerateEnumeration, "AssetType" ) );
*/
    // Above is for supporting engine types
    ///////////////////////////////////////////////////////////////
  }
}

void Asset::Cleanup()
{
  if ( --g_AssetInitCount == 0 )
  {
    g_AssetClassTypes.clear();

    g_AssetInitializerStack.Cleanup();
  }
}
