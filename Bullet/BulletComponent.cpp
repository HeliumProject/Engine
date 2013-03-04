
#include "BulletPch.h"
#include "Bullet/BulletComponent.h"
#include "Engine/AssetType.h"

HELIUM_IMPLEMENT_ASSET(Helium::BulletBodyComponentDefinition, Bullet, 0);

void Helium::BulletBodyComponentDefinition::PopulateComposite( Reflect::Composite& comp )
{
	comp.AddField(&BulletBodyComponentDefinition::m_BodyDefinition, "m_BodyDefinition");
}

HELIUM_DEFINE_COMPONENT(Helium::BulletBodyComponent, 32);

void Helium::BulletBodyComponent::PopulateComposite( Reflect::Composite& comp )
{

}


void HELIUM_BULLET_API Helium::BulletTestTaskFn()
{
    HELIUM_TRACE( TraceLevels::Info, "Bullet test task ran!\n");
}

HELIUM_DEFINE_TASK(Helium::BulletTestTask)