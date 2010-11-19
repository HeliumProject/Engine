//----------------------------------------------------------------------------------------------------------------------
// AsyncLoader.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "Foundation/AsyncLoader.h"

#include "Foundation/File/File.h"
#include "Foundation/Stream/FileStream.h"
#include "Foundation/Stream/BufferedStream.h"

namespace Lunar
{
    AsyncLoader* AsyncLoader::sm_pInstance = NULL;

    /// Constructor.
    AsyncLoader::AsyncLoader()
        : m_requestPool( REQUEST_POOL_BLOCK_SIZE )
        , m_pThread( NULL )
        , m_pWorker( NULL )
    {
    }

    /// Destructor.
    AsyncLoader::~AsyncLoader()
    {
        Shutdown();
    }

    /// Initialize the async loader.
    ///
    /// @return  True if initialization was sucessful, false if not.
    ///
    /// @see Shutdown()
    bool AsyncLoader::Initialize()
    {
        Shutdown();

        // Start up the async loading thread.
        m_pWorker = new LoadWorker;
        HELIUM_ASSERT( m_pWorker );

        m_pThread = new RunnableThread( m_pWorker, TXT( "Async loading" ) );
        HELIUM_ASSERT( m_pThread );
        HELIUM_VERIFY( m_pThread->Start() );

        return true;
    }

    /// Shut down the async loader.
    ///
    /// @see Initialize()
    void AsyncLoader::Shutdown()
    {
        if( m_pWorker )
        {
            m_pWorker->Stop();
        }

        if( m_pThread )
        {
            m_pThread->Join();
            delete m_pThread;
            m_pThread = NULL;
        }

        delete m_pWorker;
        m_pWorker = NULL;
    }

    /// Queue an async load request.
    ///
    /// @param[in] pBuffer    Buffer in which to load data.
    /// @param[in] rFileName  Path name of the file from which to load.
    /// @param[in] offset     Byte offset within the file from which to load.
    /// @param[in] size       Number of bytes to read.
    /// @param[in] priority   Load priority.
    ///
    /// @return  ID identifying the load request if queued successfully, invalid index if the request queue failed.
    ///
    /// @see SyncRequest(), TrySyncRequest()
    size_t AsyncLoader::QueueRequest(
        void* pBuffer,
        const String& rFileName,
        uint64_t offset,
        size_t size,
        EPriority priority )
    {
        HELIUM_ASSERT( pBuffer );
        HELIUM_ASSERT( static_cast< size_t >( priority ) < static_cast< size_t >( PRIORITY_MAX ) );

        // Make sure the load worker is running.
        if( !m_pWorker )
        {
            return Invalid< size_t >();
        }

        // Allocate and queue the request.
        Request* pRequest = m_requestPool.Allocate();
        HELIUM_ASSERT( pRequest );
        pRequest->pBuffer = pBuffer;
        pRequest->fileName = rFileName;
        pRequest->offset = offset;
        pRequest->size = size;
        pRequest->priority = priority;

        pRequest->bytesRead = 0;
        AtomicExchangeRelease( pRequest->processedCounter, 0 );

        m_pWorker->QueueRequest( pRequest );

        size_t requestIndex = m_requestPool.GetIndex( pRequest );
        HELIUM_ASSERT( IsValid( requestIndex ) );

        return requestIndex;
    }

    /// Block the current thread until the load request with the specified ID completes and release the request
    /// information.
    ///
    /// After calling this function, the given ID will no longer be valid.
    ///
    /// @param[in] id  Request ID.
    ///
    /// @return  Number of bytes read from the file, zero if the request offset was not within the range of the file, or
    ///          an invalid index if the file was not found or could not be opened.
    ///
    /// @see QueueRequest(), TrySyncRequest()
    size_t AsyncLoader::SyncRequest( size_t id )
    {
        HELIUM_ASSERT( IsValid( id ) );

        Request* pRequest = m_requestPool.GetObject( id );
        HELIUM_ASSERT( pRequest );

        while( pRequest->processedCounter == 0 )
        {
            Thread::Yield();
        }

        size_t bytesRead = pRequest->bytesRead;
        m_requestPool.Release( pRequest );

        return bytesRead;
    }

    /// Check whether the load request with the specified ID has completed without blocking the current thread,
    /// releasing the request information if it has completed.
    ///
    /// After calling this function, the given ID will no longer be valid.
    ///
    /// @param[in]  id          Request ID.
    /// @param[out] rBytesRead  If the request has completed, this is set to the number of bytes read from the file,
    ///                         zero if the request offset was not within the range of the file, or an invalid index if
    ///                         the file was not found or could not be opened.
    ///
    /// @return  True if the request has completed and was released, false if is still pending or in progress.
    ///
    /// @see QueueRequest(), SyncRequest()
    bool AsyncLoader::TrySyncRequest( size_t id, size_t& rBytesRead )
    {
        HELIUM_ASSERT( IsValid( id ) );

        Request* pRequest = m_requestPool.GetObject( id );
        HELIUM_ASSERT( pRequest );
        if( pRequest->processedCounter == 0 )
        {
            return false;
        }

        rBytesRead = pRequest->bytesRead;
        m_requestPool.Release( pRequest );

        return true;
    }

    /// Block the current thread until all pending load requests have completed.
    ///
    /// Note that this does not release any requests.  SyncRequest() or TrySyncRequest() must still be called for all
    /// pending requests in order to free any associated resources.
    void AsyncLoader::Flush()
    {
        if( m_pWorker )
        {
            m_pWorker->Flush();
        }
    }

    /// Lock async loading for writing to files that may be in use.
    ///
    /// @see Unlock()
    void AsyncLoader::Lock()
    {
        if( m_pWorker )
        {
            m_pWorker->Lock();
        }
    }

    /// Unlock a previous loader lock.
    ///
    /// @see Lock()
    void AsyncLoader::Unlock()
    {
        if( m_pWorker )
        {
            m_pWorker->Unlock();
        }
    }

    /// Get the singleton AsyncLoader instance, creating it if necessary.
    ///
    /// @return  Reference to the AsyncLoader instance.
    ///
    /// @see DestroyStaticInstance()
    AsyncLoader& AsyncLoader::GetStaticInstance()
    {
        if( !sm_pInstance )
        {
            sm_pInstance = new AsyncLoader;
            HELIUM_ASSERT( sm_pInstance );
        }

        return *sm_pInstance;
    }

    /// Destroy the singleton AsyncLoader instance.
    ///
    /// @see GetStaticInstance()
    void AsyncLoader::DestroyStaticInstance()
    {
        delete sm_pInstance;
        sm_pInstance = NULL;
    }

    /// Constructor.
    AsyncLoader::LoadWorker::LoadWorker()
        : m_wakeUpCondition( Condition::RESET_MODE_AUTO )
        , m_stopCounter( 0 )
        , m_processingCounter( 0 )
    {
    }

    /// Destructor.
    AsyncLoader::LoadWorker::~LoadWorker()
    {
    }

    /// Execute the async loading work.
    void AsyncLoader::LoadWorker::Run()
    {
        BufferedStream* pBufferedStream = new BufferedStream;
        HELIUM_ASSERT( pBufferedStream );

        while( m_stopCounter == 0 )
        {
            AtomicExchangeAcquire( m_processingCounter, 1 );

            Request* pRequest;
            if( !m_requestQueue.try_pop( pRequest ) )
            {
                // Queue is empty, so sleep until notified.
                AtomicExchangeRelease( m_processingCounter, 0 );
                m_wakeUpCondition.Wait();

                continue;
            }

            HELIUM_ASSERT( pRequest );

            FileStream* pFileStream = File::Open( pRequest->fileName, FileStream::MODE_READ );
            if( !pFileStream )
            {
                SetInvalid( pRequest->bytesRead );
            }
            else
            {
                pRequest->bytesRead = 0;

                pBufferedStream->Open( pFileStream );
                int64_t offset = pBufferedStream->Seek( pRequest->offset, SeekOrigins::SEEK_ORIGIN_BEGIN );
                if( static_cast< uint64_t >( offset ) == pRequest->offset )
                {
                    pRequest->bytesRead = pBufferedStream->Read( pRequest->pBuffer, 1, pRequest->size );
                }

                pBufferedStream->Open( NULL );

                delete pFileStream;
            }

            AtomicExchangeRelease( pRequest->processedCounter, 1 );

            Thread::Yield();
        }

        AtomicExchangeRelease( m_processingCounter, 0 );

        delete pBufferedStream;
    }

    /// Request the load worker to stop processing and return at the next possible opportunity.
    void AsyncLoader::LoadWorker::Stop()
    {
        AtomicExchangeRelease( m_stopCounter, 1 );
        m_wakeUpCondition.Signal();
    }

    /// Queue an async load request.
    ///
    /// @param[in] pRequest  Request to queue.  Note that the @c processedCounter value should be set to 0 prior to
    ///                      calling this function (the worker thread will set it to 1 once the request has been
    ///                      processed).
    ///
    /// @see Flush()
    void AsyncLoader::LoadWorker::QueueRequest( Request* pRequest )
    {
        HELIUM_ASSERT( pRequest );
        HELIUM_ASSERT( pRequest->processedCounter == 0 );

        // Prevent access to the load queue while an exclusive write lock is held.
        ScopeReadLock nonExclusiveLock( m_writeLock );

        m_requestQueue.push( pRequest );
        m_wakeUpCondition.Signal();
    }

    /// Block the current thread until all pending load requests have completed.
    ///
    /// @see QueueRequest()
    void AsyncLoader::LoadWorker::Flush()
    {
        while( m_processingCounter != 0 && !m_requestQueue.empty() )
        {
            Thread::Yield();
        }
    }

    /// Acquire an exclusive lock for writing to files and flush the async loader queue.
    ///
    /// @see Unlock()
    void AsyncLoader::LoadWorker::Lock()
    {
        // Prevent other threads from queueing requests or writing out data while we have a write lock.
        m_writeLock.LockWrite();

        Flush();
    }

    /// Release an exclusive lock for writing to files.
    ///
    /// @see Lock()
    void AsyncLoader::LoadWorker::Unlock()
    {
        m_writeLock.UnlockWrite();
    }
}
