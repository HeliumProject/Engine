//----------------------------------------------------------------------------------------------------------------------
// GraphicsTypeRegistration.cpp
//
// Copyright (C) 2012 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "GraphicsPch.h"
#include "Platform/Assert.h"
#include "Engine/Package.h"

#include "Graphics/Material.h"
#include "Graphics/Font.h"
#include "Graphics/Shader.h"
#include "Graphics/Animation.h"
#include "Graphics/Texture2d.h"
#include "Graphics/GraphicsConfig.h"
#include "Graphics/GraphicsScene.h"
#include "Graphics/Texture.h"
#include "Graphics/Mesh.h"

static Helium::StrongPtr< Helium::Package > spGraphicsTypePackage;

HELIUM_GRAPHICS_API Helium::Package* GetGraphicsTypePackage()
{
    Helium::Package* pPackage = spGraphicsTypePackage;
    if( !pPackage )
    {
        Helium::GameObject* pTypesPackageObject = Helium::GameObject::FindChildOf( NULL, Helium::Name( TXT( "Types" ) ) );
        HELIUM_ASSERT( pTypesPackageObject );
        HELIUM_ASSERT( pTypesPackageObject->IsPackage() );

        HELIUM_VERIFY( Helium::GameObject::Create< Helium::Package >(
            spGraphicsTypePackage,
            Helium::Name( TXT( "Graphics" ) ),
            pTypesPackageObject ) );
        pPackage = spGraphicsTypePackage;
        HELIUM_ASSERT( pPackage );
    }

    return pPackage;
}

HELIUM_GRAPHICS_API void ReleaseGraphicsTypePackage()
{
    spGraphicsTypePackage = NULL;
}

HELIUM_GRAPHICS_API void RegisterGraphicsTypes()
{
    HELIUM_VERIFY( GetGraphicsTypePackage() );

    HELIUM_VERIFY( Helium::Animation::InitStaticType() );
    HELIUM_VERIFY( Helium::Font::InitStaticType() );
    HELIUM_VERIFY( Helium::GraphicsConfig::InitStaticType() );
    HELIUM_VERIFY( Helium::GraphicsScene::InitStaticType() );
    HELIUM_VERIFY( Helium::Material::InitStaticType() );
    HELIUM_VERIFY( Helium::Shader::InitStaticType() );
    HELIUM_VERIFY( Helium::ShaderVariant::InitStaticType() );
    HELIUM_VERIFY( Helium::Texture::InitStaticType() );
    HELIUM_VERIFY( Helium::Texture2d::InitStaticType() );
    HELIUM_VERIFY( Helium::Mesh::InitStaticType() );
}

HELIUM_GRAPHICS_API void UnregisterGraphicsTypes()
{
    Helium::Animation::ReleaseStaticType();
    Helium::Font::ReleaseStaticType();
    Helium::GraphicsConfig::ReleaseStaticType();
    Helium::GraphicsScene::ReleaseStaticType();
    Helium::Material::ReleaseStaticType();
    Helium::Shader::ReleaseStaticType();
    Helium::ShaderVariant::ReleaseStaticType();
    Helium::Texture::ReleaseStaticType();
    Helium::Texture2d::ReleaseStaticType();
    Helium::Mesh::ReleaseStaticType();

    ReleaseGraphicsTypePackage();
}
