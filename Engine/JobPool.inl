/// Acquire a job of the templated type from the pool if an allocation block exists.
///
/// Unlike AcquireUnitialized(), this will make sure the job object is properly constructed using the placement
/// "new" operator before returning.
///
/// @return  Pointer to an instance of the specified job type if one was located, null if no allocations for the
///          specified type exist in the pool.
///
/// @see Release(), AcquireUninitialized(), ReleaseUninitialized()
template< typename T >
T* Helium::JobPool::Acquire()
{
	void* pJob = AcquireUninitialized( sizeof( T ) );
	if( pJob )
	{
		new( pJob ) T;
	}

	return static_cast< T* >( pJob );
}

/// Release a job allocation into this pool.
///
/// Unlike ReleaseUninitialized(), this will call the destructor on the job object before adding the allocation to
/// the pool.
///
/// @param[in] pJob  Pointer to the job instance to release.  Note that only job objects allocated using NewJob() or
///                  acquired from a pool using Acquire() should be passed to this function.
///
/// @see Acquire(), ReleaseUninitialized(), AcquireUninitialized()
template< typename T >
void Helium::JobPool::Release( T* pJob )
{
	HELIUM_ASSERT( pJob );

	pJob->~T();
	ReleaseUninitialized( pJob, sizeof( T ) );
}

/// Allocate a new job instance outside the pool that is compatible with pool usage.
///
/// When jobs are added to a pool using Release() or ReleaseUninitialized(), the memory for the job is reused for
/// data relevant to storage
/// within the pool.  This imposes a minimum size requirement for the memory allocated for a job.  Using this
/// function to allocate a job ensures that the size of the block of memory allocated meets this requirement.
///
/// Note that this should only be used when no existing allocation can be located in any previously existing pool.
/// The Acquire() or AcquireUninitialized() method should first be used on potential pools to attempt to reuse an
/// existing allocation before falling back on this function.
///
/// Job memory stored in a pool will be released when the pool is destroyed, so no equivalent DeleteJob() function
/// is necessary, although code that works with jobs should make sure to properly release the job back to a pool
/// when it is no longer needed prior to shutdown.
///
/// @return  Pointer to a newly allocated job object of the template type.
///
/// @see NewJobUninitialized(), Acquire(), Release(), AcquireUninitialized(), ReleaseUninitialized()
template< typename T >
T* Helium::JobPool::NewJob()
{
	void* pJob = NewJobUninitialized( sizeof( T ) );
	HELIUM_ASSERT( pJob );
	if( pJob )
	{
		new( pJob ) T;
	}

	return static_cast< T* >( pJob );
}

