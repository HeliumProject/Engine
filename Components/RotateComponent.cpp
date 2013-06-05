
#include "ComponentsPch.h"
#include "Components/RotateComponent.h"

#include "Components/TransformComponent.h"

#include "Ois/OisSystem.h"
#include "Framework/WorldManager.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET(Helium::RotateComponentDefinition, Components, 0);

void Helium::RotateComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	//comp.AddField(&RotateComponentDefinition::m_RotateBy, "m_TestValue");
}

RotateComponentDefinition::RotateComponentDefinition()
{

}


HELIUM_DEFINE_COMPONENT(Helium::RotateComponent, 16);

void Helium::RotateComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void Helium::RotateComponent::ApplyRotation( TransformComponent *pTransform )
{
	HELIUM_ASSERT(pTransform);

	static const float INPUT_SPEED = 2.0f;
	static const float IDLE_SPEED = 0.15f;

	float fYawChange = 0.0f;
	bool bHasInput = false;

	if (Helium::Input::IsKeyDown(Input::KeyCodes::KC_LEFT))
	{
		fYawChange += INPUT_SPEED;
		bHasInput = true;
	}

	if (Helium::Input::IsKeyDown(Input::KeyCodes::KC_RIGHT))
	{
		fYawChange -= INPUT_SPEED;
		bHasInput = true;
	}

	if (!bHasInput)
	{
		fYawChange = IDLE_SPEED;
	}

	fYawChange *= WorldManager::GetStaticInstance().GetFrameDeltaSeconds();
		
	Simd::Quat rotation( 0.0f, fYawChange, 0.0f );
	pTransform->SetRotation( pTransform->GetRotation() * rotation );
}
