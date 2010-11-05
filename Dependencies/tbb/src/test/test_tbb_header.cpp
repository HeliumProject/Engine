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

/**
    This test ensures that tbb.h brings in all the public TBB interface definitions,
    and if all the necessary symbols are exported from the library.

    Most of the checks happen at the compilation or link phases.
**/

#include "tbb/tbb.h"

static volatile size_t g_sink;

#define TestTypeDefinitionPresence( Type ) g_sink = sizeof(tbb::Type);
#define TestTypeDefinitionPresence2(TypeStart, TypeEnd) g_sink = sizeof(tbb::TypeStart,TypeEnd);
#define TestFuncDefinitionPresence(Fn, Args, ReturnType) { ReturnType (*pfn)Args = &tbb::Fn; (void)pfn; }

struct Body {
    void operator() () const {}
};
struct Body1 {
    void operator() ( int ) const {}
};
struct Body1a {
    int operator() ( const tbb::blocked_range<int>&, const int ) const { return 0; }
};
struct Body1b {
    int operator() ( const int, const int ) const { return 0; }
};
struct Body2 {
    Body2 () {}
    Body2 ( const Body2&, tbb::split ) {}
    void operator() ( const tbb::blocked_range<int>& ) const {}
    void join( const Body2& ) {}
};
struct Body3 {
    Body3 () {}
    Body3 ( const Body3&, tbb::split ) {}
    void operator() ( const tbb::blocked_range2d<int>&, tbb::pre_scan_tag ) const {}
    void operator() ( const tbb::blocked_range2d<int>&, tbb::final_scan_tag ) const {}
    void reverse_join( Body3& ) {}
    void assign( const Body3& ) {}
};

#if !__TBB_TEST_SECONDARY

#define HARNESS_NO_PARSE_COMMAND_LINE 1
#include "harness.h"

// Test if all the necessary symbols are exported for the exceptions thrown by TBB.
// Missing exports result either in link error or in runtime assertion failure.
#include "tbb/tbb_exception.h"

template <typename E>
void TestExceptionClassExports ( const E& exc, tbb::internal::exception_id eid ) {
    // The assertion here serves to shut up warnings about "eid not used". 
    ASSERT( eid<tbb::internal::eid_max, NULL );
#if TBB_USE_EXCEPTIONS
    for ( int i = 0; i < 2; ++i ) {
        try {
            if ( i == 0 )
                throw exc;
#if !__TBB_THROW_ACROSS_MODULE_BOUNDARY_BROKEN
            else
                tbb::internal::throw_exception( eid );
#endif
        }
        catch ( E& e ) {
            ASSERT ( e.what(), "Missing what() string" );
        }
        catch ( ... ) {
            ASSERT ( __TBB_EXCEPTION_TYPE_INFO_BROKEN, "Unrecognized exception. Likely RTTI related exports are missing" );
        }
    }
#else /* !TBB_USE_EXCEPTIONS */
    (void)exc;
#endif /* !TBB_USE_EXCEPTIONS */
}

void TestExceptionClassesExports () {
    TestExceptionClassExports( std::bad_alloc(), tbb::internal::eid_bad_alloc );
    TestExceptionClassExports( tbb::bad_last_alloc(), tbb::internal::eid_bad_last_alloc );
    TestExceptionClassExports( std::invalid_argument("test"), tbb::internal::eid_nonpositive_step );
    TestExceptionClassExports( std::out_of_range("test"), tbb::internal::eid_out_of_range );
    TestExceptionClassExports( std::range_error("test"), tbb::internal::eid_segment_range_error );
    TestExceptionClassExports( std::range_error("test"), tbb::internal::eid_index_range_error );
    TestExceptionClassExports( tbb::missing_wait(), tbb::internal::eid_missing_wait );
    TestExceptionClassExports( tbb::invalid_multiple_scheduling(), tbb::internal::eid_invalid_multiple_scheduling );
    TestExceptionClassExports( tbb::improper_lock(), tbb::internal::eid_improper_lock );
}
#endif /* !__TBB_TEST_SECONDARY */


#if __TBB_TEST_SECONDARY
/* This mode is used to produce a secondary object file that is linked with 
   the main one in order to detect "multiple definition" linker error.
*/
void secondary()
#else
int TestMain ()
#endif
{
    TestTypeDefinitionPresence2(aligned_space<int, 1> );
    TestTypeDefinitionPresence( atomic<int> );
    TestTypeDefinitionPresence( cache_aligned_allocator<int> );
    TestTypeDefinitionPresence( tbb_hash_compare<int> );
    TestTypeDefinitionPresence2(concurrent_hash_map<int, int> );
    TestTypeDefinitionPresence2(concurrent_unordered_map<int, int> );
    TestTypeDefinitionPresence( concurrent_bounded_queue<int> );
    TestTypeDefinitionPresence( deprecated::concurrent_queue<int> );
    TestTypeDefinitionPresence( strict_ppl::concurrent_queue<int> );
    TestTypeDefinitionPresence( combinable<int> );
    TestTypeDefinitionPresence( concurrent_vector<int> );
    TestTypeDefinitionPresence( enumerable_thread_specific<int> );
    TestTypeDefinitionPresence( mutex );
    TestTypeDefinitionPresence( null_mutex );
    TestTypeDefinitionPresence( null_rw_mutex );
    TestTypeDefinitionPresence( queuing_mutex );
    TestTypeDefinitionPresence( queuing_rw_mutex );
    TestTypeDefinitionPresence( recursive_mutex );
    TestTypeDefinitionPresence( spin_mutex );
    TestTypeDefinitionPresence( spin_rw_mutex );
    TestTypeDefinitionPresence( critical_section );
    TestTypeDefinitionPresence( reader_writer_lock );
    TestTypeDefinitionPresence( tbb_exception );
    TestTypeDefinitionPresence( captured_exception );
    TestTypeDefinitionPresence( movable_exception<int> );
#if !TBB_USE_CAPTURED_EXCEPTION
    TestTypeDefinitionPresence( internal::tbb_exception_ptr );
#endif /* !TBB_USE_CAPTURED_EXCEPTION */
    TestTypeDefinitionPresence( blocked_range3d<int> );
    TestFuncDefinitionPresence( parallel_invoke, (const Body&, const Body&), void );
    TestFuncDefinitionPresence( parallel_do, (int*, int*, const Body1&), void );
    TestFuncDefinitionPresence( parallel_for_each, (int*, int*, const Body1&), void );
    TestFuncDefinitionPresence( parallel_for, (int, int, int, const Body1&), void );
    TestFuncDefinitionPresence( parallel_for, (const tbb::blocked_range<int>&, const Body2&, const tbb::simple_partitioner&), void );
    TestFuncDefinitionPresence( parallel_reduce, (const tbb::blocked_range<int>&, const int&, const Body1a&, const Body1b&, const tbb::auto_partitioner&), int );
    TestFuncDefinitionPresence( parallel_reduce, (const tbb::blocked_range<int>&, Body2&, tbb::affinity_partitioner&), void );
    TestFuncDefinitionPresence( parallel_scan, (const tbb::blocked_range2d<int>&, Body3&, const tbb::auto_partitioner&), void );
    TestFuncDefinitionPresence( parallel_sort, (int*, int*), void );
    TestTypeDefinitionPresence( pipeline );
    TestFuncDefinitionPresence( parallel_pipeline, (size_t, const tbb::filter_t<void,void>&), void );
    TestTypeDefinitionPresence( task );
    TestTypeDefinitionPresence( empty_task );
    TestTypeDefinitionPresence( task_list );
    TestTypeDefinitionPresence( task_group_context );
    TestTypeDefinitionPresence( task_group );
    TestTypeDefinitionPresence( task_handle<Body> );
    TestTypeDefinitionPresence( task_scheduler_init );
    TestTypeDefinitionPresence( task_scheduler_observer );
    TestTypeDefinitionPresence( tbb_thread );
    TestTypeDefinitionPresence( tbb_allocator<int> );
    TestTypeDefinitionPresence( zero_allocator<int> );
    TestTypeDefinitionPresence( tick_count );
#if !__TBB_TEST_SECONDARY
    TestExceptionClassesExports();
    return Harness::Done;
#endif
}
