#include "AssetInit.h"

#include "Pipeline/Asset/Components/ArtFileComponent.h"
#include "Pipeline/Asset/AssetType.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/AssetFile.h"
#include "Pipeline/Asset/AssetFolder.h"
#include "Pipeline/Asset/AssetTemplate.h"
#include "Pipeline/Asset/AssetVersion.h"
#include "Pipeline/Asset/Components/ColorMapComponent.h"
#include "Pipeline/Asset/Components/DependenciesComponent.h"
#include "Pipeline/Asset/Components/DetailMapComponent.h"
#include "Pipeline/Asset/Classes/Entity.h"
#include "Pipeline/Asset/Classes/EntityAsset.h"
#include "Pipeline/Asset/Manifests/EntityManifest.h"
#include "Pipeline/Asset/Components/ExpensiveMapComponent.h"
#include "Pipeline/Asset/ExporterJob.h"
#include "Pipeline/Asset/Classes/SceneAsset.h"
#include "Pipeline/Asset/Manifests/ManifestVersion.h"
#include "Pipeline/Asset/Components/NormalMapComponent.h"
#include "Pipeline/Asset/Components/StandardColorMapComponent.h"
#include "Pipeline/Asset/Components/StandardDetailMapComponent.h"
#include "Pipeline/Asset/Components/StandardExpensiveMapComponent.h"
#include "Pipeline/Asset/Components/StandardNormalMapComponent.h"
#include "Pipeline/Asset/Classes/StandardShaderAsset.h"
#include "Pipeline/Asset/Components/TextureMapComponent.h"
#include "Pipeline/Asset/Manifests/SceneManifest.h"

#include "Pipeline/Component/ComponentInit.h"
#include "Pipeline/Component/ComponentCategories.h"
#include "Foundation/InitializerStack.h"
#include "Pipeline/Content/ContentInit.h"
#include "Foundation/Reflect/Registry.h"

#include "Foundation/InitializerStack.h"

using namespace Reflect;

#define ASSET_BEGIN_REGISTER_ENGINE_TYPES                                       \
  Nocturnal::Insert<M_AssetTypeInfo>::Result et_inserted;

#define ASSET_REGISTER_ENGINETYPE( __AssetTypeName )                           \
  et_inserted = g_AssetTypeInfos.insert( M_AssetTypeInfo::value_type( AssetTypes::__AssetTypeName, AssetTypeInfo( #__AssetTypeName, #__AssetTypeName"Builder.dll" ) ) ) );

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
  et_inserted.first->second.m_EntityAsset->SetComponent( new __Attribute() );

using namespace Asset;

std::vector< i32 > Asset::g_AssetClassTypes;

i32 g_AssetInitCount = 0;

Nocturnal::InitializerStack g_AssetInitializerStack;

void Asset::Initialize()
{
  if ( ++g_AssetInitCount == 1 )
  {
    g_AssetInitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
    g_AssetInitializerStack.Push( Component::Initialize, Component::Cleanup );
    g_AssetInitializerStack.Push( Content::Initialize, Content::Cleanup );

    //
    // Numbered asset conversion
    //

    g_AssetInitializerStack.Push( Reflect::RegisterClass<ExporterJob>( TXT( "ExporterJob" ) ) );


    //
    // Enums
    //

    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<RunTimeFilters::RunTimeFilter>( &RunTimeFilters::RunTimeFilterEnumerateEnumeration, TXT( "RunTimeFilter" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<WrapModes::WrapMode>( &WrapModes::WrapModeEnumerateEnumeration, TXT( "WrapMode" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<AlphaTypes::AlphaType>( &AlphaTypes::AlphaTypeEnumerateEnumeration, TXT( "AlphaType" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<WetSurfaceTypes::WetSurfaceType>( &WetSurfaceTypes::WetSurfaceTypeEnumerateEnumeration, TXT( "WetSurfaceType" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<MipGenFilterTypes::MipGenFilterType>( &MipGenFilterTypes::MipGenFilterTypeEnumerateEnumeration, TXT( "MipGenFilterType" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<PostMipFilterTypes::PostMipFilterType>( &PostMipFilterTypes::PostMipFilterTypeEnumerateEnumeration, TXT( "PostMipFilterType" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<ReductionRatios::ReductionRatio>( &ReductionRatios::ReductionRatioEnumerateEnumeration, TXT( "ReductionRatio" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<ColorTexFormats::ColorTexFormat>( &ColorTexFormats::ColorTexFormatEnumerateEnumeration, TXT( "ColorTexFormat" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<NormalTexFormats::NormalTexFormat>( &NormalTexFormats::NormalTexFormatEnumerateEnumeration, TXT( "NormalTexFormat" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<GlossParaIncanTexFormats::GlossParaIncanTexFormat>( &GlossParaIncanTexFormats::GlossParaIncanTexFormatEnumerateEnumeration, TXT( "GlossParaIncanTexFormat" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<CubeSpecTypeFormats::CubeSpecTypeFormat>( &CubeSpecTypeFormats::CubeSpecTypeFormatEnumerateEnumeration, TXT( "CubeSpecTypeFormat" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<DetailTexFormats::DetailTexFormat>( &DetailTexFormats::DetailTexFormatEnumerateEnumeration, TXT( "DetailTexFormat" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<Asset::AssetType>( &AssetTypes::AssetTypeEnumerateEnumeration, TXT( "AssetType" ) ) );

    //
    // Basic Types
    //

    g_AssetInitializerStack.Push( Reflect::RegisterClass<AssetTemplate>( TXT( "AssetTemplate" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<AssetVersion>( TXT( "AssetVersion" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<AssetClass>( TXT( "AssetClass" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<AssetFile>( TXT( "AssetFile" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<AssetFolder>( TXT( "AssetFolder" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<ShaderAsset>( TXT( "ShaderAsset" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<FileBackedComponent>( TXT( "FileBackedComponent" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<Entity>( TXT( "Entity" ) ) );

    g_AssetInitializerStack.Push( Reflect::RegisterClass<ManifestVersion>( TXT( "ManifestVersion" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<AssetManifest>( TXT( "AssetManifest" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<EntityManifest>( TXT( "EntityManifest" ) ) ); Reflect::Registry::GetInstance()->AliasType( Reflect::GetClass< EntityManifest >(), TXT( "AssetManifest" ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<SceneManifest>( TXT( "SceneManifest" ) ) );


    //
    // Misc
    //

    // texture
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<TextureWrapModes::TextureWrapMode>( &TextureWrapModes::TextureWrapModeEnumerateEnumeration, TXT( "TextureWrapMode" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<TextureColorFormats::TextureColorFormat>( &TextureColorFormats::TextureColorFormatEnumerateEnumeration, TXT( "TextureColorFormat" ) ) );

    //
    // Asset Attributes
    //

    g_AssetInitializerStack.Push( Reflect::RegisterClass<DependenciesComponent>( TXT( "DependenciesComponent" ) ) );
    Component::ComponentCategories::GetInstance()->Categorize( new DependenciesComponent );

    g_AssetInitializerStack.Push( Reflect::RegisterClass<TextureMapComponent>( TXT( "TextureMapComponent" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<ColorMapComponent>( TXT( "ColorMapComponent" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<NormalMapComponent>( TXT( "NormalMapComponent" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<ExpensiveMapComponent>( TXT( "ExpensiveMapComponent" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<DetailMapComponent>( TXT( "DetailMapComponent" ) ) );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<StandardColorMapComponent>( TXT( "StandardColorMapComponent" ) ) );
    Component::ComponentCategories::GetInstance()->Categorize( new StandardColorMapComponent );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<StandardNormalMapComponent>( TXT( "StandardNormalMapComponent" ) ) );
    Component::ComponentCategories::GetInstance()->Categorize( new StandardNormalMapComponent );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<StandardExpensiveMapComponent>( TXT( "StandardExpensiveMapComponent" ) ) );
    Component::ComponentCategories::GetInstance()->Categorize( new StandardExpensiveMapComponent );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<StandardDetailMapComponent>( TXT( "StandardDetailMapComponent" ) ) );
    Component::ComponentCategories::GetInstance()->Categorize( new StandardDetailMapComponent );

    //
    // Attribute Sets
    //

    // appearance
    g_AssetInitializerStack.Push( Reflect::RegisterClass<ArtFileComponent>( TXT( "ArtFileComponent" ) ) );
    Component::ComponentCategories::GetInstance()->Categorize( new ArtFileComponent );

    //
    // Asset classes
    //

    g_AssetInitializerStack.Push( Reflect::RegisterClass<EntityAsset>( TXT( "EntityAsset" ) ) );
    g_AssetClassTypes.push_back( Reflect::GetType<EntityAsset>() );
    g_AssetInitializerStack.Push( Reflect::RegisterClass<SceneAsset>( TXT( "SceneAsset" ) ) );
    g_AssetClassTypes.push_back( Reflect::GetType<SceneAsset>() );

    // Shaders
    g_AssetInitializerStack.Push( Reflect::RegisterClass<StandardShaderAsset>( TXT( "StandardShaderAsset" ) ) );
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
      ASSET_REGISTER_ENGINETYPE_ICONFILENAME( TXT( "enginetype_level" ) );
      ASSET_REGISTER_ENGINETYPE_TYPECOLOR_ARGB( 0xff, 142, 234, 251 );

      // Shader
      ASSET_REGISTER_ENGINETYPE( Shader );
      ASSET_REGISTER_ENGINETYPE_ICONFILENAME( TXT( "enginetype_shader" ) );
      ASSET_REGISTER_ENGINETYPE_TYPECOLOR_ARGB( 0xff, 57, 143, 202 );

      // TexturePack
      ASSET_REGISTER_ENGINETYPE( TexturePack );
      ASSET_REGISTER_ENGINETYPE_BUILDERDLL( TXT( "TexturePackBuilder.dll" ) );
      ASSET_REGISTER_ENGINETYPE_ICONFILENAME( TXT( "enginetype_texturepack" ) );
      ASSET_REGISTER_ENGINETYPE_TYPECOLOR_ARGB( 0xff, 164, 93, 163 );
    }


    //
    // Enums
    //
    g_AssetInitializerStack.Push( Reflect::RegisterEnumeration<Asset::AssetTypes::AssetType>( &Asset::AssetTypes::AssetTypeEnumerateEnumeration, TXT( "AssetType" ) ) );
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
