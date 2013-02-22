
#include "ComponentsPch.h"
#include "Engine/Package.h"

#include "Components/ExampleComponent.h"


static Helium::StrongPtr< Helium::Package > spFrameworkTypePackage;

HELIUM_COMPONENTS_API Helium::Package* GetComponentsTypePackage()
{
    Helium::Package* pPackage = spFrameworkTypePackage;
    if( !pPackage )
    {
        Helium::Asset* pTypesPackageObject = Helium::Asset::FindChildOf( NULL, Helium::Name( TXT( "Types" ) ) );
        HELIUM_ASSERT( pTypesPackageObject );
        HELIUM_ASSERT( pTypesPackageObject->IsPackage() );

        HELIUM_VERIFY( Helium::Asset::Create< Helium::Package >(
            spFrameworkTypePackage,
            Helium::Name( TXT( "Framework" ) ),
            pTypesPackageObject ) );
        pPackage = spFrameworkTypePackage;
        HELIUM_ASSERT( pPackage );
    }

    return pPackage;
}

HELIUM_COMPONENTS_API void ReleaseComponentsTypePackage()
{
    spFrameworkTypePackage = NULL;
}

HELIUM_COMPONENTS_API void RegisterFrameworkTypes()
{
    HELIUM_VERIFY( GetComponentsTypePackage() );
    
    HELIUM_VERIFY( Helium::ExampleComponentDefinition::InitStaticType() );
}

HELIUM_COMPONENTS_API void UnregisterFrameworkTypes()
{
    Helium::ExampleComponentDefinition::ReleaseStaticType();

    ReleaseComponentsTypePackage();
}
