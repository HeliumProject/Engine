
#include "BulletPch.h"
#include "Bullet/Bullet.h"
#include "Bullet/BulletEngine.h"
#include "Bullet/BulletBodyComponent.h"

#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

void Bullet::Initialize()
{
	// Don't need to do anything.. yet
	// We probably will want to do something like set up custom memory allocation
}

void Bullet::Cleanup()
{
	// Don't need to do anything.. yet
}

HELIUM_IMPLEMENT_ASSET( Helium::BulletSystemComponent, Bullet, 0 )

void Helium::BulletSystemComponent::Initialize()
{
	ms_Instance = this;
	HELIUM_ASSERT( !m_BodyFlags || m_BodyFlags->GetFlagCount() < BulletBodyComponent::MAX_BULLET_BODY_FLAGS );
}

void Helium::BulletSystemComponent::FinalizeInit()
{
	Bullet::Initialize();
}

void Helium::BulletSystemComponent::Cleanup()
{
	Bullet::Cleanup();
}

void Helium::BulletSystemComponent::Destroy()
{
	ms_Instance = NULL;
}

void BulletSystemComponent::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField( &BulletSystemComponent::m_BodyFlags, "m_BodyFlags" );
}

BulletSystemComponent *BulletSystemComponent::ms_Instance = NULL;