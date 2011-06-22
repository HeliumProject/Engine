//----------------------------------------------------------------------------------------------------------------------
// JobTask.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_JOB_TASK_H
#define LUNAR_ENGINE_JOB_TASK_H

#include "Engine/Engine.h"
#include "tbb/task.h"

namespace Helium
{
    class JobContext;

    /// TBB task implementation for executing Helium jobs.
    class JobTask : public tbb::task
    {
    public:
        /// @name Construction/Destruction
        //@{
        JobTask( JobContext* pContext );
        //@}

        /// @name Task Execution
        //@{
        tbb::task* execute();
        //@}

    private:
        /// Job execution context.
        JobContext* m_pContext;
    };
}

#endif  // LUNAR_ENGINE_JOB_TASK_H
