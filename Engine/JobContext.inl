/// Create a new job of the template type type and attach it to this context.
///
/// The lifetime of the job object is not handled automatically by the job management system.  It must be explicitly
/// released back to the job manager when it is no longer needed.
///
/// Note that this function can only be called once for a job context.
///
/// @return  Newly allocated job.
///
/// @see Attach()
template< typename JobType >
JobType* Helium::JobContext::Create()
{
	HELIUM_ASSERT( !m_attachData.GetData() );

	JobManager& rJobManager = JobManager::GetStaticInstance();
	JobType* pJob = rJobManager.AllocateJob< JobType >();
	HELIUM_ASSERT( pJob );

	m_attachData.Set( pJob );

	return pJob;
}

/// Attach an existing job to this context.
///
/// The lifetime of the job object is not handled automatically by the job management system.  It must be explicitly
/// released back to the job manager when it is no longer needed.
///
/// Note that this function can only be called once for a job context.
///
/// @param[in] pJob  Job to attach.
///
/// @see Create()
template< typename JobType >
void Helium::JobContext::Attach( JobType* pJob )
{
	HELIUM_ASSERT( pJob );
	HELIUM_ASSERT( !m_attachData.GetData() );

	m_attachData.Set( pJob );
}

/// Attach an existing job to this context.
///
/// The lifetime of the job object is not handled automatically by the job management system.  It must be explicitly
/// released back to the job manager when it is no longer needed.
///
/// Note that this function can only be called once for a job context.
///
/// @param[in] rAttachData  Attachment data for the job to attach.
///
/// @see Create()
void Helium::JobContext::Attach( const AttachData& rAttachData )
{
	HELIUM_ASSERT( !m_attachData.GetData() );

	m_attachData = rAttachData;
}

/// Get the job attachment data.
///
/// @return  Job attachment data.
const Helium::JobContext::AttachData& Helium::JobContext::GetAttachData() const
{
	return m_attachData;
}

/// Constructor.
Helium::JobContext::AttachData::AttachData()
	: m_pData( NULL )
	, m_pExecuteCallback( NULL )
{
}

/// Constructor.
///
/// @param[in] pJob  Job from which to initialize this object.
template< typename JobType >
Helium::JobContext::AttachData::AttachData( JobType* pJob )
	: m_pData( pJob )
	, m_pExecuteCallback( JobType::RunCallback )
{
	HELIUM_ASSERT( pJob );
}

/// Set the job to attach.
///
/// @param[in] pJob  Job to attach.
///
/// @see Clear(), GetData(), GetExecuteCallback()
template< typename JobType >
void Helium::JobContext::AttachData::Set( JobType* pJob )
{
	HELIUM_ASSERT( pJob );

	m_pData = pJob;
	m_pExecuteCallback = JobType::RunCallback;
}

/// Set the job to attach.
///
/// @param[in] pData             Job data.
/// @param[in] pExecuteCallback  Job execution callback.
///
/// @see Clear(), GetData(), GetExecuteCallback()
void Helium::JobContext::AttachData::Set( void* pData, JOB_EXECUTE_CALLBACK* pExecuteCallback )
{
	HELIUM_ASSERT( pData );
	HELIUM_ASSERT( pExecuteCallback );

	m_pData = pData;
	m_pExecuteCallback = pExecuteCallback;
}

/// Clear the job information.
///
/// @see Set(), GetData(), GetExecuteCallback()
void Helium::JobContext::AttachData::Clear()
{
	m_pData = NULL;
	m_pExecuteCallback = NULL;
}

/// Get the pointer to the job data.
///
/// @return  Job data.
///
/// @see GetExecuteCallback(), Set(), Clear()
void* Helium::JobContext::AttachData::GetData() const
{
	return m_pData;
}

/// Get the function to execute for the job.
///
/// @return  Job execution callback.
///
/// @see GetData(), Set(), Clear()
Helium::JobContext::JOB_EXECUTE_CALLBACK* Helium::JobContext::AttachData::GetExecuteCallback() const
{
	return m_pExecuteCallback;
}

/// Constructor.
///
/// @param[in] pContext  Job context from which to spawn jobs, or null to spawn root jobs.  Note that continuation
///                      jobs can only be spawned from an existing job context.
template< size_t ChildCountMax >
Helium::JobContext::Spawner< ChildCountMax >::Spawner( JobContext* pContext )
	: m_pContext( pContext )
	, m_pContinuationContext( NULL )
	, m_childContextCount( 0 )
{
}

/// Destructor.
template< size_t ChildCountMax >
Helium::JobContext::Spawner< ChildCountMax >::~Spawner()
{
	Commit();
}

/// Allocate a context for a child or root job.
///
/// Root jobs are allocated if this spawner was created with a null existing context specified; child contexts are
/// allocated otherwise.  Note that spawning root jobs will cause Commit() to block until the jobs have completed,
/// whereas Commit() returns immediately when spawning child jobs.
///
/// Jobs are not spawned until Commit() is called, either explicitly or through destruction of the spawner.
///
/// @return  Child or root job context if successfully allocated, null if allocation failed.
///
/// @see AllocateContinuation(), Commit()
template< size_t ChildCountMax >
Helium::JobContext* Helium::JobContext::Spawner< ChildCountMax >::Allocate()
{
	// Make sure we have enough room left to allocate another child context.
	size_t childContextCount = m_childContextCount;
	HELIUM_ASSERT( childContextCount < HELIUM_ARRAY_COUNT( m_childContexts ) );
	if( childContextCount >= HELIUM_ARRAY_COUNT( m_childContexts ) )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "The maximum number of child contexts (%" ) PRIuSZ TXT( ") has already been allocated for this spawner.\n" ),
			childContextCount );

		return NULL;
	}

	JobManager& rJobManager = JobManager::GetStaticInstance();

	// Allocate either a child or root context based on whether we have a parent context.
	JobContext* pChildContext;
	if( m_pContext )
	{
		// Make sure this spawner is the only active spawner for the parent context and that children have not
		// already been spawned for it.
		if( m_pContext->m_pActiveSpawner != this )
		{
			HELIUM_ASSERT( !m_pContext->m_pActiveSpawner );
			if( m_pContext->m_pActiveSpawner )
			{
				HELIUM_TRACE( TraceLevels::Error, TXT( "Parent context already has a job spawner attached.\n" ) );

				return NULL;
			}

			HELIUM_ASSERT( !m_pContext->m_bAllocatedChildren );
			if( m_pContext->m_bAllocatedChildren )
			{
				HELIUM_TRACE(
					TraceLevels::Error,
					( TXT( "Child context cannot be allocated for a context for which children have already been " )
					TXT( "spawned." ) ) );

				return NULL;
			}

			m_pContext->m_pActiveSpawner = this;
		}

		pChildContext = static_cast< JobContext* >( rJobManager.AllocateJobUninitialized( sizeof( JobContext ) ) );
		HELIUM_ASSERT( pChildContext );
		new( pChildContext ) JobContext;

		// Allocate jobs as children of the continuation context if one has been already allocated.
		JobContext* pSourceContext = m_pContinuationContext;
		if( !pSourceContext )
		{
			pSourceContext = m_pContext;
		}

		tbb::task* pChildTask = pSourceContext->AllocateChildTask( pChildContext );
		HELIUM_ASSERT( pChildTask );

		pChildContext->m_pTask = pChildTask;
	}
	else
	{
		// Allocate a root task.
		pChildContext = static_cast< JobContext* >( rJobManager.AllocateJobUninitialized( sizeof( JobContext ) ) );
		HELIUM_ASSERT( pChildContext );
		new( pChildContext ) JobContext;

		tbb::task* pChildTask = JobContext::AllocateRootTask( pChildContext );
		HELIUM_ASSERT( pChildTask );

		pChildContext->m_pTask = pChildTask;
	}

	// Queue the job context for spawning.
	m_childContexts[ childContextCount ] = pChildContext;
	m_childContextCount = childContextCount + 1;

	return pChildContext;
}

/// Allocate a continuation job.
///
/// Continuation jobs are executed once all child jobs, as well as any children they spawn, have completed.
/// Continuation jobs can only be allocated if this spawner was initialized with an existing job context instance
/// from which to spawn child jobs.  Continuation jobs aren't required, but only one continuation job may be spawned
/// from an existing job.
///
/// Jobs are not spawned until Commit() is called, either explicitly or through destruction of the spawner.
///
/// @return  Continuation job context if successfully allocated, null if allocation failed.
///
/// @see Allocate(), Commit()
template< size_t ChildCountMax >
Helium::JobContext* Helium::JobContext::Spawner< ChildCountMax >::AllocateContinuation()
{
	// Make sure we have not already created a continuation job.
	HELIUM_ASSERT( !m_pContinuationContext );
	if( m_pContinuationContext )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "A continuation context has already been allocated for this spawner.\n" ) );

		return NULL;
	}

	// Make sure this spawner was created with an existing context.
	HELIUM_ASSERT( m_pContext );
	if( !m_pContext )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "Spawner was not created with an existing job context.\n" ) );

		return NULL;
	}

	// Continuation jobs must be allocated before child jobs are allocated.
	HELIUM_ASSERT( m_childContextCount == 0 );
	if( m_childContextCount != 0 )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "Continuation contexts must be allocated before allocating child contexts.\n" ) );

		return NULL;
	}

	// Make sure this spawner is the only active spawner for the parent context and that children have not
	// already been spawned for it.
	if( m_pContext->m_pActiveSpawner != this )
	{
		HELIUM_ASSERT( !m_pContext->m_pActiveSpawner );
		if( m_pContext->m_pActiveSpawner )
		{
			HELIUM_TRACE( TraceLevels::Error, TXT( "Parent context already has a job spawner attached.\n" ) );

			return NULL;
		}

		HELIUM_ASSERT( !m_pContext->m_bAllocatedChildren );
		if( m_pContext->m_bAllocatedChildren )
		{
			HELIUM_TRACE(
				TraceLevels::Error,
				( TXT( "Continuation context cannot be allocated for a context for which children have already " )
				TXT( "been spawned." ) ) );

			return NULL;
		}

		m_pContext->m_pActiveSpawner = this;
	}

	// Allocate the continuation context.
	JobManager& rJobManager = JobManager::GetStaticInstance();

	m_pContinuationContext = static_cast< JobContext* >( rJobManager.AllocateJobUninitialized(
		sizeof( JobContext ) ) );
	HELIUM_ASSERT( m_pContinuationContext );
	new( m_pContinuationContext ) JobContext;

	tbb::task* pContinuationTask = m_pContext->AllocateContinuationTask( m_pContinuationContext );
	HELIUM_ASSERT( pContinuationTask );

	m_pContinuationContext->m_pTask = pContinuationTask;

	return m_pContinuationContext;
}

/// Spawn all allocated jobs.
///
/// When spawning root jobs, this will also block until those jobs and any child jobs spawned complete.  When
/// spawning child jobs, this will return immediately.
///
/// Note that this is automatically called upon destruction (such as when an instance goes out of scope).  It is
/// only necessary to call this function directly if explicit control of when the jobs are spawned is necessary.
///
/// @see Allocate(), AllocateContinuation()
template< size_t ChildCountMax >
void Helium::JobContext::Spawner< ChildCountMax >::Commit()
{
	// Get the spawn parent (continuation context if one was created, otherwise the context with which this was
	// initialized).
	JobContext* pSpawnParent = m_pContinuationContext;
	if( !pSpawnParent )
	{
		pSpawnParent = m_pContext;
	}

	tbb::task* pSpawnParentTask = NULL;
	if( pSpawnParent )
	{
		pSpawnParentTask = pSpawnParent->m_pTask;
		HELIUM_ASSERT( pSpawnParentTask );
	}

	// Set the parent task reference count if spawning child jobs.
	size_t jobCount = m_childContextCount;

	if( pSpawnParent )
	{
		int refCount = static_cast< int >( jobCount );

		// Reuse this job as an empty continuation job if a continuation job has not explicitly been created.
		if( !m_pContinuationContext )
		{
			pSpawnParentTask->set_ref_count( refCount + 1 );
			pSpawnParentTask->recycle_as_safe_continuation();
		}
		else
		{
			pSpawnParentTask->set_ref_count( refCount );
		}
	}

	// Spawn child/root jobs.
	if( pSpawnParentTask )
	{
		for( size_t jobIndex = 0; jobIndex < jobCount; ++jobIndex )
		{
			JobContext* pChildContext = m_childContexts[ jobIndex ];
			HELIUM_ASSERT( pChildContext );

			tbb::task* pChildTask = pChildContext->m_pTask;
			HELIUM_ASSERT( pChildTask );

			pSpawnParentTask->spawn( *pChildTask );
		}

		HELIUM_ASSERT( pSpawnParent );
		pSpawnParent->m_bAllocatedChildren = true;
		pSpawnParent->m_pActiveSpawner = NULL;
	}
	else
	{
		for( size_t jobIndex = 0; jobIndex < jobCount; ++jobIndex )
		{
			JobContext* pRootContext = m_childContexts[ jobIndex ];
			HELIUM_ASSERT( pRootContext );

			tbb::task* pRootTask = pRootContext->m_pTask;
			HELIUM_ASSERT( pRootTask );

			tbb::task::spawn_root_and_wait( *pRootTask );
		}
	}

	m_pContinuationContext = NULL;
	m_childContextCount = 0;
}
