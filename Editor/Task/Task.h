#pragma once

#include "Editor/API.h"

#include "Foundation/Automation/Event.h"

namespace Helium
{
    namespace Editor
    {
        namespace TaskResults
        {
            enum TaskResult
            {
                Success,
                Failure,
                Cancel,
            };
        }
        typedef TaskResults::TaskResult TaskResult;

        struct TaskStartedArgs
        {
            bool m_Cancelable;

            TaskStartedArgs(bool cancelable)
                : m_Cancelable (cancelable)
            {

            }
        };
        typedef Helium::Signature< void, const TaskStartedArgs& > TaskStartedSignature;

        LUNA_TASK_API void AddTaskStartedListener( const TaskStartedSignature::Delegate& listener );
        LUNA_TASK_API void RemoveTaskStartedListener( const TaskStartedSignature::Delegate& listener );
        LUNA_TASK_API void RaiseTaskStarted( const TaskStartedArgs& );

        struct TaskFinishedArgs
        {
            TaskResult m_Result;

            TaskFinishedArgs()
                : m_Result (TaskResults::Cancel)
            {

            }
        };
        typedef Helium::Signature< void, const TaskFinishedArgs& > TaskFinishedSignature;

        LUNA_TASK_API void AddTaskFinishedListener( const TaskFinishedSignature::Delegate& listener );
        LUNA_TASK_API void RemoveTaskFinishedListener( const TaskFinishedSignature::Delegate& listener );
        LUNA_TASK_API void RaiseTaskFinished( const TaskFinishedArgs& );
    }
}