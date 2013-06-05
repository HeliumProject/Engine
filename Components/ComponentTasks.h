
#pragma once 

#include "Components/Components.h"
#include "Framework/TaskScheduler.h"

namespace Helium
{
    struct HELIUM_COMPONENTS_API ClearTransformComponentDirtyFlagsTask : public TaskDefinition
    {
        HELIUM_DECLARE_TASK(ClearTransformComponentDirtyFlagsTask)
        virtual void DefineContract(TaskContract &rContract);
    };
        
    struct HELIUM_COMPONENTS_API UpdateRotatorComponentsTask : public TaskDefinition
    {
        HELIUM_DECLARE_TASK(UpdateRotatorComponentsTask)
        virtual void DefineContract(TaskContract &rContract);
    };
        
    struct HELIUM_COMPONENTS_API UpdateMeshComponentsTask : public TaskDefinition
    {
        HELIUM_DECLARE_TASK(UpdateMeshComponentsTask)
        virtual void DefineContract(TaskContract &rContract);
    };
}
