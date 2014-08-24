#include "GamePch.h"

#include "SideScroller.h"
#include "Framework/WorldManager.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

//////////////////////////////////////////////////////////////////////////
// SideScrollerComponent

HELIUM_DEFINE_COMPONENT(SideScrollerComponent, 1);

void SideScrollerComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void SideScrollerComponent::Initialize( const SideScrollerComponentDefinition &definition )
{
	m_StateMachine.Initialize( *GetWorld(), definition.m_StateMachine );
}

//////////////////////////////////////////////////////////////////////////
// SideScrollerComponentDefinition
HELIUM_DEFINE_CLASS(SideScrollerComponentDefinition);

void SideScrollerComponentDefinition::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &SideScrollerComponentDefinition::m_StateMachine, "m_StateMachine" );
}

//////////////////////////////////////////////////////////////////////////

void DoTickSideScroller( SideScrollerComponent *pComponent )
{
	pComponent->m_StateMachine.Tick( *pComponent->GetWorld(), WorldManager::GetStaticInstance().GetFrameDeltaSeconds() );
}

HELIUM_DEFINE_TASK( TickSideScroller, ( ForEachWorld< QueryComponents< SideScrollerComponent, DoTickSideScroller > > ), TickTypes::Gameplay )

void TickSideScroller::DefineContract( Helium::TaskContract &rContract )
{
	rContract.ExecuteAfter<StandardDependencies::PostPhysicsGameplay>();
}
