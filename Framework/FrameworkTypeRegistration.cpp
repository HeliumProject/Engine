//----------------------------------------------------------------------------------------------------------------------
// FrameworkTypeRegistration.cpp
//
// Copyright (C) 2012 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Platform/Assert.h"
#include "Engine/Package.h"

#include "Framework/Slice.h"
#include "Framework/MeshEntity.h"
#include "Framework/Mesh.h"
#include "Framework/Entity.h"
#include "Framework/Camera.h"
#include "Framework/SkeletalMeshEntity.h"
#include "Framework/StaticMeshEntity.h"
#include "Framework/World.h"

static Helium::StrongPtr< Helium::Package > spFrameworkTypePackage;

HELIUM_FRAMEWORK_API Helium::Package* GetFrameworkTypePackage()
{
    Helium::Package* pPackage = spFrameworkTypePackage;
    if( !pPackage )
    {
        Helium::GameObject* pTypesPackageObject = Helium::GameObject::FindChildOf( NULL, Helium::Name( TXT( "Types" ) ) );
        HELIUM_ASSERT( pTypesPackageObject );
        HELIUM_ASSERT( pTypesPackageObject->IsPackage() );

        HELIUM_VERIFY( Helium::GameObject::Create< Helium::Package >(
            spFrameworkTypePackage,
            Helium::Name( TXT( "Framework" ) ),
            pTypesPackageObject ) );
        pPackage = spFrameworkTypePackage;
        HELIUM_ASSERT( pPackage );
    }

    return pPackage;
}

HELIUM_FRAMEWORK_API void ReleaseFrameworkTypePackage()
{
    spFrameworkTypePackage = NULL;
}

HELIUM_FRAMEWORK_API void RegisterFrameworkTypes()
{
    HELIUM_VERIFY( GetFrameworkTypePackage() );

    HELIUM_VERIFY( Helium::Camera::InitStaticType() );
    HELIUM_VERIFY( Helium::Entity::InitStaticType() );
    HELIUM_VERIFY( Helium::Slice::InitStaticType() );
    HELIUM_VERIFY( Helium::Mesh::InitStaticType() );
    HELIUM_VERIFY( Helium::MeshEntity::InitStaticType() );
    HELIUM_VERIFY( Helium::SkeletalMeshEntity::InitStaticType() );
    HELIUM_VERIFY( Helium::StaticMeshEntity::InitStaticType() );
    HELIUM_VERIFY( Helium::World::InitStaticType() );
}

HELIUM_FRAMEWORK_API void UnregisterFrameworkTypes()
{
    Helium::Camera::ReleaseStaticType();
    Helium::Entity::ReleaseStaticType();
    Helium::Slice::ReleaseStaticType();
    Helium::Mesh::ReleaseStaticType();
    Helium::MeshEntity::ReleaseStaticType();
    Helium::SkeletalMeshEntity::ReleaseStaticType();
    Helium::StaticMeshEntity::ReleaseStaticType();
    Helium::World::ReleaseStaticType();

    ReleaseFrameworkTypePackage();
}
