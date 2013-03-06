
#pragma once

#include "Bullet/Bullet.h"
#include "Engine/TaskScheduler.h"

namespace Helium
{
    void HELIUM_BULLET_API ProcessWorlds();

    struct HELIUM_BULLET_API BulletTaskProcessWorlds : public TaskDefinition
    {
        HELIUM_DECLARE_TASK(BulletTaskProcessWorlds, ProcessWorlds)

        virtual void DefineContract(TaskContract &rContract)
        {
            //rContract.ExecuteAfterTask<MyTaskDefinition2>();
        }
    };
}
