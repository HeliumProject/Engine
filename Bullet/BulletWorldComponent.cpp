#include "BulletPch.h"
#include "Bullet/BulletWorldComponent.h"
#include "Engine/AssetType.h"
#include "Reflect/TranslatorDeduction.h"

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
