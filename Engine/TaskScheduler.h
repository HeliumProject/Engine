
#pragma once

#include "Engine/Engine.h"

#include "Foundation/DynamicArray.h"

#define HELIUM_DECLARE_TASK(__Type)                         \
        __Type();                                           \
        static Helium::DependencyDefinition m_Dependency;   \
        static __Type m_This; 


#define HELIUM_DEFINE_TASK(__Type, __Function)              \
    __Type __Type::m_This;                                  \
    Helium::DependencyDefinition __Type::m_Dependency;      \
    __Type::__Type()                                        \
        : TaskDefinition(m_Dependency, __Function, #__Type) \
    {                                                       \
                                                            \
    }

// Abstract tasks are used when you want a conceptual thing like "render" to be a dependency that other tasks
// can say they go before, after, or fulfill. This allows us to generally define a few high-level stages and 
// let client code non-intrusively hook their logic to run within these stages, or even
// define their own concepts for other code to non-obtrusively hook. This also lets us decouple tasks from each
// other (i.e. rather than AI requiring OIS input to be pulled, it can require the abstract concept of "ReceiveInput",
// which an OIS input grabbing task might fulfill, ensuring that the AI need not "know" about OIS)
#define HELIUM_DEFINE_ABSTRACT_TASK(__Type)                 \
    __Type __Type::m_This;                                  \
    Helium::DependencyDefinition __Type::m_Dependency;      \
    __Type::__Type()                                        \
        : TaskDefinition(m_Dependency, 0, #__Type)          \
    {                                                       \
                                                            \
    }


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
        void ExecuteBefore()
        {
            ExecuteBefore(T::m_Dependency);
        }

        // Task T must execute after this task
        template <class T>
        void ExecuteAfter()
        {
            ExecuteAfter(T::m_Dependency);
        }
        
        void ExecuteBefore(DependencyDefinition &rDependency)
        {
            OrderRequirement *requirement = m_OrderRequirements.New();
            requirement->m_Dependency = &rDependency;
            requirement->m_Type = OrderRequirementTypes::Before;
        }
                
        void ExecuteAfter(DependencyDefinition &rDependency)
        {
            OrderRequirement *requirement = m_OrderRequirements.New();
            requirement->m_Dependency = &rDependency;
            requirement->m_Type = OrderRequirementTypes::After;
        }
        
        template <class T>
        void Fulfills()
        {
            Fulfills(T::m_Dependency);
        }
        
        void Fulfills(const DependencyDefinition &rDependency)
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
            : m_DependencyReverseLookup(rDependency),
              m_Func(pFunc),
              m_Next(s_FirstTaskDefinition),
              m_Name(pName)
        {
            m_Contract.Fulfills(rDependency);

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
        
        const DependencyDefinition &m_DependencyReverseLookup;

        // Support for maintaining a linked list of all created task definitions (only one per type should ever exist)
        TaskDefinition *m_Next;
        static TaskDefinition *s_FirstTaskDefinition;
    };
    typedef DynamicArray<const TaskDefinition *> A_TaskDefinitionPtr;

    class HELIUM_ENGINE_API TaskScheduler
    {
    public:
        static bool CalculateSchedule();
        static void ExecuteSchedule();

        static A_TaskDefinitionPtr m_ScheduleInfo;
        static DynamicArray<TaskFunc> m_ScheduleFunc; // Compact version of our schedule
    };

    namespace StandardDependencies
    {
        struct HELIUM_ENGINE_API ReceiveInput : public TaskDefinition
        {
            HELIUM_DECLARE_TASK(ReceiveInput);
            virtual void DefineContract(TaskContract &r);
        };
                
        struct HELIUM_ENGINE_API ProcessPhysics : public TaskDefinition
        {
            HELIUM_DECLARE_TASK(ProcessPhysics);
            virtual void DefineContract(TaskContract &r);
        };
              
        struct HELIUM_ENGINE_API Render : public TaskDefinition
        {
            HELIUM_DECLARE_TASK(Render);
            virtual void DefineContract(TaskContract &r);
        };
    };
}
