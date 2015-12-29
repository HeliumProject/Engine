
#include "ComponentsPch.h"
#include "Components/RotateComponent.h"

#include "Reflect/TranslatorDeduction.h"

#include "Components/TransformComponent.h"

#include "Ois/OisSystem.h"
#include "Framework/WorldManager.h"

using namespace Helium;

HELIUM_DEFINE_CLASS(Helium::RotateComponentDefinition);

void Helium::RotateComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField(&RotateComponentDefinition::m_Roll, "m_Roll");
	comp.AddField(&RotateComponentDefinition::m_Pitch, "m_Pitch");
	comp.AddField(&RotateComponentDefinition::m_Yaw, "m_Yaw");
}

RotateComponentDefinition::RotateComponentDefinition()
	: m_Roll(0.0f)
	, m_Pitch(0.0f)
	, m_Yaw(0.0f)
{

}

HELIUM_DEFINE_COMPONENT(Helium::RotateComponent, 16);

void Helium::RotateComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void Helium::RotateComponent::Initialize( const RotateComponentDefinition &definition )
{
	m_Roll = definition.m_Roll;
	m_Pitch = definition.m_Pitch;
	m_Yaw = definition.m_Yaw;
}

void Helium::RotateComponent::ApplyRotation( TransformComponent *pTransform )
{
	HELIUM_ASSERT(pTransform);

	static const float INPUT_SPEED = 2.0f;
	static const float IDLE_SPEED = 0.15f;

	float fAmount = 0.0f;
	bool bHasInput = false;

	if (Helium::Input::IsKeyDown(Input::KeyCodes::KC_LEFT))
	{
		fAmount += INPUT_SPEED;
		bHasInput = true;
	}

	if (Helium::Input::IsKeyDown(Input::KeyCodes::KC_RIGHT))
	{
		fAmount -= INPUT_SPEED;
		bHasInput = true;
	}

	if (!bHasInput)
	{
		fAmount = IDLE_SPEED;
	}

	fAmount *= WorldManager::GetInstance().GetFrameDeltaSeconds();

	Simd::Quat rotation( m_Pitch * fAmount, m_Yaw * fAmount, m_Roll * fAmount );
	pTransform->SetRotation( pTransform->GetRotation() * rotation );
}

//////////////////////////////////////////////////////////////////////////

void UpdateRotateComponents(RotateComponent *pRotate, TransformComponent *pTransform)
{
	pRotate->ApplyRotation(pTransform);
}

void Helium::UpdateRotateComponentsTask::DefineContract( TaskContract &rContract )
{
	rContract.ExecuteBefore<StandardDependencies::ProcessPhysics>();
	rContract.ExecuteAfter<StandardDependencies::ReceiveInput>();
}

HELIUM_DEFINE_TASK( UpdateRotateComponentsTask, (ForEachWorld< QueryComponents< RotateComponent, TransformComponent, UpdateRotateComponents > >), TickTypes::Gameplay )
