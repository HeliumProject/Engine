#include "EnginePch.h"
#include "Engine/JobContext.h"

#include "Engine/JobTask.h"

using namespace Helium;

/// Constructor.
JobContext::JobContext()
	: m_pTask( NULL )
	, m_pActiveSpawner( NULL )
	, m_bAllocatedChildren( false )
{
}

/// Destructor.
JobContext::~JobContext()
{
}

/// Allocate a TBB child task instance for a given job context.
///
/// @param[in] pChildContext  Context for the child job.
///
/// @return  Child task instance.
///
/// @see AllocateContinuationTask(), AllocateRootTask()
tbb::task* JobContext::AllocateChildTask( JobContext* pChildContext )
{
	HELIUM_ASSERT( pChildContext );

	HELIUM_ASSERT( m_pTask );
	JobTask* pChildTask = new( m_pTask->allocate_child() ) JobTask( pChildContext );
	HELIUM_ASSERT( pChildTask );

	return pChildTask;
}

/// Allocate a TBB continuation task instance for a given job context.
///
/// @param[in] pContinuationContext  Context for the continuation job.
///
/// @return  Continuation task instance.
///
/// @see AllocateChildTask(), AllocateRootTask()
tbb::task* JobContext::AllocateContinuationTask( JobContext* pContinuationContext )
{
	HELIUM_ASSERT( pContinuationContext );

	HELIUM_ASSERT( m_pTask );
	JobTask* pContinuationTask = new( m_pTask->allocate_continuation() ) JobTask( pContinuationContext );
	HELIUM_ASSERT( pContinuationTask );

	return pContinuationTask;
}

/// Allocate a TBB root task instance for a given job context.
///
/// @param[in] pContext  Context for the root job.
///
/// @return  Root task instance.
///
/// @see AllocateChildTask(), AllocateContinuationTask()
tbb::task* JobContext::AllocateRootTask( JobContext* pContext )
{
	HELIUM_ASSERT( pContext );

	JobTask* pRootTask = new( tbb::task::allocate_root() ) JobTask( pContext );
	HELIUM_ASSERT( pRootTask );

	return pRootTask;
}
