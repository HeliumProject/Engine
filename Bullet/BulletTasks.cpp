
#include "BulletPch.h"
#include "Bullet/BulletTasks.h"
#include "Bullet/BulletWorldComponent.h"
#include "Framework/ComponentQuery.h"

using namespace Helium;

void ProcessWorlds(BulletWorldComponent *pWorld)
{
    pWorld->Simulate(0.01f);
}

void Helium::BulletTaskProcessWorlds::DefineContract( TaskContract &rContract )
{
    rContract.Fulfills<StandardDependencies::ProcessPhysics>();
}

HELIUM_DEFINE_TASK(BulletTaskProcessWorlds, (ForEachWorld< QueryComponents< BulletWorldComponent, ProcessWorlds > > ) )
