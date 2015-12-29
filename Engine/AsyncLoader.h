#pragma once

#include "Platform/Condition.h"
#include "Platform/Locks.h"
#include "Platform/Thread.h"

#include "Foundation/DynamicArray.h"
#include "Foundation/ObjectPool.h"
#include "Foundation/String.h"

#include "Engine/Engine.h"

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4530 )  // C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif

namespace Helium
{
	/// Async loading manager.
	class HELIUM_ENGINE_API AsyncLoader : NonCopyable
	{
	public:
		/// Request pool block size.
		static const size_t REQUEST_POOL_BLOCK_SIZE = 128;
		/// Maximum number of open file streams.
		static const size_t FILE_STREAM_LIMIT = 16;

		/// Load request priority.
		enum EPriority
		{
			PRIORITY_FIRST   =  0,
			PRIORITY_INVALID = -1,

			PRIORITY_LOW,
			PRIORITY_NORMAL,
			PRIORITY_HIGH,

			PRIORITY_MAX,
			PRIORITY_LAST = PRIORITY_MAX - 1
		};

		/// @name Initialization
		//@{
		bool Initialize();
		void Shutdown();
		//@}

		/// @name Load Request Management
		//@{
		size_t QueueRequest(
			void* pBuffer, const String& rFileName, uint64_t offset, size_t size,
			EPriority priority = PRIORITY_NORMAL );
		size_t SyncRequest( size_t id );
		bool TrySyncRequest( size_t id, size_t& rBytesRead );

		void Flush();

		void Lock();
		void Unlock();
		//@}

		/// @name Static Access
		//@{
		static AsyncLoader& GetInstance();
		static void DestroyStaticInstance();
		//@}

	private:
		/// Async load request data.
		struct Request
		{
			/// Output buffer.
			void* pBuffer;
			/// File name.
			String fileName;
			/// Offset from which to begin reading.
			uint64_t offset;
			/// Number of bytes to read.
			size_t size;
			/// Priority.
			EPriority priority;

			/// Number of bytes read.
			volatile size_t bytesRead;
			/// Set to a non-zero value once this request has been processed.
			volatile int32_t processedCounter;
		};

		/// Async loading thread runnable.
		class LoadWorker : public Runnable
		{
		public:
			/// @name Construction/Destruction
			//@{
			LoadWorker();
			virtual ~LoadWorker();
			//@}

			/// @name Runnable Interface
			//@{
			virtual void Run();
			//@}

			/// @name External Thread Control
			//@{
			void Stop();
			//@}

			/// @name External Request Queue Control
			//@{
			void QueueRequest( Request* pRequest );
			void Flush();

			void Lock();
			void Unlock();
			//@}

		private:
			/// Async load request queue.
			Locker< DynamicArray< Request* >, SpinLock > m_requestQueue;
			/// Condition used to wake up the worker thread when load requests are queued (or when it should shut down).
			Condition m_wakeUpCondition;

			/// Read-write lock used for synchronization of external file writes.
			ReadWriteLock m_writeLock;

			/// Non-zero if this thread should stop when next possible, zero if it should continue.
			volatile int32_t m_stopCounter;
			/// Non-zero if this thread is currently processing a load request.
			volatile int32_t m_processingCounter;
		};

		/// Pool of async load request objects.
		ObjectPool< Request > m_requestPool;

		/// Async loading thread.
		RunnableThread* m_pThread;
		/// Async loading thread worker.
		LoadWorker* m_pWorker;

		/// Singleton instance.
		static AsyncLoader* sm_pInstance;

		/// @name Construction/Destruction
		//@{
		AsyncLoader();
		~AsyncLoader();
		//@}
	};
}
