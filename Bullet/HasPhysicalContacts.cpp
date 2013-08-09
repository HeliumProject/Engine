#include "BulletPch.h"
#include "Bullet/HasPhysicalContacts.h"
#include "Reflect/TranslatorDeduction.h"
#include "Components/TransformComponent.h"

#include "Framework/ComponentQuery.h"

using namespace Helium;

HELIUM_DEFINE_COMPONENT(Helium::HasPhysicalContactsComponent, 128);

void Helium::HasPhysicalContactsComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

Helium::HasPhysicalContactsComponent::~HasPhysicalContactsComponent()
{
	m_BeginTouch.Clear();
	m_EndFrameTouching.Clear();
	m_EndTouch.Clear();
	m_BeginFrameTouching.Clear();
	m_EverTouchedThisFrame.Clear();
}
