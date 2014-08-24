#include "GameLibraryPch.h"

#include "Damaged.h"
#include "Framework/WorldManager.h"


using namespace Helium;
using namespace GameLibrary;

//////////////////////////////////////////////////////////////////////////
// DamagedComponent

HELIUM_DEFINE_COMPONENT(GameLibrary::DamagedComponent, 128);

void DamagedComponent::PopulateMetaType( Reflect::MetaStruct& comp )
{

}

void DamagedComponent::Initialize( float damageAmount )
{
	m_DamageAmount = damageAmount;
}
