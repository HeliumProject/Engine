
#include "ComponentsPch.h"
#include "Engine/Package.h"

#include "Components/ExampleComponent.h"
#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/RotateComponent.h"

HELIUM_COMPONENTS_API void RegisterComponentTypes()
{    
    Helium::TransformComponent::RegisterComponentType(32);
    Helium::MeshComponent::RegisterComponentType(32);
    Helium::RotateComponent::RegisterComponentType(32);
    Helium::MeshSceneObjectTransform::RegisterComponentType(128);
}

HELIUM_COMPONENTS_API void UnregisterComponentTypes()
{

}
