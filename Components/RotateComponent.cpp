
#include "ComponentsPch.h"
#include "Components/RotateComponent.h"

#include "Components/TransformComponent.h"

HELIUM_IMPLEMENT_ASSET(Helium::RotateComponentDefinition, Components, 0);

void Helium::RotateComponentDefinition::PopulateComposite( Reflect::Composite& comp )
{
	//comp.AddField(&RotateComponentDefinition::m_RotateBy, "m_TestValue");
}

HELIUM_DEFINE_COMPONENT(Helium::RotateComponent, 16);

void Helium::RotateComponent::PopulateComposite( Reflect::Composite& comp )
{

}

void Helium::RotateComponent::ApplyRotation( TransformComponent *pTransform )
{
    HELIUM_ASSERT(pTransform);

    Simd::Quat rotation( 0.0f, 0.01f, 0.0f );

    //Simd::Quat rotation( meshRotation * 0.438f, static_cast< float32_t >( HELIUM_PI_2 ), meshRotation );
    pTransform->SetRotation( pTransform->GetRotation() * rotation );
}
