#include "GamePch.h"

#include "ShapeShooter.h"
#include "Framework/WorldManager.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

//////////////////////////////////////////////////////////////////////////
// ShapeShooterComponent

HELIUM_DEFINE_COMPONENT(ShapeShooterComponent, 1);

void ShapeShooterComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void ShapeShooterComponent::Initialize( const ShapeShooterComponentDefinition &definition )
{
	m_StateMachine.Initialize( *GetWorld(), definition.m_StateMachine );
}

//////////////////////////////////////////////////////////////////////////
// ShapeShooterComponentDefinition
HELIUM_DEFINE_CLASS(ShapeShooterComponentDefinition);

void ShapeShooterComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &ShapeShooterComponentDefinition::m_StateMachine, "m_StateMachine" );
}

//////////////////////////////////////////////////////////////////////////

void DoTickShapeShooter( ShapeShooterComponent *pComponent )
{
	pComponent->m_StateMachine.Tick( *pComponent->GetWorld(), WorldManager::GetStaticInstance().GetFrameDeltaSeconds() );
}

HELIUM_DEFINE_TASK( TickShapeShooter, ( ForEachWorld< QueryComponents< ShapeShooterComponent, DoTickShapeShooter > > ), TickTypes::Gameplay )

void TickShapeShooter::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<StandardDependencies::PostPhysicsGameplay>();
}
