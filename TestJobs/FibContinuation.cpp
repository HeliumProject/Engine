//----------------------------------------------------------------------------------------------------------------------
// FibContinuation.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "TestJobsPch.h"
#include "TestJobs/TestJobsInterface.h"

#include "Platform/Assert.h"
#include "Engine/JobContext.h"

using namespace Helium;

void FibContinuation::Run( JobContext* pContext )
{
    HELIUM_UNREF( pContext );

    HELIUM_ASSERT( pContext );
    HELIUM_ASSERT( m_parameters.pSum );

    *m_parameters.pSum = m_parameters.x + m_parameters.y;

    JobManager& rJobManager = JobManager::GetStaticInstance();
    rJobManager.ReleaseJob( this );
}
