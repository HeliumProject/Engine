//----------------------------------------------------------------------------------------------------------------------
// JobTask.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/JobTask.h"

#include "Engine/JobContext.h"

using namespace Helium;


/// Constructor.
///
/// @param[in] pContext  Job execution context.
JobTask::JobTask( JobContext* pContext )
: m_pContext( pContext )
{
    HELIUM_ASSERT( pContext );
}

/// Execute this task.
///
/// @return  Next task to run on the current thread (bypassing task scheduling).
tbb::task* JobTask::execute()
{
    if( m_pContext )
    {
        // Execute the job.
        const JobContext::AttachData& rAttachData = m_pContext->GetAttachData();
        void* pData = rAttachData.GetData();
        if( pData )
        {
            JobContext::JOB_EXECUTE_CALLBACK* pExecuteCallback = rAttachData.GetExecuteCallback();
            HELIUM_ASSERT( pExecuteCallback );

            pExecuteCallback( pData, m_pContext );
        }

        // Delete the job context object, as it is no longer needed.
        JobManager& rJobManager = JobManager::GetStaticInstance();
        rJobManager.ReleaseJob( m_pContext );
        m_pContext = NULL;
    }

    return NULL;
}
