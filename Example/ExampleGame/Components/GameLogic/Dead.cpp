#include "ExampleGamePch.h"

#include "Dead.h"
#include "Framework/WorldManager.h"


using namespace Helium;
using namespace ExampleGame;

//////////////////////////////////////////////////////////////////////////
// DeadComponent

HELIUM_DEFINE_COMPONENT(ExampleGame::DeadComponent, 128);

void DeadComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void DeadComponent::Initialize( float damageAmount )
{
	m_DamageAmount = m_DamageAmount;
}
