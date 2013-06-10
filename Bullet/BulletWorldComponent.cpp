#include "BulletPch.h"
#include "Bullet/BulletWorldComponent.h"
#include "Reflect/TranslatorDeduction.h"
#include "Framework/WorldManager.h"
#include "Framework/ComponentQuery.h"

using namespace Helium;

HELIUM_IMPLEMENT_ASSET(Helium::BulletWorldComponentDefinition, Bullet, 0);

void Helium::BulletWorldComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField(&BulletWorldComponentDefinition::m_WorldDefinition, "m_WorldDefinition");
}

HELIUM_DEFINE_COMPONENT(Helium::BulletWorldComponent, 32);

void Helium::BulletWorldComponent::PopulateStructure( Reflect::Structure& comp )
{

}

Helium::BulletWorldComponent::BulletWorldComponent()
	: m_World(0)
{
	
}

Helium::BulletWorldComponent::~BulletWorldComponent()
{
	delete m_World;
	m_World = 0;
}

void Helium::BulletWorldComponent::Finalize( const BulletWorldComponentDefinition *pDefinition )
{
	HELIUM_ASSERT(!m_World);
	m_World = new BulletWorld();
	m_World->Initialize(*pDefinition->m_WorldDefinition);
}

void Helium::BulletWorldComponent::Simulate( float dt )
{
	m_World->Simulate(dt);
}

//////////////////////////////////////////////////////////////////////////

void DoProcessPhysics( BulletWorldComponent *pComponent )
{
	pComponent->Simulate(WorldManager::GetStaticInstance().GetFrameDeltaSeconds());
};

HELIUM_DEFINE_TASK( ProcessPhysics, (ForEachWorld< QueryComponents< BulletWorldComponent, DoProcessPhysics > >) )

void ProcessPhysics::DefineContract( Helium::TaskContract &rContract )
{
	rContract.Fulfills<Helium::StandardDependencies::ProcessPhysics>();
}
