
#include "BulletPch.h"
#include "Bullet/BulletWorldComponent.h"
#include "Engine/AssetType.h"

HELIUM_IMPLEMENT_ASSET(Helium::BulletWorldComponentDefinition, Bullet, 0);

void Helium::BulletWorldComponentDefinition::PopulateComposite( Reflect::Composite& comp )
{
	comp.AddField(&BulletWorldComponentDefinition::m_WorldDefinition, "m_WorldDefinition");
}

HELIUM_DEFINE_COMPONENT(Helium::BulletWorldComponent, 32);

void Helium::BulletWorldComponent::PopulateComposite( Reflect::Composite& comp )
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
