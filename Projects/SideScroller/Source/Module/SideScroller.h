#pragma once

#include "Engine/Asset.h"
#include "Framework/ComponentDefinition.h"
#include "Framework/TaskScheduler.h"
#include "Framework/StateMachine.h"

class SideScrollerComponentDefinition;

typedef Helium::StrongPtr<SideScrollerComponentDefinition> SideScrollerComponentDefinitionPtr;	
typedef Helium::StrongPtr<const SideScrollerComponentDefinition> ConstSideScrollerComponentDefinitionPtr;

struct SideScrollerComponent : public Helium::Component
{
	HELIUM_DECLARE_COMPONENT( SideScrollerComponent, Helium::Component );
	static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

	void Initialize( const SideScrollerComponentDefinition &definition);
	void Tick();

	Helium::StateMachineInstance m_StateMachine;
};

class SideScrollerComponentDefinition : public Helium::ComponentDefinitionHelper<SideScrollerComponent, SideScrollerComponentDefinition>
{
	HELIUM_DECLARE_CLASS( SideScrollerComponentDefinition, Helium::ComponentDefinition );
	static void PopulateMetaType( Helium::Reflect::MetaStruct& comp );

	Helium::StateMachineDefinitionPtr m_StateMachine;
};

struct TickSideScroller : public Helium::TaskDefinition
{
	HELIUM_DECLARE_TASK(TickSideScroller)
	virtual void DefineContract(Helium::TaskContract &rContract);
};
