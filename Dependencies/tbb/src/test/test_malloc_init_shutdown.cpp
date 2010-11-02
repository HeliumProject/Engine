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

#include "tbb/scalable_allocator.h"
#include "tbb/atomic.h"
#include "tbb/aligned_space.h"
#include "../tbb/tbb_assert_impl.h"

#if _WIN64 && defined(_M_AMD64) && !__MINGW64__
void __TBB_machine_pause(__int32 /*delay*/ ) {}
#elif  __linux__ && __ia64__
#include <pthread.h>

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

int32_t __TBB_machine_fetchadd4__TBB_full_fence (volatile void *ptr, int32_t value)
{
    pthread_mutex_lock(&counter_mutex);
    int32_t result = *(int32_t*)ptr;
    *(int32_t*)ptr = result + value;
    pthread_mutex_unlock(&counter_mutex);
    return result;
}

void __TBB_machine_pause(int32_t /*delay*/) {}
#endif

#include "harness.h"
#include "harness_barrier.h"

tbb::atomic<int> FinishedTasks;
const int MaxTasks = 16;

/*--------------------------------------------------------------------*/
// The regression test against a bug triggered when malloc initialization 
// and thread shutdown were called simultaneously, in which case
// Windows dynamic loader lock and allocator initialization/termination lock
// were taken in different order.

class TestFunc1 {
    Harness::SpinBarrier* my_barr;
public:
    TestFunc1 (Harness::SpinBarrier& barr) : my_barr(&barr) {}
    void operator() (bool do_malloc) const {
        my_barr->wait();
        if (do_malloc) scalable_malloc(10);
        ++FinishedTasks;
    }
};

typedef NativeParallelForTask<bool,TestFunc1> TestTask1;

void Test1 () {
    int NTasks = min(MaxTasks, max(2, MaxThread));
    Harness::SpinBarrier barr(NTasks);
    TestFunc1 tf(barr);
    FinishedTasks = 0;
    tbb::aligned_space<TestTask1,MaxTasks> tasks;

    for(int i=0; i<NTasks; ++i) {
        TestTask1* t = tasks.begin()+i;
        new(t) TestTask1(i%2==0, tf);
        t->start();
    }
    
    Harness::Sleep(1000); // wait a second :)
    ASSERT( FinishedTasks==NTasks, "Some threads appear to deadlock" );

    for(int i=0; i<NTasks; ++i) {
        TestTask1* t = tasks.begin()+i;
        t->wait_to_finish();
        t->~TestTask1();
    }
}

/*--------------------------------------------------------------------*/
// The regression test against a bug when cross-thread deallocation
// caused livelock at thread shutdown.

void* ptr = NULL;

class TestFunc2a {
    Harness::SpinBarrier* my_barr;
public:
    TestFunc2a (Harness::SpinBarrier& barr) : my_barr(&barr) {}
    void operator() (int) const {
        ptr = scalable_malloc(8);
        my_barr->wait();
        ++FinishedTasks;
    }
};

typedef NativeParallelForTask<int,TestFunc2a> TestTask2a;

class TestFunc2b: NoAssign {
    Harness::SpinBarrier* my_barr;
    TestTask2a& my_ward;
public:
    TestFunc2b (Harness::SpinBarrier& barr, TestTask2a& t) : my_barr(&barr), my_ward(t) {}
    void operator() (int) const {
        tbb::internal::spin_wait_while_eq(ptr, (void*)NULL);
        scalable_free(ptr);
        my_barr->wait();
        my_ward.wait_to_finish();
        ++FinishedTasks;
    }
};
void Test2() {
    Harness::SpinBarrier barr(2);
    TestFunc2a func2a(barr);
    TestTask2a t2a(0, func2a);
    TestFunc2b func2b(barr, t2a);
    NativeParallelForTask<int,TestFunc2b> t2b(1, func2b);
    FinishedTasks = 0;
    t2a.start(); t2b.start();
    Harness::Sleep(1000); // wait a second :)
    ASSERT( FinishedTasks==2, "Threads appear to deadlock" );
    t2b.wait_to_finish(); // t2a is monitored by t2b
}

int TestMain () {
    Test1(); // requires malloc initialization so should be first
    Test2();
    return Harness::Done;
}
