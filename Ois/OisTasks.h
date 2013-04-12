#pragma once

#include "Ois/Ois.h"
#include "Engine/TaskScheduler.h"

namespace Helium
{
    struct HELIUM_OIS_API OisTaskCapture : public TaskDefinition
    {
        HELIUM_DECLARE_TASK(OisTaskCapture)

        virtual void DefineContract(TaskContract &rContract);
    };
}