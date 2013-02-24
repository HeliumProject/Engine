#include "EnginePch.h"
#include "Engine/JobManager.h"

#include "Platform/Atomic.h"

using namespace Helium;

JobManager* JobManager::sm_pInstance = NULL;

/// Constructor.
JobManager::JobManager()
: m_pHeadPool( NULL )
{
}

/// Destructor.
JobManager::~JobManager()
{
    Shutdown();
}

/// Initialize this manager for use.
///
/// @return  True if initialization was successful, false if not.
///
/// @see Shutdown()
bool JobManager::Initialize()
{
    Shutdown();

    return true;
}

/// Shut down this manager and free any allocated resources.
///
/// @see Initialize()
void JobManager::Shutdown()
{
    m_poolTls.SetPointer( NULL );

#if HELIUM_TRACK_JOB_POOL_HITS
    HELIUM_TRACE( TraceLevels::Info, TXT( "Job pool allocation stats:\n" ) );
    HELIUM_TRACE( TraceLevels::Info, TXT( "Pool index\tLocal hits\tStolen hits\tMisses\n" ) );
    uint32_t nodeIndex = 0;
#endif

    PoolNode* pNode = m_pHeadPool;
    while( pNode )
    {
#if HELIUM_TRACK_JOB_POOL_HITS
        HELIUM_TRACE(
            TraceLevels::Info,
            TXT( "%" ) TPRIu32 TXT( "\t%" ) TPRIu32 TXT( "\t%" ) TPRIu32 TXT( "\t%" ) TPRIu32 TXT( "\n" ),
            nodeIndex,
            pNode->localHits,
            pNode->stolenHits,
            pNode->misses );
        ++nodeIndex;
#endif

        PoolNode* pNext = pNode->pNext;
        delete pNode;
        pNode = pNext;
    }

#if HELIUM_TRACK_JOB_POOL_HITS
    HELIUM_TRACE( TraceLevels::Info, TXT( "\n" ) );
#endif

    m_pHeadPool = NULL;
}

/// Allocate memory for a job of a given size.
///
/// Unlike AllocateJob(), this does not initialize the memory for the job.  The caller is responsible for
/// initializing the job instance using the placement "new" operator before using the job.
///
/// @param[in] size  Job size.
///
/// @return  Pointer to an uninitialized allocation for the job.
///
/// @see ReleaseJobUninitialized(), AllocateJob(), ReleaseJob()
void* JobManager::AllocateJobUninitialized( size_t size )
{
    // Get the current thread's job pool.
    PoolNode* pLocalNode = GetThreadLocalPoolNode();
    HELIUM_ASSERT( pLocalNode );

    // Attempt to acquire an allocation from the current thread's pool first.
    void* pJob = pLocalNode->pool.AcquireUninitialized( size );
    if( pJob )
    {
#if HELIUM_TRACK_JOB_POOL_HITS
        ++pLocalNode->localHits;
#endif

        return pJob;
    }

    // Attempt to steal an allocation from an existing pool, starting with the pool immediately following the
    // current thread's pool in the list and looping through all available pools.
    for( PoolNode* pNode = pLocalNode->pNext; pNode != NULL; pNode = pNode->pNext )
    {
        pJob = pNode->pool.AcquireUninitialized( size );
        if( pJob )
        {
#if HELIUM_TRACK_JOB_POOL_HITS
            ++pLocalNode->stolenHits;
#endif

            return pJob;
        }
    }

    for( PoolNode* pNode = m_pHeadPool; pNode != pLocalNode; pNode = pNode->pNext )
    {
        HELIUM_ASSERT( pNode != NULL );
        pJob = pNode->pool.AcquireUninitialized( size );
        if( pJob )
        {
#if HELIUM_TRACK_JOB_POOL_HITS
            ++pLocalNode->stolenHits;
#endif

            return pJob;
        }
    }

    // No job allocation could be retrieved from any other thread's pools, so allocate a new job.
    pJob = JobPool::NewJobUninitialized( size );
    HELIUM_ASSERT( pJob );

#if HELIUM_TRACK_JOB_POOL_HITS
    ++pLocalNode->misses;
#endif


    return pJob;
}

/// Release memory for a job of the given size.
///
/// This does not actually free the allocated job memory, but instead places it in a pool for later reuse.
///
/// Unlike ReleaseJob(), this does not call the destructor for the job.  It is the responsibility of the caller to
/// make sure the job object's destructor is called prior to calling this function.
///
/// @param[in] pJob  Job allocation memory.
/// @param[in] size  Job size.
///
/// @see AllocateJobUninitialized(), ReleaseJob(), AllocateJob()
void JobManager::ReleaseJobUninitialized( void* pJob, size_t size )
{
    HELIUM_ASSERT( pJob );

    // Release the job to the current thread's job pool.
    PoolNode* pLocalNode = GetThreadLocalPoolNode();
    HELIUM_ASSERT( pLocalNode );

    pLocalNode->pool.ReleaseUninitialized( pJob, size );
}

/// Get the static task manager instance, creating it if necessary.
///
/// @return  Job manager instance.
///
/// @see DestroyStaticInstance()
JobManager& JobManager::GetStaticInstance()
{
    if( !sm_pInstance )
    {
        sm_pInstance = new JobManager;
        HELIUM_ASSERT( sm_pInstance );
    }

    return *sm_pInstance;
}

/// Destroy the static job manager instance if one exists.
///
/// @see GetStaticInstance()
void JobManager::DestroyStaticInstance()
{
    if( sm_pInstance )
    {
        sm_pInstance->Shutdown();
        delete sm_pInstance;
        sm_pInstance = NULL;
    }
}

/// Get the job pool node for the current thread, allocating it if necessary.
///
/// @return  Pointer to the current thread's job pool node.
JobManager::PoolNode* JobManager::GetThreadLocalPoolNode()
{
    PoolNode* pNode = static_cast< PoolNode* >( m_poolTls.GetPointer() );
    if( !pNode )
    {
        // Pool does not yet exist, so allocate one and add it to the global list of pool nodes.
        pNode = new PoolNode;
        HELIUM_ASSERT( pNode );
        m_poolTls.SetPointer( pNode );

#if HELIUM_TRACK_JOB_POOL_HITS
        pNode->localHits = 0;
        pNode->stolenHits = 0;
        pNode->misses = 0;
#endif

        PoolNode* pTestNext;
        PoolNode* pNext = m_pHeadPool;
        do
        {
            pTestNext = pNext;
            pNode->pNext = pTestNext;

            pNext = AtomicCompareExchangeRelease( m_pHeadPool, pNode, pTestNext );
        } while( pNext != pTestNext );
    }

    return pNode;
}
