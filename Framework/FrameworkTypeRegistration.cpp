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
#include "Framework/SliceDefinition.h"
#include "Framework/Mesh.h"
#include "Framework/EntityDefinition.h"
#include "Framework/World.h"
#include "Framework/WorldDefinition.h"
#include "Framework/ComponentDefinition.h"
#include "Framework/ComponentDefinitionSet.h"

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
    
    HELIUM_VERIFY( Helium::ComponentDefinition::InitStaticType() );
    HELIUM_VERIFY( Helium::ComponentDefinitionSet::InitStaticType() );
    HELIUM_VERIFY( Helium::EntityDefinition::InitStaticType() );
    HELIUM_VERIFY( Helium::SliceDefinition::InitStaticType() );
    HELIUM_VERIFY( Helium::Mesh::InitStaticType() );
    HELIUM_VERIFY( Helium::WorldDefinition::InitStaticType() );
}

HELIUM_FRAMEWORK_API void UnregisterFrameworkTypes()
{
    Helium::ComponentDefinition::ReleaseStaticType();
    Helium::ComponentDefinitionSet::ReleaseStaticType();
    Helium::EntityDefinition::ReleaseStaticType();
    Helium::SliceDefinition::ReleaseStaticType();
    Helium::Mesh::ReleaseStaticType();
    Helium::WorldDefinition::ReleaseStaticType();

    ReleaseFrameworkTypePackage();
}
