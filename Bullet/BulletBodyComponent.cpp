
#include "BulletPch.h"
#include "Bullet/BulletBodyComponent.h"
#include "Engine/AssetType.h"

HELIUM_IMPLEMENT_ASSET(Helium::BulletBodyComponentDefinition, Bullet, 0);

void Helium::BulletBodyComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField(&BulletBodyComponentDefinition::m_BodyDefinition, "m_BodyDefinition");
}

HELIUM_DEFINE_COMPONENT(Helium::BulletBodyComponent, 32);

void Helium::BulletBodyComponent::PopulateStructure( Reflect::Structure& comp )
{

}
