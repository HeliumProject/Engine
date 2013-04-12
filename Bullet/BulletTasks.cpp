
#include "BulletPch.h"
#include "Bullet/BulletTasks.h"
#include "Bullet/BulletWorldComponent.h"

using namespace Helium;

void ProcessWorlds()
{
    DynamicArray<BulletWorldComponent *> worlds;
    Components::GetAllComponents<BulletWorldComponent>(worlds);

    for (DynamicArray<BulletWorldComponent *>::Iterator iter = worlds.Begin();
        iter != worlds.End(); ++iter)
    {
        (*iter)->Simulate(0.01f);
    }
}

void Helium::BulletTaskProcessWorlds::DefineContract( TaskContract &rContract )
{
    rContract.Fulfills<StandardDependencies::ProcessPhysics>();
}

HELIUM_DEFINE_TASK(BulletTaskProcessWorlds, ProcessWorlds)