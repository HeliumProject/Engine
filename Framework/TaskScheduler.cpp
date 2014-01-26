
#include "FrameworkPch.h"
#include "TaskScheduler.h"
#include "Foundation/Map.h"

using namespace Helium;


TaskDefinition *TaskDefinition::s_FirstTaskDefinition = NULL;
bool TaskScheduler::m_ContractsDefined = false;

bool InsertToTaskList(A_TaskDefinitionPtr &rTaskInfoList, DynamicArray<TaskFunc> &rTaskFuncList, A_TaskDefinitionPtr &rTaskStack, const TaskDefinition *pTask, uint32_t tickType);

bool TaskScheduler::CalculateSchedule(uint32_t tickType, TaskSchedule &schedule)
{	
	// Call DoDefineContract on everything once, if we haven't already done so
	if (!TaskScheduler::m_ContractsDefined)
	{
		typedef DynamicArray<TaskDefinition *> A_TaskDefinitionPtrNonConst;
		typedef Helium::Map<const TaskDefinition *, A_TaskDefinitionPtrNonConst > M_DependencyTaskMap;
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
			for (DynamicArray<const TaskDefinition *>::ConstIterator dependency_iter = task->m_Contract.m_ContributedDependencies.Begin();
				dependency_iter != task->m_Contract.m_ContributedDependencies.End(); ++dependency_iter)
			{
				// And for each of those dependencies, insert an entry into the dependency map
				M_DependencyTaskMap::Iterator map_entry = dependencyContributingTaskMap.Find(*dependency_iter);
				if (map_entry == dependencyContributingTaskMap.End())
				{
					A_TaskDefinitionPtrNonConst value;
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
			// Find all tasks that contribute to the task with the order requirement
			M_DependencyTaskMap::Iterator outer_map_iter = dependencyContributingTaskMap.Find(&task->m_DependencyReverseLookup);
			HELIUM_ASSERT( outer_map_iter != dependencyContributingTaskMap.End() );

			// Look at the order requirements
			for (DynamicArray<OrderRequirement>::Iterator requirement = task->m_Contract.m_OrderRequirements.Begin();
				requirement != task->m_Contract.m_OrderRequirements.End(); ++requirement)
			{
				// Find all the tasks that contribute to the order requirement's dependency
				M_DependencyTaskMap::Iterator inner_map_iter = dependencyContributingTaskMap.Find(requirement->m_Dependency);
				HELIUM_ASSERT( inner_map_iter != dependencyContributingTaskMap.End() );

				// For every before/after pair, apply the constraint
				for (A_TaskDefinitionPtrNonConst::Iterator outer_task_iter = outer_map_iter->Second().Begin();
					outer_task_iter != outer_map_iter->Second().End(); ++outer_task_iter)
				{
					for (A_TaskDefinitionPtrNonConst::Iterator inner_task_iter = inner_map_iter->Second().Begin();
						inner_task_iter != inner_map_iter->Second().End(); ++inner_task_iter)
					{
						if (requirement->m_Type == OrderRequirementTypes::Before)
						{
							// If this task needs to go *before* something, make all those tasks depend on us
							(*inner_task_iter)->m_RequiredTasks.Add(*outer_task_iter);
						}
						else
						{
							// Make us depend on all those tasks
							(*outer_task_iter)->m_RequiredTasks.Add(*inner_task_iter);
						}
					}
				}
			}

			task = task->m_Next;
		}

		TaskScheduler::m_ContractsDefined = true;
	}
	
	A_TaskDefinitionPtr taskStack;
	
	const TaskDefinition *task = TaskDefinition::s_FirstTaskDefinition;
	while (task)
	{
		// Drop any task we don't want to run
		if (!InsertToTaskList(schedule.m_ScheduleInfo, schedule.m_ScheduleFunc, taskStack, task, tickType))
		{
			schedule.m_ScheduleInfo.Clear();
			schedule.m_ScheduleFunc.Clear();
			return false;
		}

		task = task->m_Next;
	}
	
	HELIUM_TRACE(TraceLevels::Info, TXT( "Successfully generated a schedule for all tasks.\n" ));

#if HELIUM_TOOLS
	HELIUM_TRACE(TraceLevels::Debug, TXT( "Calculated task schedule:\n" ));
	for (A_TaskDefinitionPtr::Iterator iter = schedule.m_ScheduleInfo.Begin();
		iter != schedule.m_ScheduleInfo.End(); ++iter)
	{
		if ( !(*iter)->m_Func )
		{
			continue;
		}
		
		HELIUM_TRACE(
			TraceLevels::Debug, 
			(*iter)->m_Func ? TXT(" - %s") : TXT(" - *%s"), 
			(*iter)->m_Name );

		int count = 0;

		for (DynamicArray<const TaskDefinition *>::ConstIterator dependency_iter = (*iter)->m_Contract.m_ContributedDependencies.Begin();
			dependency_iter != (*iter)->m_Contract.m_ContributedDependencies.End(); ++dependency_iter)
		{
			if (*iter == *dependency_iter)
			{
				continue;
			}

			if (!count)
			{
				HELIUM_TRACE(TraceLevels::Debug, " - [ ");
			}

			HELIUM_TRACE(
				TraceLevels::Debug, 
				count ? ", %s" : "%s", 
				(*dependency_iter)->m_Name);

			++count;
		}

		HELIUM_TRACE(TraceLevels::Debug, count ? " ]\n" : "\n");
	}

#if 0
	task = TaskDefinition::s_FirstTaskDefinition;
	while (task)
	{
		HELIUM_TRACE(TraceLevels::Debug, TXT( "\nTASK INFO FOR %s:" ), task->m_Name);
		HELIUM_TRACE(TraceLevels::Debug, TXT( "\n  Required Tasks:" ));
		for (A_TaskDefinitionPtr::Iterator iter = task->m_RequiredTasks.Begin();
			iter != task->m_RequiredTasks.End(); ++iter)
		{
			HELIUM_TRACE(TraceLevels::Debug, TXT( "  %s" ), (*iter)->m_Name);
		}
		task = task->m_Next;
	}
#endif
#endif
	
	size_t i_copy_to = 0;
	size_t i_copy_from = 0;
	const size_t taskCount = schedule.m_ScheduleFunc.GetSize();

	// Clear out the abstract tasks as they are not relevant once a final order is calculated
	while (i_copy_from < taskCount)
	{
		if (schedule.m_ScheduleFunc[i_copy_from] == 0)
		{
			++i_copy_from;
			continue;
		}

		if (i_copy_from != i_copy_to)
		{
			schedule.m_ScheduleFunc[i_copy_to] = schedule.m_ScheduleFunc[i_copy_from];
			schedule.m_ScheduleInfo[i_copy_to] = schedule.m_ScheduleInfo[i_copy_from];
		}

		++i_copy_from;
		++i_copy_to;
	}

	schedule.m_ScheduleFunc.Resize(i_copy_to);
	schedule.m_ScheduleInfo.Resize(i_copy_to);

#if HELIUM_ASSERT_ENABLED
	for (DynamicArray<TaskFunc>::Iterator iter = schedule.m_ScheduleFunc.Begin();
		iter != schedule.m_ScheduleFunc.End(); ++iter)
	{
		HELIUM_ASSERT(*iter);
	}
#endif

	return true;
}

bool InsertToTaskList(A_TaskDefinitionPtr &rTaskInfoList, DynamicArray<TaskFunc> &rTaskFuncList, A_TaskDefinitionPtr &rTaskStack, const TaskDefinition *pTask, uint32_t tickType)
{
	// Don't add functions that do not run under the given tick type
	if ((pTask->m_Contract.m_TickType & tickType) == 0)
	{
		return true;
	}

	for (size_t i = 0; i < rTaskStack.GetSize(); ++i)
	{
		if (rTaskStack[i] == pTask)
		{
#if HELIUM_TOOLS
			HELIUM_TRACE(TraceLevels::Error, TXT( "Dependency cycle detected in task scheduler. Verify both explicit task order requirements as well as "
				"indirect order requirements by fulfilling/requiring TaskDefinitions. Dependency cycle is:\n" ));

			rTaskStack.Push(pTask);

			HELIUM_ASSERT(rTaskStack.GetSize() > 1);
			for (; i < rTaskStack.GetSize() - 1; ++i)
			{
				const TaskDefinition *pBeforeTask = rTaskStack[i];
				const TaskDefinition *pAfterTask = rTaskStack[i + 1];

				HELIUM_TRACE(TraceLevels::Error, TXT( " - %s must execute after %s\n"), pBeforeTask->m_Name, pAfterTask->m_Name);
			}
#else
			HELIUM_TRACE(TraceLevels::Error, TXT( "Dependency cycle detected in task scheduler. Verify both explicit task order requirements as well as "
				"indirect order requirements by fulfilling/requiring TaskDefinitions. Dependency cycle would be printed here if HELIUM_TOOLS=1\n" ));
#endif

			rTaskStack.Pop();

			// TODO: Spew the contracts of the tasks in the cycle
			return false;
		}
	}

	bool already_inserted = false;
	for (A_TaskDefinitionPtr::Iterator iter = rTaskInfoList.Begin();
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

	for (A_TaskDefinitionPtr::ConstIterator prior_task_iter = pTask->m_RequiredTasks.Begin();
		prior_task_iter != pTask->m_RequiredTasks.End(); ++prior_task_iter)
	{
		if (!InsertToTaskList(rTaskInfoList, rTaskFuncList, rTaskStack, *prior_task_iter, tickType))
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

void TaskScheduler::ExecuteSchedule( const TaskSchedule &schedule, DynamicArray< WorldPtr > &rWorlds )
{
	int i = 0;
	for (DynamicArray<TaskFunc>::ConstIterator iter = schedule.m_ScheduleFunc.Begin(); iter != schedule.m_ScheduleFunc.End(); ++iter)
	{
		(*iter)( rWorlds );
		HELIUM_ASSERT(schedule.m_ScheduleInfo[i++]->m_Func == *iter);
	}
}

void Helium::TaskScheduler::ResetContracts()
{
	TaskDefinition *task = TaskDefinition::s_FirstTaskDefinition;
	while (task)
	{
		task->m_RequiredTasks.Clear();
		task->m_Contract.m_ContributedDependencies.Clear();
		task->m_Contract.m_OrderRequirements.Clear();
		task = task->m_Next;
	}

	m_ContractsDefined = false;
}

using namespace Helium::StandardDependencies;

HELIUM_DEFINE_ABSTRACT_TASK(ReceiveInput);
void Helium::StandardDependencies::ReceiveInput::DefineContract( TaskContract &rContract )
{
	rContract.ExecuteBefore<StandardDependencies::PrePhysicsGameplay>();
}

HELIUM_DEFINE_ABSTRACT_TASK(PrePhysicsGameplay);
void Helium::StandardDependencies::PrePhysicsGameplay::DefineContract( TaskContract &rContract )
{
	rContract.ExecuteBefore<StandardDependencies::ProcessPhysics>();
}

HELIUM_DEFINE_ABSTRACT_TASK(ProcessPhysics);
void Helium::StandardDependencies::ProcessPhysics::DefineContract( TaskContract &rContract )
{

}

HELIUM_DEFINE_ABSTRACT_TASK(PostPhysicsGameplay);
void Helium::StandardDependencies::PostPhysicsGameplay::DefineContract( TaskContract &rContract )
{
	rContract.ExecuteAfter<StandardDependencies::ProcessPhysics>();
}

HELIUM_DEFINE_ABSTRACT_TASK(Render);
void Helium::StandardDependencies::Render::DefineContract( TaskContract &rContract )
{
	rContract.ExecuteAfter<StandardDependencies::PostPhysicsGameplay>();
}

HELIUM_DEFINE_ABSTRACT_TASK(PostRender);
void Helium::StandardDependencies::PostRender::DefineContract( TaskContract &rContract )
{
	rContract.ExecuteAfter<StandardDependencies::Render>();
}
