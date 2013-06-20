#include "ExampleGamePch.h"

#include "Damaged.h"
#include "Framework/WorldManager.h"


using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// DamagedComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::DamagedComponent, 128);

void DamagedComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void DamagedComponent::Initialize( float damageAmount )
{
	m_DamageAmount = m_DamageAmount;
}
