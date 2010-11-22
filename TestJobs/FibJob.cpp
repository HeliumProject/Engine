//----------------------------------------------------------------------------------------------------------------------
// FibJob.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "TestJobsPch.h"
#include "TestJobs/TestJobsInterface.h"

#include "Platform/Assert.h"
#include "Engine/JobContext.h"

namespace Lunar
{
    void FibJob::Run( JobContext* pContext )
    {
        HELIUM_ASSERT( pContext );
        HELIUM_ASSERT( m_parameters.pSum );

        if( m_parameters.n < 2 )
        {
            *m_parameters.pSum = m_parameters.n;
        }
        else
        {
            JobContext::Spawner< 2 > jobSpawner( pContext );

            JobContext* pContinuationContext = jobSpawner.AllocateContinuation();
            HELIUM_ASSERT( pContinuationContext );
            FibContinuation* pContinuationJob = pContinuationContext->Create< FibContinuation >();
            HELIUM_ASSERT( pContinuationJob );
            pContinuationJob->GetParameters().pSum = m_parameters.pSum;

            JobContext* pChildContext;
            FibJob* pChildJob;

            pChildContext = jobSpawner.Allocate();
            HELIUM_ASSERT( pChildContext );
            pChildJob = pChildContext->Create< FibJob >();
            HELIUM_ASSERT( pChildJob );
            pChildJob->GetParameters().n = m_parameters.n - 2;
            pChildJob->GetParameters().pSum = &pContinuationJob->GetParameters().x;

            pChildContext = jobSpawner.Allocate();
            HELIUM_ASSERT( pChildContext );
            pChildJob = pChildContext->Create< FibJob >();
            HELIUM_ASSERT( pChildJob );
            pChildJob->GetParameters().n = m_parameters.n - 1;
            pChildJob->GetParameters().pSum = &pContinuationJob->GetParameters().y;
        }

        JobManager& rJobManager = JobManager::GetStaticInstance();
        rJobManager.ReleaseJob( this );
    }
}
