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

#include "tbb/parallel_reduce.h"
#include "tbb/atomic.h"
#include "harness_assert.h"

using namespace std;

static tbb::atomic<long> ForkCount;
static tbb::atomic<long> FooBodyCount;

//! Class with public interface that is exactly minimal requirements for Range concept
class MinimalRange {
    size_t begin, end;
    friend class FooBody;
    explicit MinimalRange( size_t i ) : begin(0), end(i) {}
    friend void Flog( int nthread, bool inteference );
public:
    MinimalRange( MinimalRange& r, tbb::split ) : end(r.end) {
        begin = r.end = (r.begin+r.end)/2;
    }
    bool is_divisible() const {return end-begin>=2;}
    bool empty() const {return begin==end;}
};

//! Class with public interface that is exactly minimal requirements for Body of a parallel_reduce
class FooBody {
private:
    FooBody( const FooBody& );          // Deny access
    void operator=( const FooBody& );   // Deny access
    friend void Flog( int nthread, bool interference );
    //! Parent that created this body via split operation.  NULL if original body.
    FooBody* parent;
    //! Total number of index values processed by body and its children.
    size_t sum;
    //! Number of join operations done so far on this body and its children.
    long join_count;
    //! Range that has been processed so far by this body and its children.
    size_t begin, end;
    //! True if body has not yet been processed at least once by operator().
    bool is_new;
    //! 1 if body was created by split; 0 if original body.
    int forked;
    FooBody() {++FooBodyCount;}
public:
    ~FooBody() {
        forked = 0xDEADBEEF; 
        sum=0xDEADBEEF;
        join_count=0xDEADBEEF;
        --FooBodyCount;
    } 
    FooBody( FooBody& other, tbb::split ) {
        ++FooBodyCount;
        ++ForkCount;
        sum = 0;
        parent = &other;
        join_count = 0;
        is_new = true;
        forked = 1;
    }
    void join( FooBody& s ) {
        ASSERT( s.forked==1, NULL );
        ASSERT( this!=&s, NULL );
        ASSERT( this==s.parent, NULL ); 
        ASSERT( end==s.begin, NULL );
        end = s.end;
        sum += s.sum;
        join_count += s.join_count + 1;
        s.forked = 2;
    }
    void operator()( const MinimalRange& r ) {
        for( size_t k=r.begin; k<r.end; ++k )
            ++sum;
        if( is_new ) {
            is_new = false;
            begin = r.begin;
        } else
            ASSERT( end==r.begin, NULL );
        end = r.end;
    }
};

#include <cstdio>
#include "harness.h"
#include "tbb/tick_count.h"

void Flog( int nthread, bool interference=false ) {
    for (int mode = 0;  mode < 4; mode++) {
        tbb::tick_count T0 = tbb::tick_count::now();
        long join_count = 0;        
        tbb::affinity_partitioner ap;
        for( size_t i=0; i<=1000; ++i ) {
            FooBody f;
            f.sum = 0;
            f.parent = NULL;
            f.join_count = 0;
            f.is_new = true;
            f.forked = 0;
            f.begin = ~size_t(0);
            f.end = ~size_t(0);
            ASSERT( FooBodyCount==1, NULL );
            switch (mode) {
                case 0:
                    tbb::parallel_reduce( MinimalRange(i), f );
                    break;
                case 1:
                    tbb::parallel_reduce( MinimalRange(i), f, tbb::simple_partitioner() );
                    break;
                case 2:
                    tbb::parallel_reduce( MinimalRange(i), f, tbb::auto_partitioner() );
                    break;
                case 3: 
                    tbb::parallel_reduce( MinimalRange(i), f, ap );
                    break;
            }
            join_count += f.join_count;
            ASSERT( FooBodyCount==1, NULL );
            ASSERT( f.sum==i, NULL );
            ASSERT( f.begin==(i==0 ? ~size_t(0) : 0), NULL );
            ASSERT( f.end==(i==0 ? ~size_t(0) : i), NULL );
        }
        tbb::tick_count T1 = tbb::tick_count::now();
        REMARK("time=%g join_count=%ld ForkCount=%ld nthread=%d%s\n",
                   (T1-T0).seconds(),join_count,long(ForkCount), nthread, interference ? " with interference)":"");
    }
}

class DeepThief: public tbb::task {
    /*override*/tbb::task* execute() {
        if( !is_stolen_task() )
            spawn(*child);
        wait_for_all();
        return NULL;
    }
    task* child;
    friend void FlogWithInterference(int);
public:
    DeepThief() : child() {}
};

//! Test for problem in TBB 2.1 parallel_reduce where middle of a range is stolen.
/** Warning: this test is a somewhat abusive use of TBB somewhat because 
    it requires two or more threads to avoid deadlock. */
void FlogWithInterference( int nthread ) {
    ASSERT( nthread>=2, "requires too or more threads" );

    // Build linear chain of tasks. 
    // The purpose is to drive up "task depth" in TBB 2.1.
    // An alternative would be to use add_to_depth, but that method is deprecated in TBB 2.2,
    // and this way we generalize to catching problems with implicit depth calculations.
    tbb::task* root = new( tbb::task::allocate_root() ) tbb::empty_task;
    root->set_ref_count(2);
    tbb::task* t = root;
    for( int i=0; i<3; ++i ) {
        t = new( t->allocate_child() ) tbb::empty_task;
        t->set_ref_count(1);
    }

    // Append a DeepThief to the chain.
    DeepThief* deep_thief = new( t->allocate_child() ) DeepThief;
    deep_thief->set_ref_count(2);

    // Append a leaf to the chain. 
    tbb::task* leaf = new( deep_thief->allocate_child() ) tbb::empty_task;
    deep_thief->child = leaf;

    root->spawn(*deep_thief);

    Flog(nthread,true);

    if( root->ref_count()==2 ) {
        // Spawn leaf, which when it finishes, cause the DeepThief and rest of the chain to finish.
        root->spawn( *leaf );
    }
    // Wait for all tasks in the chain from root to leaf to finish.
    root->wait_for_all();
    root->destroy( *root );
}

#include "tbb/blocked_range.h"

#if _MSC_VER
    typedef tbb::internal::uint64_t ValueType;
#else
    typedef uint64_t ValueType;
#endif

struct Sum {
    template<typename T>
    T operator() ( const T& v1, const T& v2 ) const {
        return v1 + v2;
    }
};

struct Accumulator {
    ValueType operator() ( const tbb::blocked_range<ValueType*>& r, ValueType value ) const {
        for ( ValueType* pv = r.begin(); pv != r.end(); ++pv )
            value += *pv;
        return value;
    }
};

void ParallelSum () {
    const ValueType I = 0,
                    N = 1000000,
                    R = N * (N + 1) / 2;
    ValueType *array = new ValueType[N + 1];
    for ( ValueType i = 0; i < N; ++i )
        array[i] = i + 1;
    tbb::blocked_range<ValueType*> range(array, array + N);
    ValueType r1 = tbb::parallel_reduce( range, I, Accumulator(), Sum() );
    ASSERT( r1 == R, NULL );
#if __TBB_LAMBDAS_PRESENT
    ValueType r2 = tbb::parallel_reduce( range, I, 
        [](const tbb::blocked_range<ValueType*>& r, ValueType value) -> ValueType { 
            for ( ValueType* pv = r.begin(); pv != r.end(); ++pv )
                value += *pv;
            return value;
        },
        Sum()
    );
    ASSERT( r2 == R, NULL );
#endif /* LAMBDAS */
    delete array;
}

#include "tbb/task_scheduler_init.h"
#include "harness_cpu.h"

int TestMain () {
    if( MinThread<0 ) {
        REPORT("Usage: nthread must be positive\n");
        exit(1);
    }
    for( int p=MinThread; p<=MaxThread; ++p ) {
        tbb::task_scheduler_init init( p );
        Flog(p);
        if( p>=2 )
            FlogWithInterference(p);
        ParallelSum();
        // Test that all workers sleep when no work
        TestCPUUserTime(p);
    }
    return Harness::Done;
}
