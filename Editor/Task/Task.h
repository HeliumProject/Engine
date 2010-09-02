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
        typedef Helium::Signature< const TaskStartedArgs& > TaskStartedSignature;

        void AddTaskStartedListener( const TaskStartedSignature::Delegate& listener );
        void RemoveTaskStartedListener( const TaskStartedSignature::Delegate& listener );
        void RaiseTaskStarted( const TaskStartedArgs& );

        struct TaskFinishedArgs
        {
            TaskResult m_Result;

            TaskFinishedArgs()
                : m_Result (TaskResults::Cancel)
            {

            }
        };
        typedef Helium::Signature< const TaskFinishedArgs& > TaskFinishedSignature;

        void AddTaskFinishedListener( const TaskFinishedSignature::Delegate& listener );
        void RemoveTaskFinishedListener( const TaskFinishedSignature::Delegate& listener );
        void RaiseTaskFinished( const TaskFinishedArgs& );
    }
}