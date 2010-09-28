#include "AssetInit.h"

#include "Foundation/Component/ComponentInit.h"
#include "Foundation/InitializerStack.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/InitializerStack.h"

#include "Core/Asset/TextureEnums.h"
#include "Core/Asset/AssetClass.h"
#include "Core/Asset/Classes/Entity.h"
#include "Core/Asset/Classes/Texture.h"
#include "Core/Asset/Classes/AnimationClip.h"
#include "Core/Asset/Classes/AudioClip.h"
#include "Core/Asset/Classes/SceneAsset.h"
#include "Core/Asset/Classes/ShaderAsset.h"
#include "Core/Asset/Classes/RawFile.h"

#include "Core/Asset/Components/DependenciesComponent.h"
#include "Core/Asset/Components/MeshProcessingComponent.h"
#include "Core/Asset/Components/TextureProcessingComponent.h"
#include "Core/Asset/Components/TransformComponent.h"
#include "Core/Asset/Components/BoundingBoxComponent.h"

#include "Core/Asset/Factories/MeshAssetFactory.h"
#include "Core/Asset/Factories/TextureAssetFactory.h"
#include "Core/Asset/Manifests/EntityManifest.h"

#include "Core/Asset/Manifests/ManifestVersion.h"
#include "Core/Asset/Manifests/SceneManifest.h"

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

std::vector< i32 > Asset::g_AssetClassTypes;

i32 g_AssetInitCount = 0;

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

        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< TextureFilters::TextureFilter >( &TextureFilters::TextureFilterEnumerateEnum, TXT( "TextureFilter" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< WrapModes::WrapMode >( &WrapModes::WrapModeEnumerateEnum, TXT( "WrapMode" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< AlphaTypes::AlphaType >( &AlphaTypes::AlphaTypeEnumerateEnum, TXT( "AlphaType" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< WetSurfaceTypes::WetSurfaceType >( &WetSurfaceTypes::WetSurfaceTypeEnumerateEnum, TXT( "WetSurfaceType" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< MipGenFilterTypes::MipGenFilterType >( &MipGenFilterTypes::MipGenFilterTypeEnumerateEnum, TXT( "MipGenFilterType" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< PostMipFilterTypes::PostMipFilterType >( &PostMipFilterTypes::PostMipFilterTypeEnumerateEnum, TXT( "PostMipFilterType" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< ReductionRatios::ReductionRatio >( &ReductionRatios::ReductionRatioEnumerateEnum, TXT( "ReductionRatio" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< AnimationClipModes::AnimationClipMode >( &AnimationClipModes::AnimationClipModeEnumerateEnum, TXT( "AnimationClipMode" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType< AudioClipModes::AudioClipMode >( &AudioClipModes::AudioClipModeEnumerateEnum, TXT( "AudioClipMode" ) ) );

        //
        // Basic Types
        //

        g_AssetInitializerStack.Push( Reflect::RegisterClassType<AssetClass>( TXT( "AssetClass" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<ManifestVersion>( TXT( "ManifestVersion" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<AssetManifest>( TXT( "AssetManifest" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<EntityManifest>( TXT( "EntityManifest" ) ) ); Reflect::Registry::GetInstance()->AliasType( Reflect::GetClass< EntityManifest >(), TXT( "AssetManifest" ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<SceneManifest>( TXT( "SceneManifest" ) ) );


        //
        // Misc
        //

        // texture
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType<TextureWrapModes::TextureWrapMode>( &TextureWrapModes::TextureWrapModeEnumerateEnum, TXT( "TextureWrapMode" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType<TextureColorFormats::TextureColorFormat>( &TextureColorFormats::TextureColorFormatEnumerateEnum, TXT( "TextureColorFormat" ) ) );

        //
        // Components
        //

        g_AssetInitializerStack.Push( Reflect::RegisterClassType<DependenciesComponent>( TXT( "DependenciesComponent" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType<MeshCompressionFactor>( &MeshCompressionFactors::MeshComressionFactorEnumerateEnum, TXT("MeshCompressionFactor") ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<MeshProcessingComponent>( TXT( "MeshProcessingComponent" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType<TextureDataFormat>( &TextureDataFormats::TextureDataFormatEnumerateEnum, TXT("TextureDataFormat") ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<TextureProcessingComponent>( TXT( "TextureProcessingComponent" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<TransformComponent>( TXT( "TransformComponent" ) ) );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType< BoundingBoxComponent >( TXT( "BoundingBoxComponent" ) ) );


        //
        // Asset classes
        //

        g_AssetInitializerStack.Push( Reflect::RegisterClassType<ShaderAsset>( TXT( "ShaderAsset" ) ) );
        g_AssetClassTypes.push_back( Reflect::GetType< ShaderAsset >() );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<Entity>( TXT( "Entity" ) ) );
        g_AssetClassTypes.push_back( Reflect::GetType<Entity>() );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<SceneAsset>( TXT( "SceneAsset" ) ) );
        g_AssetClassTypes.push_back( Reflect::GetType<SceneAsset>() );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<Texture>( TXT( "Texture" ) ) );
        g_AssetClassTypes.push_back( Reflect::GetType<Texture>() );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<AnimationClip>( TXT( "AnimationClip" ) ) );
        g_AssetClassTypes.push_back( Reflect::GetType<AnimationClip>() );
        g_AssetInitializerStack.Push( Reflect::RegisterClassType<AudioClip>( TXT( "AudioClip" ) ) );
        g_AssetClassTypes.push_back( Reflect::GetType<AudioClip>() );
		g_AssetInitializerStack.Push( Reflect::RegisterClassType<RawFile>( TXT( "RawFile" ) ) );
		g_AssetClassTypes.push_back( Reflect::GetType<RawFile>() );

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
        g_AssetInitializerStack.Push( Reflect::RegisterEnumType<Asset::AssetTypes::AssetType>( &Asset::AssetTypes::AssetTypeEnumerateEnum, TXT( "AssetType" ) ) );
        */
        // Above is for supporting engine types
        ///////////////////////////////////////////////////////////////

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

        g_AssetClassTypes.clear();

        g_AssetInitializerStack.Cleanup();
    }
}
