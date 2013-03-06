
#pragma once

#include "Engine/Engine.h"

#define HELIUM_DECLARE_TASK(__Type, __Function)                 \
        __Type()                                                \
            : TaskDefinition(m_Dependency, __Function, #__Type) \
        {                                                       \
                                                                \
        }                                                       \
        static Helium::DependencyDefinition m_Dependency;       \
        static __Type m_This; 


#define HELIUM_DEFINE_TASK(__Type)                      \
    __Type __Type::m_This;                              \
    Helium::DependencyDefinition __Type::m_Dependency;

namespace Helium
{
    // Declared statically to specify a "thing" that will be completed during the frame (i.e. PhysicsIsComplete)
    // We allow custom dependencies to allow tasks to stay decoupled
    class DependencyDefinition
    {

    };
    
    namespace OrderRequirementTypes
    {
        enum OrderRequirementType
        {
            Before,
            After, 
        };
    }
    typedef OrderRequirementTypes::OrderRequirementType OrderRequirementType;

    struct OrderRequirement
    {
        const DependencyDefinition *m_Dependency;
        OrderRequirementType m_Type;
    };

    // Defines what the task expects and what it provides
    struct TaskContract
    {
        // Task T must execute before this task
        template <class T>
        void ExecuteBeforeTask()
        {
            ExecuteBeforeDependency(T::m_Dependency);
        }

        // Task T must execute after this task
        template <class T>
        void ExecuteAfterTask()
        {
            ExecuteAfterDependency(T::m_Dependency);
        }

        void ExecuteBeforeDependency(DependencyDefinition &rDependency)
        {
            OrderRequirement *requirement = m_OrderRequirements.New();
            requirement->m_Dependency = &rDependency;
            requirement->m_Type = OrderRequirementTypes::Before;
        }

        // Task T must execute after this task
        void ExecuteAfterDependency(DependencyDefinition &rDependency)
        {
            OrderRequirement *requirement = m_OrderRequirements.New();
            requirement->m_Dependency = &rDependency;
            requirement->m_Type = OrderRequirementTypes::After;
        }
        
        void FulfillsDependency(const DependencyDefinition &rDependency)
        {
            m_ContributedDependencies.Push(&rDependency);
        }

        // Every requirement to be before or after another dependency goes here
        DynamicArray<OrderRequirement> m_OrderRequirements;

        // All dependencies we contribute to fulfilling
        DynamicArray<const DependencyDefinition *> m_ContributedDependencies;
    };

    typedef void (*TaskFunc)();

    struct HELIUM_ENGINE_API TaskDefinition
    {
        TaskDefinition(const DependencyDefinition &rDependency, TaskFunc pFunc, const tchar_t *pName)
            : m_Func(pFunc),
              m_Next(s_FirstTaskDefinition),
              m_Name(pName)
        {
            m_Contract.FulfillsDependency(rDependency);

            s_FirstTaskDefinition = this;
        }
        
        // Scaffolding to allow child classes to define their contract
        virtual void DefineContract(TaskContract &) = 0;
        void DoDefineContract()
        {
            DefineContract(m_Contract);
        }
        
        // We build this list of tasks that must execute before us in TaskScheduler::CalculateSchedule()
        mutable DynamicArray<const TaskDefinition *> m_RequiredTasks;

        // Task name useful for debug purposes
        const tchar_t *m_Name;

        // Our contract to be filled out by subclass
        TaskContract m_Contract;

        // The callback that will execute this task
        TaskFunc m_Func;

        // Support for maintaining a linked list of all created task definitions (only one per type should ever exist)
        TaskDefinition *m_Next;
        static TaskDefinition *s_FirstTaskDefinition;
    };
    typedef DynamicArray<const TaskDefinition *> A_TaskDefinition;

    class HELIUM_ENGINE_API TaskScheduler
    {
    public:
        static bool CalculateSchedule();
        static void ExecuteSchedule();

        static A_TaskDefinition m_ScheduleInfo;
        static DynamicArray<TaskFunc> m_ScheduleFunc; // Compact version of our schedule
    };

    
    //////////////////////////////////////////////////////////////////////////


    
    HELIUM_ENGINE_API extern Helium::DependencyDefinition g_MyDependency;

    HELIUM_ENGINE_API void MyTask();

    struct HELIUM_ENGINE_API MyTaskDefinition : public TaskDefinition
    {
        HELIUM_DECLARE_TASK(MyTaskDefinition, MyTask)
        
        virtual void DefineContract(TaskContract &rContract)
        {
            rContract.ExecuteAfterDependency(g_MyDependency);
        }
    };
    
    HELIUM_ENGINE_API void MyTask2();

    struct HELIUM_ENGINE_API MyTaskDefinition2 : public TaskDefinition
    {
        HELIUM_DECLARE_TASK(MyTaskDefinition2, MyTask2)

        virtual void DefineContract(TaskContract &rContract)
        {
            rContract.ExecuteBeforeTask<MyTaskDefinition>();
            rContract.FulfillsDependency(g_MyDependency);
        }
    };

    HELIUM_ENGINE_API void MyTask3();

    struct HELIUM_ENGINE_API MyTaskDefinition3 : public TaskDefinition
    {
        HELIUM_DECLARE_TASK(MyTaskDefinition3, MyTask3)

        virtual void DefineContract(TaskContract &rContract)
        {
            rContract.ExecuteAfterTask<MyTaskDefinition2>();
            rContract.FulfillsDependency(g_MyDependency);
        }
    };
}
