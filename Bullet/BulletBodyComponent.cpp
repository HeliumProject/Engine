#include "BulletPch.h"
#include "Bullet/BulletBodyComponent.h"
#include "Reflect/TranslatorDeduction.h"
#include "Components/TransformComponent.h"

#include "Bullet/BulletWorldComponent.h"

HELIUM_IMPLEMENT_ASSET(Helium::BulletBodyComponentDefinition, Bullet, 0);

void Helium::BulletBodyComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField(&BulletBodyComponentDefinition::m_BodyDefinition, "m_BodyDefinition");
}

HELIUM_DEFINE_COMPONENT(Helium::BulletBodyComponent, 32);

void Helium::BulletBodyComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void Helium::BulletBodyComponent::Finalize( const BulletBodyComponentDefinition *pDefinition )
{
	BulletWorldComponent *pBulletWorldComponent = GetWorld()->GetComponents().GetFirst<BulletWorldComponent>();
	HELIUM_ASSERT( pBulletWorldComponent );

	TransformComponent *pTransform = GetComponentCollection()->GetFirst<TransformComponent>();
	HELIUM_ASSERT( pTransform );
	
	m_Body.Initialize(
		*pBulletWorldComponent->GetBulletWorld(), 
		*pDefinition->m_BodyDefinition, 
		pTransform->GetPosition(), 
		pTransform->GetRotation());
}
