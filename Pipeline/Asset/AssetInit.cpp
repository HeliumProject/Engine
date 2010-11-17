#include "AssetInit.h"

#include "Foundation/Component/ComponentInit.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/InitializerStack.h"

#include "Pipeline/Asset/TextureEnums.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Classes/Entity.h"
#include "Pipeline/Asset/Classes/Texture.h"
#include "Pipeline/Asset/Classes/AnimationClip.h"
#include "Pipeline/Asset/Classes/AudioClip.h"
#include "Pipeline/Asset/Classes/SceneAsset.h"
#include "Pipeline/Asset/Classes/ShaderAsset.h"
#include "Pipeline/Asset/Classes/RawFile.h"

#include "Pipeline/Asset/Components/DependenciesComponent.h"
#include "Pipeline/Asset/Components/MeshProcessingComponent.h"
#include "Pipeline/Asset/Components/TextureProcessingComponent.h"
#include "Pipeline/Asset/Components/TransformComponent.h"
#include "Pipeline/Asset/Components/BoundingBoxComponent.h"

#include "Pipeline/Asset/Factories/MeshAssetFactory.h"
#include "Pipeline/Asset/Factories/TextureAssetFactory.h"
#include "Pipeline/Asset/Manifests/EntityManifest.h"

#include "Pipeline/Asset/Manifests/ManifestVersion.h"
#include "Pipeline/Asset/Manifests/SceneManifest.h"

using namespace Helium;

#define ASSET_BEGIN_REGISTER_ENGINE_TYPES                                       \
    Helium::Insert<M_AssetTypeInfo>::Result et_inserted;

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

int32_t g_AssetInitCount = 0;

Helium::InitializerStack g_AssetInitializerStack;

std::vector< AssetFactory* > g_AssetFactories;

void Asset::Initialize()
{
    if ( ++g_AssetInitCount == 1 )
    {
        g_AssetInitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
        g_AssetInitializerStack.Push( Component::Initialize, Component::Cleanup );

        //
        // Enums
        //

        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< TextureFilters::TextureFilter >( &TextureFilters::TextureFilterEnumerateEnum, TXT( "Asset::TextureFilter" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< WrapModes::WrapMode >( &WrapModes::WrapModeEnumerateEnum, TXT( "Asset::WrapMode" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< AlphaTypes::AlphaType >( &AlphaTypes::AlphaTypeEnumerateEnum, TXT( "Asset::AlphaType" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< WetSurfaceTypes::WetSurfaceType >( &WetSurfaceTypes::WetSurfaceTypeEnumerateEnum, TXT( "Asset::WetSurfaceType" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< MipGenFilterTypes::MipGenFilterType >( &MipGenFilterTypes::MipGenFilterTypeEnumerateEnum, TXT( "Asset::MipGenFilterType" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< PostMipFilterTypes::PostMipFilterType >( &PostMipFilterTypes::PostMipFilterTypeEnumerateEnum, TXT( "Asset::PostMipFilterType" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< ReductionRatios::ReductionRatio >( &ReductionRatios::ReductionRatioEnumerateEnum, TXT( "Asset::ReductionRatio" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< AnimationClipModes::AnimationClipMode >( &AnimationClipModes::AnimationClipModeEnumerateEnum, TXT( "Asset::AnimationClipMode" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< AudioClipModes::AudioClipMode >( &AudioClipModes::AudioClipModeEnumerateEnum, TXT( "Asset::AudioClipMode" ) ) );

        //
        // Basic Types
        //

        g_AssetInitializerStack.Push( Reflect::RegisterClassType<AssetClass>( TXT( "Asset::AssetClass" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<ManifestVersion>( TXT( "Asset::ManifestVersion" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<AssetManifest>( TXT( "Asset::AssetManifest" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<SceneManifest>( TXT( "Asset::SceneManifest" ) ) );


        //
        // Misc
        //

        // texture
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType<TextureWrapModes::TextureWrapMode>( &TextureWrapModes::TextureWrapModeEnumerateEnum, TXT( "Asset::TextureWrapMode" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType<TextureColorFormats::TextureColorFormat>( &TextureColorFormats::TextureColorFormatEnumerateEnum, TXT( "Asset::TextureColorFormat" ) ) );

        //
        // Components
        //

        g_AssetInitializerStack.Push( Reflect::RegisterClassType<DependenciesComponent>( TXT( "Asset::DependenciesComponent" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType<MeshCompressionFactor>( &MeshCompressionFactors::MeshComressionFactorEnumerateEnum, TXT("Asset::MeshCompressionFactor") ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<MeshProcessingComponent>( TXT( "Asset::MeshProcessingComponent" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType<TextureDataFormat>( &TextureDataFormats::TextureDataFormatEnumerateEnum, TXT("Asset::TextureDataFormat") ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<TextureProcessingComponent>( TXT( "Asset::TextureProcessingComponent" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<TransformComponent>( TXT( "Asset::TransformComponent" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType< BoundingBoxComponent >( TXT( "Asset::BoundingBoxComponent" ) ) );


        //
        // Asset classes
        //

        g_AssetInitializerStack.Push( Reflect::RegisterClassType<ShaderAsset>( TXT( "Asset::ShaderAsset" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<Entity>( TXT( "Asset::Entity" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<SceneAsset>( TXT( "Asset::SceneAsset" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<Texture>( TXT( "Asset::Texture" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<AnimationClip>( TXT( "Asset::AnimationClip" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<AudioClip>( TXT( "Asset::AudioClip" ) ) );
		g_AssetInitializerStack.Push( Reflect::RegisterClassType<RawFile>( TXT( "Asset::RawFile" ) ) );

        g_AssetFactories.push_back( new MeshAssetFactory() );
		g_AssetFactories.push_back( new TextureAssetFactory() );

        for ( std::vector< AssetFactory* >::iterator itr = g_AssetFactories.begin(), end = g_AssetFactories.end(); itr != end; ++itr )
        {
            for ( std::set< tstring >::const_iterator extensionItr = (*itr)->GetExtensions().begin(), extensionEnd = (*itr)->GetExtensions().end(); extensionItr != extensionEnd; ++extensionItr )
            {
                AssetClass::RegisterFactory( (*extensionItr), (*itr) );
            }
        }
    }
}

void Asset::Cleanup()
{
    if ( --g_AssetInitCount == 0 )
    {
        for ( std::vector< AssetFactory* >::iterator itr = g_AssetFactories.begin(), end = g_AssetFactories.end(); itr != end; ++itr )
        {
            AssetClass::UnregisterFactory( (*itr) );
            delete (*itr);
        }
        g_AssetFactories.clear();

        g_AssetInitializerStack.Cleanup();
    }
}
