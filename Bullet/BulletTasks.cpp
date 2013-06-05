
#include "BulletPch.h"
#include "Bullet/BulletTasks.h"
#include "Bullet/BulletWorldComponent.h"
#include "Framework/ComponentQuery.h"
#include "Framework/WorldManager.h"

using namespace Helium;

void ProcessWorlds(BulletWorldComponent *pWorld)
{
	pWorld->Simulate( WorldManager::GetStaticInstance().GetFrameDeltaSeconds() );
}

void Helium::BulletTaskProcessWorlds::DefineContract( TaskContract &rContract )
{
	rContract.Fulfills<StandardDependencies::ProcessPhysics>();
}

HELIUM_DEFINE_TASK(BulletTaskProcessWorlds, (ForEachWorld< QueryComponents< BulletWorldComponent, ProcessWorlds > > ) )
