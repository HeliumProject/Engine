
#include "ComponentsPch.h"
#include "Engine/Package.h"

#include "Components/ExampleComponent.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/RotateComponent.h"

static Helium::StrongPtr< Helium::Package > spComponentsTypePackage;

HELIUM_COMPONENTS_API Helium::Package* GetComponentsTypePackage()
{
    Helium::Package* pPackage = spComponentsTypePackage;
    if( !pPackage )
    {
        Helium::Asset* pTypesPackageObject = Helium::Asset::FindChildOf( NULL, Helium::Name( TXT( "Types" ) ) );
        HELIUM_ASSERT( pTypesPackageObject );
        HELIUM_ASSERT( pTypesPackageObject->IsPackage() );

        HELIUM_VERIFY( Helium::Asset::Create< Helium::Package >(
            spComponentsTypePackage,
            Helium::Name( TXT( "Components" ) ),
            pTypesPackageObject ) );
        pPackage = spComponentsTypePackage;
        HELIUM_ASSERT( pPackage );
    }

    return pPackage;
}

HELIUM_COMPONENTS_API void ReleaseComponentsTypePackage()
{
    spComponentsTypePackage = NULL;
}

HELIUM_COMPONENTS_API void RegisterComponentTypes()
{
    HELIUM_VERIFY( GetComponentsTypePackage() );
    
    HELIUM_VERIFY( Helium::ExampleComponentDefinition::InitStaticType() );
    HELIUM_VERIFY( Helium::TransformComponentDefinition::InitStaticType() );
    HELIUM_VERIFY( Helium::MeshComponentDefinition::InitStaticType() );
    HELIUM_VERIFY( Helium::RotateComponentDefinition::InitStaticType() );

    Helium::TransformComponent::RegisterComponentType(32);
    Helium::MeshComponent::RegisterComponentType(32);
    Helium::RotateComponent::RegisterComponentType(32);
    Helium::MeshSceneObjectTransform::RegisterComponentType(128);
}

HELIUM_COMPONENTS_API void UnregisterComponentTypes()
{
    Helium::ExampleComponentDefinition::ReleaseStaticType();
    Helium::TransformComponentDefinition::ReleaseStaticType();
    Helium::MeshComponentDefinition::ReleaseStaticType();
    Helium::RotateComponentDefinition::ReleaseStaticType();

    ReleaseComponentsTypePackage();
}
