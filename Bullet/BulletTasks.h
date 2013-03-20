
#pragma once

#include "Bullet/Bullet.h"
#include "Engine/TaskScheduler.h"

namespace Helium
{
    struct HELIUM_BULLET_API BulletTaskProcessWorlds : public TaskDefinition
    {
        HELIUM_DECLARE_TASK(BulletTaskProcessWorlds)

        virtual void DefineContract(TaskContract &rContract);
    };
}
