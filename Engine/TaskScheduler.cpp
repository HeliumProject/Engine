
#include "EnginePch.h"
#include "TaskScheduler.h"

using namespace Helium;


TaskDefinition *TaskDefinition::s_FirstTaskDefinition = NULL;
A_TaskDefinition TaskScheduler::m_ScheduleInfo;
DynamicArray<TaskFunc> TaskScheduler::m_ScheduleFunc;

bool InsertToTaskList(A_TaskDefinition &rTaskInfoList, DynamicArray<TaskFunc> &rTaskFuncList, A_TaskDefinition &rTaskStack, const TaskDefinition *pTask);

bool TaskScheduler::CalculateSchedule()
{
    typedef Helium::Map<const DependencyDefinition *, A_TaskDefinition > M_DependencyTaskMap;

    M_DependencyTaskMap dependencyContributingTaskMap;
        
    // For each task
    TaskDefinition *task = TaskDefinition::s_FirstTaskDefinition;
    while (task)
    {
        task->DoDefineContract();
        task = task->m_Next;
    }

    // For each task
    task = TaskDefinition::s_FirstTaskDefinition;
    while (task)
    {
        // Look at all of its dependencies
        for (DynamicArray<const DependencyDefinition *>::Iterator dependency_iter = task->m_Contract.m_ContributedDependencies.Begin();
            dependency_iter != task->m_Contract.m_ContributedDependencies.End(); ++dependency_iter)
        {
            // And for each of those dependencies, insert an entry into the dependency map
            M_DependencyTaskMap::Iterator map_entry = dependencyContributingTaskMap.Find(*dependency_iter);
            if (map_entry == dependencyContributingTaskMap.End())
            {
                A_TaskDefinition value;
                value.Add(task);
                dependencyContributingTaskMap.Insert(map_entry, M_DependencyTaskMap::ValueType(*dependency_iter, value));
            }
            else
            {
                map_entry->Second().Add(task);
            }
        }
        
        task = task->m_Next;
    }

    // For each task
    task = TaskDefinition::s_FirstTaskDefinition;
    while (task)
    {
        // Look at the order requirements
        for (DynamicArray<OrderRequirement>::Iterator requirement = task->m_Contract.m_OrderRequirements.Begin();
            requirement != task->m_Contract.m_OrderRequirements.End(); ++requirement)
        {
            // Find all the tasks that contribute to the order requirement's dependency
            M_DependencyTaskMap::Iterator map_iter = dependencyContributingTaskMap.Find(requirement->m_Dependency);
            for (A_TaskDefinition::Iterator inner_task_iter = map_iter->Second().Begin();
                inner_task_iter != map_iter->Second().End(); ++inner_task_iter)
            {
                if (requirement->m_Type == OrderRequirementTypes::Before)
                {
                    // If this task needs to go *before* something, make all those tasks depend on us
                    (*inner_task_iter)->m_RequiredTasks.Add(task);
                }
                else
                {
                    // Make us depend on all those tasks
                    task->m_RequiredTasks.Add(*inner_task_iter);
                }
            }
        }

        task = task->m_Next;
    }

    A_TaskDefinition taskStack;
    
    task = TaskDefinition::s_FirstTaskDefinition;
    while (task)
    {
        if (!InsertToTaskList(m_ScheduleInfo, m_ScheduleFunc, taskStack, task))
        {
            m_ScheduleInfo.Clear();
            m_ScheduleFunc.Clear();
            return false;
        }

        task = task->m_Next;
    }
    
    HELIUM_TRACE(TraceLevels::Info, TXT( "Successfully generated a schedule for all tasks.\n" ));
    HELIUM_TRACE(TraceLevels::Debug, TXT( "Calculated task schedule:\n" ));
    for (A_TaskDefinition::Iterator iter = m_ScheduleInfo.Begin();
        iter != m_ScheduleInfo.End(); ++iter)
    {
        HELIUM_TRACE(TraceLevels::Debug, TXT( " - %s\n" ), (*iter)->m_Name );
    }

    return true;
}

bool InsertToTaskList(A_TaskDefinition &rTaskInfoList, DynamicArray<TaskFunc> &rTaskFuncList, A_TaskDefinition &rTaskStack, const TaskDefinition *pTask)
{
    for (int i = 0; i < rTaskStack.GetSize(); ++i)
    {
        if (rTaskStack[i] == pTask)
        {
            HELIUM_TRACE(TraceLevels::Error, TXT( "Dependency cycle detected in task scheduler. Verify both explicit task order requirements as well as "
                "indirect order requirements by fulfilling/requiring DependencyDefinitions. Dependency cycle is:\n" ));

            rTaskStack.Push(pTask);

            HELIUM_ASSERT(rTaskStack.GetSize() > 1);
            for (; i < rTaskStack.GetSize() - 1; ++i)
            {
                const TaskDefinition *pBeforeTask = rTaskStack[i];
                const TaskDefinition *pAfterTask = rTaskStack[i + 1];
                HELIUM_TRACE(TraceLevels::Error, TXT( " - %s must execute after %s\n"), pBeforeTask->m_Name, pAfterTask->m_Name);
            }

            rTaskStack.Pop();

            // TODO: Spew the contracts of the tasks in the cycle
            return false;
        }
    }

    bool already_inserted = false;
    for (A_TaskDefinition::Iterator iter = rTaskInfoList.Begin();
        iter != rTaskInfoList.End(); ++iter)
    {
        if (*iter == pTask)
        {
            already_inserted = true;
            break;
        }
    }

    if (already_inserted)
    {
        return true;
    }

    rTaskStack.Push(pTask);

    for (A_TaskDefinition::Iterator prior_task_iter = pTask->m_RequiredTasks.Begin();
        prior_task_iter != pTask->m_RequiredTasks.End(); ++prior_task_iter)
    {
        if (!InsertToTaskList(rTaskInfoList, rTaskFuncList, rTaskStack, *prior_task_iter))
        {
            rTaskStack.Pop();
            return false;
        }
    }

    rTaskInfoList.Add(pTask);
    rTaskFuncList.Add(pTask->m_Func);
    rTaskStack.Pop();
    return true;
}

void TaskScheduler::ExecuteSchedule()
{
    int i = 0;
    for (DynamicArray<TaskFunc>::Iterator iter = m_ScheduleFunc.Begin(); iter != m_ScheduleFunc.End(); ++iter)
    {
        (*iter)();
        HELIUM_ASSERT(m_ScheduleInfo[i++]->m_Func == *iter);
    }
}

Helium::DependencyDefinition Helium::StandardDependencies::g_ReceiveInput;
Helium::DependencyDefinition Helium::StandardDependencies::g_ProcessPhysics;
Helium::DependencyDefinition Helium::StandardDependencies::g_Render;
