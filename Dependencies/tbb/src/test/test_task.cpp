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

#include "tbb/task.h"
#include "tbb/atomic.h"
#include "tbb/tbb_thread.h"
#include "harness_assert.h"
#include <cstdlib>

//------------------------------------------------------------------------
// Helper for verifying that old use cases of spawn syntax still work.
//------------------------------------------------------------------------
tbb::task* GetTaskPtr( int& counter ) {
    ++counter;
    return NULL;
}
 
//------------------------------------------------------------------------
// Test for task::spawn_children and task_list
//------------------------------------------------------------------------

class UnboundedlyRecursiveOnUnboundedStealingTask : public tbb::task {
    typedef UnboundedlyRecursiveOnUnboundedStealingTask this_type;

    this_type *m_Parent;
    const int m_Depth; 
    volatile bool m_GoAhead;

    // Well, virtually unboundedly, for any practical purpose
    static const int max_depth = 1000000; 

public:
    UnboundedlyRecursiveOnUnboundedStealingTask( this_type *parent_ = NULL, int depth_ = max_depth )
        : m_Parent(parent_)
        , m_Depth(depth_)
        , m_GoAhead(true)
    {}

    /*override*/
    tbb::task* execute() {
        // Using large padding array sppeds up reaching stealing limit
        const int paddingSize = 16 * 1024;
        volatile char padding[paddingSize];
        if( !m_Parent || (m_Depth > 0 &&  m_Parent->m_GoAhead) ) {
            if ( m_Parent ) {
                // We are stolen, let our parent to start waiting for us
                m_Parent->m_GoAhead = false;
            }
            tbb::task &t = *new( tbb::task::allocate_child() ) this_type(this, m_Depth - 1);
            set_ref_count( 2 );
            spawn( t );
            // Give a willing thief a chance to steal
            for( int i = 0; i < 1000000 && m_GoAhead; ++i ) {
                ++padding[i % paddingSize];
                __TBB_Yield();
            }
            // If our child has not been stolen yet, then prohibit it siring ones 
            // of its own (when this thread executes it inside the next wait_for_all)
            m_GoAhead = false;
            wait_for_all();
        }
        return NULL;
    }
}; // UnboundedlyRecursiveOnUnboundedStealingTask

tbb::atomic<int> Count;

class RecursiveTask: public tbb::task {
    const int m_ChildCount;
    const int m_Depth; 
    //! Spawn tasks in list.  Exact method depends upon m_Depth&bit_mask.
    void SpawnList( tbb::task_list& list, int bit_mask ) {
        if( m_Depth&bit_mask ) {
            // Take address to check that signature of spawn(task_list&) is static.
            void (*s)(tbb::task_list&) = &tbb::task::spawn;
            (*s)(list);
            ASSERT( list.empty(), NULL );
            wait_for_all();
        } else {
            spawn_and_wait_for_all(list);
            ASSERT( list.empty(), NULL );
        }
    }
public:
    RecursiveTask( int child_count, int depth_ ) : m_ChildCount(child_count), m_Depth(depth_) {}
    /*override*/ tbb::task* execute() {
        ++Count;
        if( m_Depth>0 ) {
            tbb::task_list list;
            ASSERT( list.empty(), NULL );
            for( int k=0; k<m_ChildCount; ++k ) {
                list.push_back( *new( tbb::task::allocate_child() ) RecursiveTask(m_ChildCount/2,m_Depth-1 ) );
                ASSERT( !list.empty(), NULL );
            }
            set_ref_count( m_ChildCount+1 );
            SpawnList( list, 1 );
            // Now try reusing this as the parent.
            set_ref_count(2);
            list.push_back( *new (tbb::task::allocate_child() ) tbb::empty_task() );
            SpawnList( list, 2 );
        }
        return NULL;
    }
};

//! Compute what Count should be after RecursiveTask(child_count,depth) runs.
static int Expected( int child_count, int depth ) {
    return depth<=0 ? 1 : 1+child_count*Expected(child_count/2,depth-1);
}

#include "tbb/task_scheduler_init.h"
#include "harness.h"

void TestStealLimit( int nthread ) {
    REMARK( "testing steal limiting heuristics for %d threads\n", nthread );
    tbb::task_scheduler_init init(nthread);
    tbb::task &t = *new( tbb::task::allocate_root() ) UnboundedlyRecursiveOnUnboundedStealingTask();
    tbb::task::spawn_root_and_wait(t);
}

//! Test task::spawn( task_list& )
void TestSpawnChildren( int nthread ) {
    REMARK("testing task::spawn(task_list&) for %d threads\n",nthread);
    tbb::task_scheduler_init init(nthread);
    for( int j=0; j<50; ++j ) {
        Count = 0;
        RecursiveTask& p = *new( tbb::task::allocate_root() ) RecursiveTask(j,4);
        tbb::task::spawn_root_and_wait(p);
        int expected = Expected(j,4);
        ASSERT( Count==expected, NULL );
    }
}

//! Test task::spawn_root_and_wait( task_list& )
void TestSpawnRootList( int nthread ) {
    REMARK("testing task::spawn_root_and_wait(task_list&) for %d threads\n",nthread);
    tbb::task_scheduler_init init(nthread);
    for( int j=0; j<5; ++j )
        for( int k=0; k<10; ++k ) {
            Count = 0;
            tbb::task_list list; 
            for( int i=0; i<k; ++i )
                list.push_back( *new( tbb::task::allocate_root() ) RecursiveTask(j,4) );
            tbb::task::spawn_root_and_wait(list);
            int expected = k*Expected(j,4);
            ASSERT( Count==expected, NULL );
        }    
}

//------------------------------------------------------------------------
// Test for task::recycle_as_safe_continuation
//------------------------------------------------------------------------

class TaskGenerator: public tbb::task {
    int m_ChildCount;
    int m_Depth;
    
public:
    TaskGenerator( int child_count, int _depth ) : m_ChildCount(child_count), m_Depth(_depth) {}
    ~TaskGenerator( ) { m_ChildCount = m_Depth = -125; }

    /*override*/ tbb::task* execute() {
        ASSERT( m_ChildCount>=0 && m_Depth>=0, NULL );
        if( m_Depth>0 ) {
            recycle_as_safe_continuation();
            set_ref_count( m_ChildCount+1 );
            int k=0; 
            for( int j=0; j<m_ChildCount; ++j ) {
                tbb::task& t = *new( allocate_child() ) TaskGenerator(m_ChildCount/2,m_Depth-1);
                GetTaskPtr(k)->spawn(t);
            }
            ASSERT(k==m_ChildCount,NULL);
            --m_Depth;
            __TBB_Yield();
            ASSERT( state()==recycle && ref_count()>0, NULL);
        }
        return NULL;
    }
};

void TestSafeContinuation( int nthread ) {
    REMARK("testing task::recycle_as_safe_continuation for %d threads\n",nthread);
    tbb::task_scheduler_init init(nthread);
    for( int j=8; j<33; ++j ) {
        TaskGenerator& p = *new( tbb::task::allocate_root() ) TaskGenerator(j,5);
        tbb::task::spawn_root_and_wait(p);
    }
}

//------------------------------------------------------------------------
// Test affinity interface
//------------------------------------------------------------------------
tbb::atomic<int> TotalCount;

struct AffinityTask: public tbb::task {
    const tbb::task::affinity_id expected_affinity_id; 
    bool noted;
    /** Computing affinities is NOT supported by TBB, and may disappear in the future.
        It is done here for sake of unit testing. */
    AffinityTask( int expected_affinity_id_ ) : 
        expected_affinity_id(tbb::task::affinity_id(expected_affinity_id_)), 
        noted(false) 
    {
        set_affinity(expected_affinity_id);
        ASSERT( 0u-expected_affinity_id>0u, "affinity_id not an unsigned integral type?" );  
        ASSERT( affinity()==expected_affinity_id, NULL );
    } 
    /*override*/ tbb::task* execute() {
        ++TotalCount;
        return NULL;
    }
    /*override*/ void note_affinity( affinity_id id ) {
        // There is no guarantee in TBB that a task runs on its affinity thread.
        // However, the current implementation does accidentally guarantee it
        // under certain conditions, such as the conditions here.
        // We exploit those conditions for sake of unit testing.
        ASSERT( id!=expected_affinity_id, NULL );
        ASSERT( !noted, "note_affinity_id called twice!" );
        ASSERT ( &tbb::task::self() == (tbb::task*)this, "Wrong innermost running task" );
        noted = true;
    }
};

/** Note: This test assumes a lot about the internal implementation of affinity.
    Do NOT use this as an example of good programming practice with TBB */
void TestAffinity( int nthread ) {
    TotalCount = 0;
    int n = tbb::task_scheduler_init::default_num_threads();
    if( n>nthread ) 
        n = nthread;
    tbb::task_scheduler_init init(n);
    tbb::empty_task* t = new( tbb::task::allocate_root() ) tbb::empty_task;
    tbb::task::affinity_id affinity_id = t->affinity();
    ASSERT( affinity_id==0, NULL );
    // Set ref_count for n-1 children, plus 1 for the wait.
    t->set_ref_count(n);
    // Spawn n-1 affinitized children.
    for( int i=1; i<n; ++i ) 
        tbb::task::spawn( *new(t->allocate_child()) AffinityTask(i) );
    if( n>1 ) {
        // Keep master from stealing
        while( TotalCount!=n-1 ) 
            __TBB_Yield();
    }
    // Wait for the children
    t->wait_for_all();
    int k = 0;
    GetTaskPtr(k)->destroy(*t);
    ASSERT(k==1,NULL);
}

struct NoteAffinityTask: public tbb::task {
    bool noted;
    NoteAffinityTask( int id ) : noted(false)
    {
        set_affinity(tbb::task::affinity_id(id));
    }
    ~NoteAffinityTask () {
        ASSERT (noted, "note_affinity has not been called");
    }
    /*override*/ tbb::task* execute() {
        return NULL;
    }
    /*override*/ void note_affinity( affinity_id /*id*/ ) {
        noted = true;
        ASSERT ( &tbb::task::self() == (tbb::task*)this, "Wrong innermost running task" );
    }
};

// This test checks one of the paths inside the scheduler by affinitizing the child task 
// to non-existent thread so that it is proxied in the local task pool but not retrieved 
// by another thread. 
// If no workers requested, the extra slot #2 is allocated for a worker thread to serve
// "enqueued" tasks. In this test, it is used only for the affinity purpose.
void TestNoteAffinityContext() {
    tbb::task_scheduler_init init(1);
    tbb::empty_task* t = new( tbb::task::allocate_root() ) tbb::empty_task;
    t->set_ref_count(2);
    // This master in the absence of workers will have an affinity id of 1. 
    // So use another number to make the task get proxied.
    tbb::task::spawn( *new(t->allocate_child()) NoteAffinityTask(2) );
    t->wait_for_all();
    tbb::task::destroy(*t);
}

//------------------------------------------------------------------------
// Test that recovery actions work correctly for task::allocate_* methods
// when a task's constructor throws an exception.
//------------------------------------------------------------------------

#if TBB_USE_EXCEPTIONS
static int TestUnconstructibleTaskCount;

struct ConstructionFailure {
};

#if _MSC_VER && !defined(__INTEL_COMPILER)
    // Suppress pointless "unreachable code" warning.
    #pragma warning (push)
    #pragma warning (disable: 4702)
#endif

//! Task that cannot be constructed.  
template<size_t N>
struct UnconstructibleTask: public tbb::empty_task {
    char space[N];
    UnconstructibleTask() {
        throw ConstructionFailure();
    }
};

#if _MSC_VER && !defined(__INTEL_COMPILER)
    #pragma warning (pop)
#endif

#define TRY_BAD_CONSTRUCTION(x)                  \
    {                                            \
        try {                                    \
            new(x) UnconstructibleTask<N>;       \
        } catch( const ConstructionFailure& ) {                                                    \
            ASSERT( parent()==original_parent, NULL ); \
            ASSERT( ref_count()==original_ref_count, "incorrectly changed ref_count" );\
            ++TestUnconstructibleTaskCount;      \
        }                                        \
    }

template<size_t N>
struct RootTaskForTestUnconstructibleTask: public tbb::task {
    tbb::task* execute() {
        tbb::task* original_parent = parent();
        ASSERT( original_parent!=NULL, NULL );
        int original_ref_count = ref_count();
        TRY_BAD_CONSTRUCTION( allocate_root() );
        TRY_BAD_CONSTRUCTION( allocate_child() );
        TRY_BAD_CONSTRUCTION( allocate_continuation() );
        TRY_BAD_CONSTRUCTION( allocate_additional_child_of(*this) );
        return NULL;
    }
};

template<size_t N>
void TestUnconstructibleTask() {
    TestUnconstructibleTaskCount = 0;
    tbb::task_scheduler_init init;
    tbb::task* t = new( tbb::task::allocate_root() ) RootTaskForTestUnconstructibleTask<N>;
    tbb::task::spawn_root_and_wait(*t);
    ASSERT( TestUnconstructibleTaskCount==4, NULL );
}
#endif /* TBB_USE_EXCEPTIONS */

//------------------------------------------------------------------------
// Test for alignment problems with task objects.
//------------------------------------------------------------------------

#if _MSC_VER && !defined(__INTEL_COMPILER)
    // Workaround for pointless warning "structure was padded due to __declspec(align())
    #pragma warning (push)
    #pragma warning (disable: 4324)
#endif

//! Task with members of type T.
/** The task recursively creates tasks. */
template<typename T> 
class TaskWithMember: public tbb::task {
    T x;
    T y;
    unsigned char count;
    /*override*/ tbb::task* execute() {
        x = y;
        if( count>0 ) { 
            set_ref_count(2);
            tbb::task* t = new( tbb::task::allocate_child() ) TaskWithMember<T>(count-1);
            spawn_and_wait_for_all(*t);
        }
        return NULL;
    }
public:
    TaskWithMember( unsigned char n ) : count(n) {}
};

#if _MSC_VER && !defined(__INTEL_COMPILER)
    #pragma warning (pop)
#endif

template<typename T> 
void TestAlignmentOfOneClass() {
    typedef TaskWithMember<T> task_type;
    tbb::task* t = new( tbb::task::allocate_root() ) task_type(10);
    tbb::task::spawn_root_and_wait(*t);
}

#include "harness_m128.h"

void TestAlignment() {
    REMARK("testing alignment\n");
    tbb::task_scheduler_init init;
    // Try types that have variety of alignments
    TestAlignmentOfOneClass<char>();
    TestAlignmentOfOneClass<short>();
    TestAlignmentOfOneClass<int>();
    TestAlignmentOfOneClass<long>();
    TestAlignmentOfOneClass<void*>();
    TestAlignmentOfOneClass<float>();
    TestAlignmentOfOneClass<double>();
#if HAVE_m128
    TestAlignmentOfOneClass<__m128>();
#endif /* HAVE_m128 */
}

//------------------------------------------------------------------------
// Test for recursing on left while spawning on right
//------------------------------------------------------------------------

int Fib( int n );

struct RightFibTask: public tbb::task {
    int* y;
    const int n;
    RightFibTask( int* y_, int n_ ) : y(y_), n(n_) {}
    task* execute() {
        *y = Fib(n-1);
        return 0;
    } 
};

int Fib( int n ) {
    if( n<2 ) {
        return n;
    } else {
        // y actually does not need to be initialized.  It is initialized solely to suppress
        // a gratuitous warning "potentially uninitialized local variable". 
        int y=-1;
        tbb::task* root_task = new( tbb::task::allocate_root() ) tbb::empty_task;
        root_task->set_ref_count(2);
        tbb::task::spawn( *new( root_task->allocate_child() ) RightFibTask(&y,n) );
        int x = Fib(n-2);
        root_task->wait_for_all();
        tbb::task::destroy(*root_task);
        return y+x;
    }
}

void TestLeftRecursion( int p ) {
    REMARK("testing non-spawned roots for %d threads\n",p);
    tbb::task_scheduler_init init(p);
    int sum = 0; 
    for( int i=0; i<100; ++i )
        sum +=Fib(10);
    ASSERT( sum==5500, NULL );
}

//------------------------------------------------------------------------
// Test for computing with DAG of tasks.
//------------------------------------------------------------------------

class DagTask: public tbb::task {
    typedef unsigned long long number_t;
    const int i, j;
    number_t sum_from_left, sum_from_above;
    void check_sum( number_t sum ) {
        number_t expected_sum = 1;
        for( int k=i+1; k<=i+j; ++k ) 
            expected_sum *= k;
        for( int k=1; k<=j; ++k ) 
            expected_sum /= k;
        ASSERT(sum==expected_sum, NULL);
    }
public:
    DagTask *successor_to_below, *successor_to_right;
    DagTask( int i_, int j_ ) : i(i_), j(j_), sum_from_left(0), sum_from_above(0) {}
    task* execute() {
        __TBB_ASSERT( ref_count()==0, NULL );
        number_t sum = i==0 && j==0 ? 1 : sum_from_left+sum_from_above;
        check_sum(sum);
        ++execution_count;
        if( DagTask* t = successor_to_right ) {
            t->sum_from_left = sum;
            if( t->decrement_ref_count()==0 )
                // Test using spawn to evaluate DAG
                spawn( *t );
        }
        if( DagTask* t = successor_to_below ) {
            t->sum_from_above = sum;
            if( t->decrement_ref_count()==0 ) 
                // Test using bypass to evaluate DAG
                return t;
        } 
        return NULL;  
    }
    ~DagTask() {++destruction_count;}
    static tbb::atomic<int> execution_count;
    static tbb::atomic<int> destruction_count;
};

tbb::atomic<int> DagTask::execution_count;
tbb::atomic<int> DagTask::destruction_count;

void TestDag( int p ) {
    REMARK("testing evaluation of DAG for %d threads\n",p);
    tbb::task_scheduler_init init(p);
    DagTask::execution_count=0;
    DagTask::destruction_count=0;
    const int n = 10;
    DagTask* a[n][n];
    for( int i=0; i<n; ++i ) 
        for( int j=0; j<n; ++j )
            a[i][j] = new( tbb::task::allocate_root() ) DagTask(i,j);
    for( int i=0; i<n; ++i ) 
        for( int j=0; j<n; ++j ) {
            a[i][j]->successor_to_below = i+1<n ? a[i+1][j] : NULL;
            a[i][j]->successor_to_right = j+1<n ? a[i][j+1] : NULL;
            a[i][j]->set_ref_count((i>0)+(j>0));
        }
    a[n-1][n-1]->increment_ref_count();
    a[n-1][n-1]->spawn_and_wait_for_all(*a[0][0]);
    ASSERT( DagTask::execution_count == n*n - 1, NULL );
    tbb::task::destroy(*a[n-1][n-1]);
    ASSERT( DagTask::destruction_count > n*n - p, NULL );
    while ( DagTask::destruction_count != n*n )
        __TBB_Yield();
}

#include "harness_barrier.h"

class RelaxedOwnershipTask: public tbb::task {
    tbb::task &m_taskToSpawn,
              &m_taskToDestroy,
              &m_taskToExecute;
    static Harness::SpinBarrier m_barrier;

    tbb::task* execute () {
        tbb::task &p = *parent();
        tbb::task &r = *new( tbb::task::allocate_root() ) tbb::empty_task;
        r.set_ref_count( 1 );
        m_barrier.wait();
        p.spawn( *new(p.allocate_child()) tbb::empty_task );
        p.spawn( *new(task::allocate_additional_child_of(p)) tbb::empty_task );
        p.spawn( m_taskToSpawn );
        p.destroy( m_taskToDestroy );
        r.spawn_and_wait_for_all( m_taskToExecute );
        p.destroy( r );
        return NULL;
    }
public:
    RelaxedOwnershipTask ( tbb::task& toSpawn, tbb::task& toDestroy, tbb::task& toExecute )
        : m_taskToSpawn(toSpawn)
        , m_taskToDestroy(toDestroy)
        , m_taskToExecute(toExecute)
    {}
    static void SetBarrier ( int numThreads ) { m_barrier.initialize( numThreads ); }
};

Harness::SpinBarrier RelaxedOwnershipTask::m_barrier;

void TestRelaxedOwnership( int p ) {
    if ( p < 2 )
        return;

    if( unsigned(p)>tbb::tbb_thread::hardware_concurrency() )
        return;

    REMARK("testing tasks exercising relaxed ownership freedom for %d threads\n", p);
    tbb::task_scheduler_init init(p);
    RelaxedOwnershipTask::SetBarrier(p);
    tbb::task &r = *new( tbb::task::allocate_root() ) tbb::empty_task;
    tbb::task_list tl;
    for ( int i = 0; i < p; ++i ) {
        tbb::task &tS = *new( r.allocate_child() ) tbb::empty_task,
                  &tD = *new( r.allocate_child() ) tbb::empty_task,
                  &tE = *new( r.allocate_child() ) tbb::empty_task;
        tl.push_back( *new( r.allocate_child() ) RelaxedOwnershipTask(tS, tD, tE) );
    }
    r.set_ref_count( 5 * p + 1 );
    int k=0;
    GetTaskPtr(k)->spawn( tl );
    ASSERT(k==1,NULL);
    r.wait_for_all();
    r.destroy( r );
}

//------------------------------------------------------------------------
// Test for running TBB scheduler on user-created thread.
//------------------------------------------------------------------------

void RunSchedulerInstanceOnUserThread( int n_child ) {
    tbb::task* e = new( tbb::task::allocate_root() ) tbb::empty_task;
    e->set_ref_count(1+n_child);
    for( int i=0; i<n_child; ++i )
        tbb::task::spawn( *new(e->allocate_child()) tbb::empty_task );
    e->wait_for_all();
    e->destroy(*e);
}

void TestUserThread( int p ) {
    tbb::task_scheduler_init init(p);
    // Try with both 0 and 1 children.  Only the latter scenario permits stealing.
    for( int n_child=0; n_child<2; ++n_child ) {
        tbb::tbb_thread t( RunSchedulerInstanceOnUserThread, n_child );
        t.join();
    }
}


class TaskWithChildToSteal : public tbb::task {
    const int m_Depth; 
    volatile bool m_GoAhead;

public:
    TaskWithChildToSteal( int depth_ )
        : m_Depth(depth_)
        , m_GoAhead(false)
    {}

    /*override*/
    tbb::task* execute() {
        m_GoAhead = true;
        if ( m_Depth > 0 ) {
            TaskWithChildToSteal &t = *new( tbb::task::allocate_child() ) TaskWithChildToSteal(m_Depth - 1);
            t.SpawnAndWaitOnParent();
        }
        else
            Harness::Sleep(50); // The last task in chain sleeps for 50 ms
        return NULL;
    }

    void SpawnAndWaitOnParent() {
        parent()->set_ref_count( 2 );
        parent()->spawn( *this );
        while (!this->m_GoAhead )
            __TBB_Yield();
        parent()->wait_for_all();
    }
}; // TaskWithChildToSteal

void TestDispatchLoopResponsiveness() {
    REMARK("testing that dispatch loops do not go into eternal sleep when all remaining children are stolen\n");
    // Recursion depth values test the following sorts of dispatch loops
    // 0 - master's outermost
    // 1 - worker's nested
    // 2 - master's nested
    tbb::task_scheduler_init init(2);
    tbb::task &r = *new( tbb::task::allocate_root() ) tbb::empty_task;
    for ( int depth = 0; depth < 3; ++depth ) {
        TaskWithChildToSteal &t = *new( r.allocate_child() ) TaskWithChildToSteal(depth);
        t.SpawnAndWaitOnParent();
    }
    r.destroy(r);
    // The success criteria of this test is not hanging
}

void TestWaitDiscriminativenessWithoutStealing() {
    REMARK( "testing that task::wait_for_all is specific to the root it is called on (no workers)\n" );
    // The test relies on the strict LIFO scheduling order in the absence of workers
    tbb::task_scheduler_init init(1);
    tbb::task &r1 = *new( tbb::task::allocate_root() ) tbb::empty_task;
    tbb::task &r2 = *new( tbb::task::allocate_root() ) tbb::empty_task;
    const int NumChildren = 10;
    r1.set_ref_count( NumChildren + 1 );
    r2.set_ref_count( NumChildren + 1 );
    for( int i=0; i < NumChildren; ++i ) {
        tbb::empty_task &t1 = *new( r1.allocate_child() ) tbb::empty_task;
        tbb::empty_task &t2 = *new( r2.allocate_child() ) tbb::empty_task;
        tbb::task::spawn(t1);
        tbb::task::spawn(t2);
    }
    r2.wait_for_all();
    ASSERT( r2.ref_count() <= 1, "Not all children of r2 executed" );
    ASSERT( r1.ref_count() > 1, "All children of r1 prematurely executed" );
    r1.wait_for_all();
    ASSERT( r1.ref_count() <= 1, "Not all children of r1 executed" );
    r1.destroy(r1);
    r2.destroy(r2);
}


using tbb::internal::spin_wait_until_eq;

//! Deterministic emulation of a long running task
class LongRunningTask : public tbb::task {
    volatile bool& m_CanProceed;

    tbb::task* execute() {
        spin_wait_until_eq( m_CanProceed, true );
        return NULL;
    }
public:
    LongRunningTask ( volatile bool& canProceed ) : m_CanProceed(canProceed) {}
};

void TestWaitDiscriminativenessWithStealing() {
    if( tbb::tbb_thread::hardware_concurrency() < 2 )
        return;
    REMARK( "testing that task::wait_for_all is specific to the root it is called on (one worker)\n" );
    volatile bool canProceed = false;
    tbb::task_scheduler_init init(2);
    tbb::task &r1 = *new( tbb::task::allocate_root() ) tbb::empty_task;
    tbb::task &r2 = *new( tbb::task::allocate_root() ) tbb::empty_task;
    r1.set_ref_count( 2 );
    r2.set_ref_count( 2 );
    tbb::task& t1 = *new( r1.allocate_child() ) tbb::empty_task;
    tbb::task& t2 = *new( r2.allocate_child() ) LongRunningTask(canProceed);
    tbb::task::spawn(t2);
    tbb::task::spawn(t1);
    r1.wait_for_all();
    ASSERT( r1.ref_count() <= 1, "Not all children of r1 executed" );
    ASSERT( r2.ref_count() == 2, "All children of r2 prematurely executed" );
    canProceed = true;
    r2.wait_for_all();
    ASSERT( r2.ref_count() <= 1, "Not all children of r2 executed" );
    r1.destroy(r1);
    r2.destroy(r2);
}

struct MasterBody : NoAssign, Harness::NoAfterlife {
    static Harness::SpinBarrier my_barrier;

    class BarrenButLongTask : public tbb::task {
        volatile bool& m_Started;
        volatile bool& m_CanProceed;

        tbb::task* execute() {
            m_Started = true;
            spin_wait_until_eq( m_CanProceed, true );
            volatile int k = 0;
            for ( int i = 0; i < 1000000; ++i ) ++k;
            return NULL;
        }
    public:
        BarrenButLongTask ( volatile bool& started, volatile bool& can_proceed )
            : m_Started(started), m_CanProceed(can_proceed)
        {}
    };

    class BinaryRecursiveTask : public tbb::task {
        int m_Depth;

        tbb::task* execute() {
            if( !m_Depth )
                return NULL;
            set_ref_count(3);
            spawn( *new( tbb::task::allocate_child() ) BinaryRecursiveTask(m_Depth - 1) );
            spawn( *new( tbb::task::allocate_child() ) BinaryRecursiveTask(m_Depth - 1) );
            wait_for_all();
            return NULL;
        }

        void note_affinity( affinity_id ) {
            __TBB_ASSERT( false, "These tasks cannot be stolen" );
        }
    public:
        BinaryRecursiveTask ( int depth_ ) : m_Depth(depth_) {}
    };

    void operator() ( int id ) const {
        if ( id ) {
            tbb::task_scheduler_init init(2);
            volatile bool child_started = false,
                          can_proceed = false;
            tbb::task& r = *new( tbb::task::allocate_root() ) tbb::empty_task;
            r.set_ref_count(2);
            r.spawn( *new(r.allocate_child()) BarrenButLongTask(child_started, can_proceed) );
            spin_wait_until_eq( child_started, true );
            my_barrier.wait();
            can_proceed = true;
            r.wait_for_all();
            r.destroy(r);
        }
        else {
            my_barrier.wait();
            tbb::task_scheduler_init init(1);
            Count = 0;
            int depth = 16;
            BinaryRecursiveTask& r = *new( tbb::task::allocate_root() ) BinaryRecursiveTask(depth);
            tbb::task::spawn_root_and_wait(r);
        }
    }
public:
    MasterBody ( int num_masters ) { my_barrier.initialize(num_masters); }
};

Harness::SpinBarrier MasterBody::my_barrier;

/** Ensures that tasks spawned by a master thread or one of the workers servicing
    it cannot be stolen by another master thread. **/
void TestMastersIsolation ( int p ) {
    // The test requires at least 3-way parallelism to work correctly
    if ( p > 2 && tbb::task_scheduler_init::default_num_threads() >= p ) {
        tbb::task_scheduler_init init(p);
        NativeParallelFor( p, MasterBody(p) );
    }
}

//------------------------------------------------------------------------
// Test for tbb::task::enqueue
//------------------------------------------------------------------------

const int PairsPerTrack = 100;

class EnqueuedTask : public tbb::task {
    task* my_successor;
    int my_enqueue_order;
    int* my_track;
    tbb::task* execute() {
        // Capture execution order in the very beginning
        int execution_order = 2 - my_successor->decrement_ref_count();
        // Create some local work.
        TaskGenerator& p = *new( tbb::task::allocate_root() ) TaskGenerator(2,2);
        tbb::task::spawn_root_and_wait(p);
        if( execution_order==2 ) { // the "slower" of two peer tasks
            ++nCompletedPairs;
            // Of course execution order can differ from dequeue order.
            // But there is no better approximation at hand; and a single worker
            // will execute in dequeue order, which is enough for our check.
            if (my_enqueue_order==execution_order)
                ++nOrderedPairs;
            FireTwoTasks(my_track);
            destroy(*my_successor);
        }
        return NULL;
    }
public:
    EnqueuedTask( task* successor, int enq_order, int* track )
    : my_successor(successor), my_enqueue_order(enq_order), my_track(track) {}

    // Create and enqueue two tasks
    static void FireTwoTasks( int* track ) {
        int progress = ++*track;
        if( progress < PairsPerTrack ) {
            task* successor = new (tbb::task::allocate_root()) tbb::empty_task;
            successor->set_ref_count(2);
            enqueue( *new (tbb::task::allocate_root()) EnqueuedTask(successor, 1, track) );
            enqueue( *new (tbb::task::allocate_root()) EnqueuedTask(successor, 2, track) );
        }
    }

    static tbb::atomic<int> nCompletedPairs;
    static tbb::atomic<int> nOrderedPairs;
};

tbb::atomic<int> EnqueuedTask::nCompletedPairs;
tbb::atomic<int> EnqueuedTask::nOrderedPairs;

const int nTracks = 10;
static int TaskTracks[nTracks];
const int stall_threshold = 100000;

void TimedYield( double pause_time );

class ProgressMonitor {
public:
    void operator() ( ) {
        int track_snapshot[nTracks];
        int stall_count = 0, uneven_progress_count = 0, last_progress_mask = 0;
        for(int i=0; i<nTracks; ++i)
            track_snapshot[i]=0;
        bool completed;
        do {
            // Yield repeatedly for at least 1 usec
            TimedYield( 1E-6 );
            int overall_progress = 0, progress_mask = 0;
            const int all_progressed = (1<<nTracks) - 1;
            completed = true;
            for(int i=0; i<nTracks; ++i) {
                int ti = TaskTracks[i];
                int pi = ti-track_snapshot[i];
                if( pi ) progress_mask |= 1<<i;
                overall_progress += pi;
                completed = completed && ti==PairsPerTrack;
                track_snapshot[i]=ti;
            }
            // The constants in the next asserts are subjective and may need correction.
            if( overall_progress )
                stall_count=0;
            else {
                ++stall_count;
                // no progress for at least 0.1 s; consider it dead.
                ASSERT(stall_count < stall_threshold, "no progress on enqueued tasks; deadlock?");
            }
            if( progress_mask==all_progressed || progress_mask^last_progress_mask ) {
                uneven_progress_count = 0;
                last_progress_mask = progress_mask;
            }
            else if ( overall_progress > 2 ) {
                ++uneven_progress_count;
                ASSERT(uneven_progress_count < 5, "some enqueued tasks seem stalling; no simultaneous progress?");
            }
        } while( !completed );
    }
};

void TestEnqueue( int p ) {
    REMARK("testing task::enqueue for %d threads\n", p);
    for(int mode=0;mode<3;++mode) {
        tbb::task_scheduler_init init(p);
        EnqueuedTask::nCompletedPairs = EnqueuedTask::nOrderedPairs = 0;
        for(int i=0; i<nTracks; ++i) {
            TaskTracks[i] = -1; // to accomodate for the starting call
            EnqueuedTask::FireTwoTasks(TaskTracks+i);
        }
        ProgressMonitor pm;
        tbb::tbb_thread thr( pm );
        if(mode==1) {
            // do some parallel work in the meantime
            for(int i=0; i<10; i++) {
                TaskGenerator& g = *new( tbb::task::allocate_root() ) TaskGenerator(2,5);
                tbb::task::spawn_root_and_wait(g);
                TimedYield( 1E-6 );
            }
        }
        if( mode==2 ) {
            // Additionally enqueue a bunch of empty tasks. The goal is to test that tasks
            // allocated and enqueued by a thread are safe to use after the thread leaves TBB.
            tbb::task* root = new (tbb::task::allocate_root()) tbb::empty_task;
            root->set_ref_count(100);
            for( int i=0; i<100; ++i )
                tbb::task::enqueue( *new (root->allocate_child()) tbb::empty_task );
            init.terminate(); // master thread deregistered
        }
        thr.join();
        ASSERT(EnqueuedTask::nCompletedPairs==nTracks*PairsPerTrack, NULL);
        ASSERT(EnqueuedTask::nOrderedPairs<EnqueuedTask::nCompletedPairs,
            "all task pairs executed in enqueue order; de facto guarantee is too strong?");
    }
}

//------------------------------------------------------------------------
// Run all tests.
//------------------------------------------------------------------------

int TestMain () {
#if TBB_USE_EXCEPTIONS
    TestUnconstructibleTask<1>();
    TestUnconstructibleTask<10000>();
#endif
    TestAlignment();
    TestNoteAffinityContext();
    TestDispatchLoopResponsiveness();
    TestWaitDiscriminativenessWithoutStealing();
    TestWaitDiscriminativenessWithStealing();
    for( int p=MinThread; p<=MaxThread; ++p ) {
        TestSpawnChildren( p );
        TestSpawnRootList( p );
        TestSafeContinuation( p );
        TestEnqueue( p );
        TestLeftRecursion( p );
        TestDag( p );
        TestAffinity( p );
        TestUserThread( p );
        TestStealLimit( p );
        TestRelaxedOwnership( p );
#if __TBB_ARENA_PER_MASTER
        TestMastersIsolation( p );
#endif /* __TBB_ARENA_PER_MASTER */
    }
    return Harness::Done;
}

#include "tbb/tick_count.h"
void TimedYield( double pause_time ) {
    tbb::tick_count start = tbb::tick_count::now();
    while( (tbb::tick_count::now()-start).seconds() < pause_time )
        __TBB_Yield();
}

