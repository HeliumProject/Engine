#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#include "Platform/Atomic.h"
#include "Platform/Thread.h"
#include "Platform/Condition.h"
#include "Platform/Utility.h"

#if !HELIUM_OS_WIN
# include <pthread.h>
#endif

namespace Helium
{
	/// Scoped locking mechanism.
	///
	/// This class provides functionality for locking a synchronization object (i.e. mutex, spin lock) and automatically
	/// unlocking the object when the scope in which the scope lock exists ends.
	template< typename T, void ( T::*LockFunction )() = &T::Lock, void ( T::*UnlockFunction )() = &T::Unlock >
	class ScopeLock : NonCopyable
	{
	public:
		/// @name Construction/Destruction
		//@{
		explicit ScopeLock( T& rSyncObject );
		~ScopeLock();
		//@}

	private:
		/// Reference to the synchronization object being locked.
		T& m_rSyncObject;
	};
	
	/// Spin lock.
	///
	/// This provides a simple mechanism for synchronizing access to a given resource through the use of a spin lock.
	/// A lock is acquired simply by setting an integer to a non-zero value.  While the counter is non-zero, any other
	/// thread that tries to acquire a lock will block until the counter is available.  Instead of letting the OS handle
	/// putting the thread into a wait state until the lock is available, the thread continuously repeats its attempt to
	/// acquire the lock.  To avoid excess starving of other threads, the thread will yield after each failed attempt.
	///
	/// Note that recursive locking is not supported.  If a thread attempts to acquire a lock while it already has one,
	/// a deadlock will occur.
	class HELIUM_PLATFORM_API SpinLock
	{
	public:
		/// @name Construction/Destruction
		//@{
		inline SpinLock();
		//@}

		/// @name Synchronization Interface
		//@{
		inline void Lock();
		inline void Unlock();
		inline bool TryLock();

		inline bool IsLocked() const;
		inline void WaitForUnlock();
		//@}

	private:
		/// Spin lock counter.
		volatile int32_t m_counter;
	};

	/// Scope-based locking mechanism for SpinLock objects.
	typedef ScopeLock< SpinLock > ScopeSpinLock;
	
	/// Read-write lock.
	///
	/// A read-write lock is a synchronization mechanism for allowing multiple threads to acquire read-only access to a
	/// shared resource simultaneously, while allowing exclusive access to a single thread when read-write access is
	/// desired.  This allows for much more minimal blocking of threads, particularly when exclusive write access is
	/// kept to a minimum.
	///
	/// For efficiency, locking is only guaranteed to be functional within the context of the process in which the
	/// read-write lock was created.
	class HELIUM_PLATFORM_API ReadWriteLock
	{
	public:
		/// @name Construction/Destruction
		//@{
		ReadWriteLock();
		~ReadWriteLock();
		//@}

		/// @name Synchronization Interface
		//@{
		void LockRead();
		void UnlockRead();

		void LockWrite();
		void UnlockWrite();
		//@}

	private:
		/// Number of threads with read access, or -1 if write access is currently being held.
		volatile int32_t m_readLockCount;

		/// Read-lock release event.
		Condition m_readReleaseCondition;
		/// Write-lock release event.
		Condition m_writeReleaseCondition;
	};

	/// Scope-based read-locking mechanism for ReadWriteLock objects.
	typedef ScopeLock< ReadWriteLock, &ReadWriteLock::LockRead, &ReadWriteLock::UnlockRead > ScopeReadLock;
	/// Scope-based write-locking mechanism for ReadWriteLock objects.
	typedef ScopeLock< ReadWriteLock, &ReadWriteLock::LockWrite, &ReadWriteLock::UnlockWrite > ScopeWriteLock;

	/// Mutex.
	///
	/// On supported platforms, this is implemented using a lightweight mutex that can only be used within the context
	/// of the process in which it is created (i.e. critical sections on Windows).  This typically yields better
	/// performance than a full-scale mutex, making it a more desirable alternative for general use.
	///
	/// If a mutex is needed for synchronization across processes, ProcessMutex should be used instead.
	class HELIUM_PLATFORM_API Mutex : NonCopyable
	{
	public:
#if HELIUM_OS_WIN
		struct Handle
		{
			struct DebugInfoStruct
			{
				uint16_t Type;
				uint16_t CreatorBackTraceIndex;
				struct Handle *CriticalSection;
				struct ListEntryStruct
				{
					struct ListEntryStruct *Flink;
					struct ListEntryStruct *Blink;
				} ProcessLocksList;
				uint32_t EntryCount;
				uint32_t ContentionCount;
				uint32_t Spare[ 2 ];
			} *DebugInfo;

			int32_t LockCount;
			int32_t RecursionCount;
			void* OwningThread;
			void* LockSemaphore;
			uint32_t* SpinCount;
		};
#else
		typedef pthread_mutex_t Handle;
#endif

	public:
		/// @name Construction/Destruction
		//@{
		Mutex();
		~Mutex();
		//@}

		/// @name Synchronization Interface
		//@{
		void Lock();
		void Unlock();
		bool TryLock();
		//@}

		/// @name Data Access
		//@{
		inline const Handle& GetHandle() const;
		//@}

	private:
		/// Platform-specific mutex handle.
		Handle m_Handle;
	};

	/// Scope-based locking mechanism for Mutex objects.
	typedef ScopeLock< Mutex > MutexScopeLock;

	/// Simple template to make some data only accessible to one thread at a time.
	template< typename T, typename LockType = Mutex >
	class Locker : NonCopyable
	{
	public:
		/// Handle for accessing the protected data.
		class Handle : NonCopyable
		{
		public:
			/// @name Construction/Destruction
			//@{
			inline explicit Handle( Locker& locker );
			inline ~Handle();
			//@}

			/// @name Overloaded Operators
			//@{
			inline T* operator->();
			//@}

		private:
			/// Protected data locker.
			Locker& m_Locker;
		};

	private:
		/// Protected data.
		T m_Data;
		/// Synchronization object.
		LockType m_LockObject;
	};
}

#include "Platform/Locks.inl"
