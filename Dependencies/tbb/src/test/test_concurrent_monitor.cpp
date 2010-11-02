/*
    Copyright 2005-2010 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks.

    Threading Building Blocks is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    Threading Building Blocks is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/

#include "tbb/concurrent_monitor.h"
#include "tbb/atomic.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "harness.h"
#include "tbb/concurrent_monitor.cpp"

using namespace tbb;

//! Queuing lock with concurrent_monitor; to test concurrent_monitor::notify( Predicate p )
class QueuingMutex {
public:
    //! Construct unacquired mutex.
    QueuingMutex() { q_tail = NULL; }

    //! The scoped locking pattern
    class ScopedLock: internal::no_copy {
        void Initialize() { mutex = NULL; }
    public:
        ScopedLock() {Initialize();}
        ScopedLock( QueuingMutex& m ) { Initialize(); Acquire(m); }
        ~ScopedLock() { if( mutex ) Release(); }
        void Acquire( QueuingMutex& m );
        void Release();
        void SleepPerhaps();

    private:
        QueuingMutex* mutex;
        ScopedLock* next;
        uintptr_t going;
        internal::concurrent_monitor::thread_context thr_ctx;
    };

    friend class ScopedLock;
private:
    //! The last competitor requesting the lock
    atomic<ScopedLock*> q_tail;
    internal::concurrent_monitor waitq;
};

struct PredicateEq {
    void* p;
    PredicateEq( void* p_ ) : p(p_) {}
    bool operator() ( void* v ) const {return p==v;}
};

//! A method to acquire QueuingMutex lock
void QueuingMutex::ScopedLock::Acquire( QueuingMutex& m )
{
    // Must set all fields before the fetch_and_store, because once the
    // fetch_and_store executes, *this becomes accessible to other threads.
    mutex = &m;
    next  = NULL;
    going = 0;

    // The fetch_and_store must have release semantics, because we are
    // "sending" the fields initialized above to other processors.
    ScopedLock* pred = m.q_tail.fetch_and_store<tbb::release>(this);
    if( pred ) {
        __TBB_ASSERT( !pred->next, "the predecessor has another successor!");
        pred->next = this;
        for( int i=0; i<16; ++i ) {
            if( going!=0ul ) break;
            __TBB_Yield();
        }
        SleepPerhaps();
    }

    // Force acquire so that user's critical section receives correct values
    // from processor that was previously in the user's critical section.
    __TBB_load_with_acquire(going);
}

//! A method to release QueuingMutex lock
void QueuingMutex::ScopedLock::Release( )
{
    if( !next ) {
        if( this == mutex->q_tail.compare_and_swap<tbb::release>(NULL, this) ) {
            // this was the only item in the queue, and the queue is now empty.
            goto done;
        }
        // Someone in the queue
        spin_wait_while_eq( next, (ScopedLock*)0 );
    }
    __TBB_store_with_release(next->going, 1);
    mutex->waitq.notify( PredicateEq(next) );
done:
    Initialize();
}

//! Yield and block; go to sleep
void QueuingMutex::ScopedLock::SleepPerhaps()
{
    bool slept = false;
    internal::concurrent_monitor& mq = mutex->waitq;
    mq.prepare_wait( thr_ctx, this );
    while( going==0ul ) {
        if( (slept=mq.commit_wait( thr_ctx ))==true )
            break;
        mq.prepare_wait( thr_ctx, this );
    }
    if( !slept )
        mq.cancel_wait( thr_ctx );
}

// Spin lock with concurrent_monitor; to test concurrent_monitor::notify_all() and concurrent_monitor::notify()
class SpinMutex {
public:
    //! Construct unacquired mutex.
    SpinMutex() : toggle(false) { flag = 0; }

    //! The scoped locking pattern
    class ScopedLock: internal::no_copy {
        void Initialize() { mutex = NULL; }
    public:
        ScopedLock() {Initialize();}
        ScopedLock( SpinMutex& m ) { Initialize(); Acquire(m); }
        ~ScopedLock() { if( mutex ) Release(); }
        void Acquire( SpinMutex& m );
        void Release();
        void SleepPerhaps();

    private:
        SpinMutex* mutex;
        internal::concurrent_monitor::thread_context thr_ctx;
    };

    friend class ScopedLock;
private:
    tbb::atomic<unsigned> flag;
    bool toggle;
    internal::concurrent_monitor waitq;
};

//! A method to acquire SpinMutex lock
void SpinMutex::ScopedLock::Acquire( SpinMutex& m )
{
    mutex = &m;
retry:
    if( m.flag.compare_and_swap( 1, 0 )!=0 ) {
        SleepPerhaps();
        goto retry;
    }
}

//! A method to release SpinMutex lock
void SpinMutex::ScopedLock::Release()
{
    bool old_toggle = mutex->toggle;
    mutex->toggle = !mutex->toggle;
    mutex->flag = 0;
    if( old_toggle )
        mutex->waitq.notify_one();
    else
        mutex->waitq.notify_all();
}

//! Yield and block; go to sleep
void SpinMutex::ScopedLock::SleepPerhaps()
{
    bool slept = false;
    internal::concurrent_monitor& mq = mutex->waitq;
    mq.prepare_wait( thr_ctx, this );
    while( mutex->flag ) {
        if( (slept=mq.commit_wait( thr_ctx ))==true )
            break;
        mq.prepare_wait( thr_ctx, this );
    }
    if( !slept )
        mq.cancel_wait( thr_ctx );
}

template<typename M>
struct Counter {
    typedef M mutex_type;
    M mutex;
    volatile long value;
};

//! Function object for use with parallel_for.h.
template<typename C>
struct AddOne: NoAssign {
    C& counter;
    /** Increments counter once for each iteration in the iteration space. */
    void operator()( tbb::blocked_range<size_t>& range ) const {
        for( size_t i=range.begin(); i!=range.end(); ++i ) {
            typename C::mutex_type::ScopedLock lock(counter.mutex);
            counter.value = counter.value+1;
        }
    }
    AddOne( C& counter_ ) : counter(counter_) {}
};

//! Generic test of a TBB mutex type M.
/** Does not test features specific to reader-writer locks. */
template<typename M>
void Test() {
    Counter<M> counter;
    counter.value = 0;
    const int n = 100000;
    tbb::parallel_for(tbb::blocked_range<size_t>(0,n,n/10),AddOne<Counter<M> >(counter));
    if( counter.value!=n )
        REPORT("ERROR : counter.value=%ld\n",counter.value);
}

int TestMain () {
    for( int p=MinThread; p<=MaxThread; ++p ) {
        REMARK( "testing with %d workers\n", static_cast<int>(p) );
        // test the predicated notify 
        Test<QueuingMutex>();
        // test the notify_all method
        Test<SpinMutex>();
        REMARK( "calling destructor for task_scheduler_init\n" );
    }
    return Harness::Done;
}
