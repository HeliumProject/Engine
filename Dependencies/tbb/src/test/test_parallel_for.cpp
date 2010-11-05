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

// Test for function template parallel_for.h

#if _MSC_VER
#pragma warning (push)
#if !defined(__INTEL_COMPILER)
    // Suppress pointless "unreachable code" warning.
    #pragma warning (disable: 4702)
#endif
#if defined(_Wp64)
    // Workaround for overzealous compiler warnings in /Wp64 mode
    #pragma warning (disable: 4267)
#endif
#endif //#if _MSC_VER 

#include "tbb/parallel_for.h"
#include "tbb/atomic.h"
#include "harness_assert.h"
#include "harness.h"

static tbb::atomic<int> FooBodyCount;

//! An range object whose only public members are those required by the Range concept.
template<size_t Pad>
class FooRange {
    //! Start of range
    int start;

    //! Size of range
    int size;
    FooRange( int start_, int size_ ) : start(start_), size(size_) {
        zero_fill<char>(pad, Pad);
        pad[Pad-1] = 'x';
    }
    template<size_t Pad_> friend void Flog( int nthread );
    template<size_t Pad_> friend class FooBody;
    void operator&();

    char pad[Pad];
public:
    bool empty() const {return size==0;}
    bool is_divisible() const {return size>1;}
    FooRange( FooRange& original, tbb::split ) : size(original.size/2) {
        original.size -= size;
        start = original.start+original.size;
        ASSERT( original.pad[Pad-1]=='x', NULL );
        pad[Pad-1] = 'x';
    }
};

//! An range object whose only public members are those required by the parallel_for.h body concept.
template<size_t Pad>
class FooBody {
    static const int LIVE = 0x1234;
    tbb::atomic<int>* array;
    int state;
    friend class FooRange<Pad>;
    template<size_t Pad_> friend void Flog( int nthread );
    FooBody( tbb::atomic<int>* array_ ) : array(array_), state(LIVE) {}
public:
    ~FooBody() {
        --FooBodyCount;
        for( size_t i=0; i<sizeof(*this); ++i )
            reinterpret_cast<char*>(this)[i] = -1;
    }
    //! Copy constructor 
    FooBody( const FooBody& other ) : array(other.array), state(other.state) {
        ++FooBodyCount;
        ASSERT( state==LIVE, NULL );
    }
    void operator()( FooRange<Pad>& r ) const {
        for( int k=0; k<r.size; ++k )
            array[r.start+k]++;
    }
};

#include "tbb/tick_count.h"

static const int N = 1000;
static tbb::atomic<int> Array[N];

template<size_t Pad>
void Flog( int nthread ) {
    tbb::tick_count T0 = tbb::tick_count::now();
    for( int i=0; i<N; ++i ) {
        for ( int mode = 0; mode < 4; ++mode) 
        {
            FooRange<Pad> r( 0, i );
            const FooRange<Pad> rc = r;
            FooBody<Pad> f( Array );
            const FooBody<Pad> fc = f;
            memset( Array, 0, sizeof(Array) );
            FooBodyCount = 1;
            switch (mode) {
                case 0:
                    tbb::parallel_for( rc, fc );
                break;
                case 1:
                    tbb::parallel_for( rc, fc, tbb::simple_partitioner() );
                break;
                case 2:
                    tbb::parallel_for( rc, fc, tbb::auto_partitioner() );
                break;
                case 3: {
                    static tbb::affinity_partitioner affinity;
                    tbb::parallel_for( rc, fc, affinity );
                }
                break;
            }
            for( int j=0; j<i; ++j ) 
                ASSERT( Array[j]==1, NULL );
            for( int j=i; j<N; ++j ) 
                ASSERT( Array[j]==0, NULL );
            // Destruction of bodies might take a while, but there should be at most one body per thread
            // at this point.
            while( FooBodyCount>1 && FooBodyCount<=nthread )
                __TBB_Yield();
            ASSERT( FooBodyCount==1, NULL );
        }
    }
    tbb::tick_count T1 = tbb::tick_count::now();
    REMARK("time=%g\tnthread=%d\tpad=%d\n",(T1-T0).seconds(),nthread,int(Pad));
}

// Testing parallel_for with step support
const size_t PFOR_BUFFER_TEST_SIZE = 1024;
// test_buffer has some extra items beyound right bound
const size_t PFOR_BUFFER_ACTUAL_SIZE = PFOR_BUFFER_TEST_SIZE + 1024; 
size_t pfor_buffer[PFOR_BUFFER_ACTUAL_SIZE];

template<typename T>
class TestFunctor{
public:
    void operator ()(T index) const {
        pfor_buffer[index]++;
    }
};

#if !TBB_USE_EXCEPTIONS && _MSC_VER
    // Suppress "C++ exception handler used, but unwind semantics are not enabled" warning in STL headers
    #pragma warning (push)
    #pragma warning (disable: 4530)
#endif

#include <stdexcept> // std::invalid_argument

#if !TBB_USE_EXCEPTIONS && _MSC_VER
    #pragma warning (pop)
#endif

template <typename T>
void TestParallelForWithStepSupport()
{
    const T pfor_buffer_test_size = static_cast<T>(PFOR_BUFFER_TEST_SIZE);
    const T pfor_buffer_actual_size = static_cast<T>(PFOR_BUFFER_ACTUAL_SIZE);
    // Testing parallel_for with different step values
    for (T begin = 0; begin < pfor_buffer_test_size - 1; begin += pfor_buffer_test_size / 10 + 1) {
        T step;
        for (step = 1; step < pfor_buffer_test_size; step++) {
            memset(pfor_buffer, 0, pfor_buffer_actual_size * sizeof(size_t));
            if (step == 1){
                tbb::parallel_for(begin, pfor_buffer_test_size, TestFunctor<T>());
            } else {
                tbb::parallel_for(begin, pfor_buffer_test_size, step, TestFunctor<T>());
            }
            // Verifying that parallel_for processed all items it should
            for (T i = begin; i < pfor_buffer_test_size; i = i + step) {
                ASSERT(pfor_buffer[i] == 1, "parallel_for didn't process all required elements");
                pfor_buffer[i] = 0;
            }
            // Verifying that no extra items were processed and right bound of array wasn't crossed
            for (T i = 0; i < pfor_buffer_actual_size; i++) {
                ASSERT(pfor_buffer[i] == 0, "parallel_for processed an extra element");
            }
        }
    }

    // Testing some corner cases
    tbb::parallel_for(static_cast<T>(2), static_cast<T>(1), static_cast<T>(1), TestFunctor<T>());
#if TBB_USE_EXCEPTIONS && !__TBB_THROW_ACROSS_MODULE_BOUNDARY_BROKEN
    try{
        tbb::parallel_for(static_cast<T>(1), static_cast<T>(100), static_cast<T>(0), TestFunctor<T>());  // should cause std::invalid_argument
    }catch(std::invalid_argument){
        return;
    }
    catch ( ... ) {
        ASSERT ( __TBB_EXCEPTION_TYPE_INFO_BROKEN, "Unrecognized exception. std::invalid_argument is expected" );
    }
#endif /* TBB_USE_EXCEPTIONS && !__TBB_THROW_ACROSS_MODULE_BOUNDARY_BROKEN */
}

// Exception support test
#define HARNESS_EH_SIMPLE_MODE 1
#include "tbb/tbb_exception.h"
#include "harness_eh.h"

#if TBB_USE_EXCEPTIONS
class test_functor_with_exception {
public:
    void operator ()(size_t) const { ThrowTestException(); }
};

void TestExceptionsSupport() {
    REMARK (__FUNCTION__);
    { // Tests version with a step provided
        ResetEhGlobals();
        TRY();
            tbb::parallel_for((size_t)0, (size_t)PFOR_BUFFER_TEST_SIZE, (size_t)1, test_functor_with_exception());
        CATCH_AND_ASSERT();
    }
    { // Tests version without a step
        ResetEhGlobals();
        TRY();
            tbb::parallel_for((size_t)0, (size_t)PFOR_BUFFER_TEST_SIZE, test_functor_with_exception());
        CATCH_AND_ASSERT();
    }
}
#endif /* TBB_USE_EXCEPTIONS */

// Cancellation support test
class functor_to_cancel {
public:
    void operator()(size_t) const {
        ++g_CurExecuted;
        CancellatorTask::WaitUntilReady();
    }
};

size_t g_worker_task_step = 0;

class my_worker_pfor_step_task : public tbb::task
{
    tbb::task_group_context &my_ctx;

    tbb::task* execute () {
        if (g_worker_task_step == 0){
            tbb::parallel_for((size_t)0, (size_t)PFOR_BUFFER_TEST_SIZE, functor_to_cancel(), my_ctx);
        }else{
            tbb::parallel_for((size_t)0, (size_t)PFOR_BUFFER_TEST_SIZE, g_worker_task_step, functor_to_cancel(), my_ctx);
        }
        return NULL;
    }
public:
    my_worker_pfor_step_task ( tbb::task_group_context &context_) : my_ctx(context_) { }
};

void TestCancellation()
{
    // tests version without a step
    g_worker_task_step = 0;
    ResetEhGlobals();
    RunCancellationTest<my_worker_pfor_step_task, CancellatorTask>();

    // tests version with step
    g_worker_task_step = 1;
    ResetEhGlobals();
    RunCancellationTest<my_worker_pfor_step_task, CancellatorTask>();
}

#include "harness_m128.h"

#if HAVE_m128 && !__TBB_SSE_STACK_ALIGNMENT_BROKEN
struct SSE_Functor {
    ClassWithSSE* Src, * Dst;
    SSE_Functor( ClassWithSSE* src, ClassWithSSE* dst ) : Src(src), Dst(dst) {}

    void operator()( tbb::blocked_range<int>& r ) const {
        for( int i=r.begin(); i!=r.end(); ++i )
            Dst[i] = Src[i];
    }     
};

//! Test that parallel_for works with stack-allocated __m128
void TestSSE() {
    ClassWithSSE Array1[N], Array2[N];
    for( int i=0; i<N; ++i )
        Array1[i] = ClassWithSSE(i);
    tbb::parallel_for( tbb::blocked_range<int>(0,N), SSE_Functor(Array1, Array2) );
    for( int i=0; i<N; ++i )
        ASSERT( Array2[i]==ClassWithSSE(i), NULL ) ;
}
#endif /* HAVE_m128 */

#include <cstdio>
#include "tbb/task_scheduler_init.h"
#include "harness_cpu.h"

int TestMain () {
    if( MinThread<1 ) {
        REPORT("number of threads must be positive\n");
        exit(1);
    }
    for( int p=MinThread; p<=MaxThread; ++p ) {
        if( p>0 ) {
            tbb::task_scheduler_init init( p );
            Flog<1>(p);
            Flog<10>(p);
            Flog<100>(p);
            Flog<1000>(p);
            Flog<10000>(p);

            // Testing with different integer types
            TestParallelForWithStepSupport<short>();
            TestParallelForWithStepSupport<unsigned short>();
            TestParallelForWithStepSupport<int>();
            TestParallelForWithStepSupport<unsigned int>();
            TestParallelForWithStepSupport<long>();
            TestParallelForWithStepSupport<unsigned long>();
            TestParallelForWithStepSupport<long long>();
            TestParallelForWithStepSupport<unsigned long long>();
            TestParallelForWithStepSupport<size_t>();
#if TBB_USE_EXCEPTIONS && !__TBB_THROW_ACROSS_MODULE_BOUNDARY_BROKEN
            TestExceptionsSupport();
#endif /* TBB_USE_EXCEPTIONS && !__TBB_THROW_ACROSS_MODULE_BOUNDARY_BROKEN */
            if (p>1) TestCancellation();
#if HAVE_m128 && !__TBB_SSE_STACK_ALIGNMENT_BROKEN
            TestSSE();
#endif /* HAVE_m128 */

            // Test that all workers sleep when no work
            TestCPUUserTime(p);
        }
    }
#if __TBB_THROW_ACROSS_MODULE_BOUNDARY_BROKEN
    REPORT("Known issue: exception handling tests are skipped.\n");
#endif
#if HAVE_m128 && __TBB_SSE_STACK_ALIGNMENT_BROKEN
    REPORT("Known issue: stack alignment for SSE not tested.\n");
#endif
    return Harness::Done;
}

#if _MSC_VER
#pragma warning (pop)
#endif
