/// Allocate a job, acquiring a pooled allocation if possible.
///
/// Unlike AllocateJobUninitialized(), this will handle initializing the acquired job instance using the placement
/// "new" operator before returning.
///
/// @return  Pointer to an allocated job.
///
/// @see ReleaseJob(), AllocateJobUninitialized(), ReleaseJobUninitialized()
template< typename T >
T* Helium::JobManager::AllocateJob()
{
	void* pJob = AllocateJobUninitialized( sizeof( T ) );
	if( pJob )
	{
		new( pJob ) T;
	}

	return static_cast< T* >( pJob );
}

/// Release an allocated job.
///
/// Unlike ReleaseJobUninitialized(), this will handle calling the destructor on the job instance prior to returning
/// it to a pool.
///
/// @param[in] pJob  Job instance to release.
///
/// @see AllocateJob(), ReleaseJobUninitialized(), AllocateJobUninitialized()
template< typename T >
void Helium::JobManager::ReleaseJob( T* pJob )
{
	HELIUM_ASSERT( pJob );

	pJob->~T();
	ReleaseJobUninitialized( pJob, sizeof( T ) );
}
