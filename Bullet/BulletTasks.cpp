
#include "BulletPch.h"
#include "Bullet/BulletTasks.h"
#include "Bullet/BulletWorldComponent.h"

using namespace Helium;

HELIUM_DEFINE_TASK(Helium::BulletTaskProcessWorlds)

void HELIUM_BULLET_API Helium::ProcessWorlds()
{
    DynamicArray<BulletWorldComponent *> worlds;
    Components::GetAllComponents<BulletWorldComponent>(worlds);

    for (DynamicArray<BulletWorldComponent *>::Iterator iter = worlds.Begin();
        iter != worlds.End(); ++iter)
    {
        (*iter)->Simulate(0.01f);
    }
}
