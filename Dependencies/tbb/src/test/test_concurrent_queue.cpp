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

#include "tbb/concurrent_queue.h"
#include "tbb/atomic.h"
#include "tbb/tick_count.h"
#include "harness.h"
#include "harness_allocator.h"

static tbb::atomic<long> FooConstructed;
static tbb::atomic<long> FooDestroyed;

class Foo {
    enum state_t{
        LIVE=0x1234,
        DEAD=0xDEAD
    };
    state_t state;
public:
    int thread_id;
    int serial;
    Foo() : state(LIVE), thread_id(0), serial(0) {
        ++FooConstructed;
    }
    Foo( const Foo& item ) : state(LIVE) {
        ASSERT( item.state==LIVE, NULL );
        ++FooConstructed;
        thread_id = item.thread_id;
        serial = item.serial;
    }
    ~Foo() {
        ASSERT( state==LIVE, NULL );
        ++FooDestroyed;
        state=DEAD;
        thread_id=0xDEAD;
        serial=0xDEAD;
    }
    void operator=( const Foo& item ) {
        ASSERT( item.state==LIVE, NULL );
        ASSERT( state==LIVE, NULL );
        thread_id = item.thread_id;
        serial = item.serial;
    }
    bool is_const() {return false;}
    bool is_const() const {return true;}
};

// problem size
static const int N = 50000;     // # of bytes

#if TBB_USE_EXCEPTIONS
//! Exception for concurrent_queue
class Foo_exception : public std::bad_alloc {
public:
    virtual const char *what() const throw() { return "out of Foo limit"; }
    virtual ~Foo_exception() throw() {}
};

static tbb::atomic<long> FooExConstructed;
static tbb::atomic<long> FooExDestroyed;
static tbb::atomic<long> serial_source;
static long MaxFooCount = 0;
static const long Threshold = 400;

class FooEx {
    enum state_t{
        LIVE=0x1234,
        DEAD=0xDEAD
    };
    state_t state;
public:
    int serial;
    FooEx() : state(LIVE) {
        ++FooExConstructed;
        serial = serial_source++;
    }

    FooEx( const FooEx& item ) : state(LIVE) {
        ++FooExConstructed;
        if( MaxFooCount && (FooExConstructed-FooExDestroyed) >= MaxFooCount ) // in push()
            throw Foo_exception();
        serial = item.serial;
    }
    ~FooEx() {
        ASSERT( state==LIVE, NULL );
        ++FooExDestroyed;
        state=DEAD;
        serial=0xDEAD;
    }
    void operator=( FooEx& item ) {
        ASSERT( item.state==LIVE, NULL );
        ASSERT( state==LIVE, NULL );
        serial = item.serial;
        if( MaxFooCount==2*Threshold && (FooExConstructed-FooExDestroyed) <= MaxFooCount/4 ) // in pop()
            throw Foo_exception();
    }
} ;
#endif /* TBB_USE_EXCEPTIONS */

const size_t MAXTHREAD = 256;

static int Sum[MAXTHREAD];

//! Count of various pop operations
/** [0] = pop_if_present that failed
    [1] = pop_if_present that succeeded
    [2] = pop */
static tbb::atomic<long> PopKind[3];

const int M = 10000;

#if TBB_DEPRECATED
#define CALL_BLOCKING_POP(q,v) (q)->pop(v)
#define CALL_TRY_POP(q,v,i) (((i)&0x2)?q->try_pop(v):q->pop_if_present(v))
#define SIZE() size()
#else
#define CALL_BLOCKING_POP(q,v) while( !(q)->try_pop(v) ) __TBB_Yield()
#define CALL_TRY_POP(q,v,i) q->try_pop(v)
#define SIZE() unsafe_size()
#endif

struct Body: NoAssign {
    tbb::concurrent_queue<Foo>* queue;
    const int nthread;
    Body( int nthread_ ) : nthread(nthread_) {}
    void operator()( int thread_id ) const {
        long pop_kind[3] = {0,0,0};
        int serial[MAXTHREAD+1];
        memset( serial, 0, nthread*sizeof(int) );
        ASSERT( thread_id<nthread, NULL );

        long sum = 0;
        for( long j=0; j<M; ++j ) {
            Foo f;
            f.thread_id = 0xDEAD;
            f.serial = 0xDEAD;
            bool prepopped = false;
            if( j&1 ) {
                prepopped = CALL_TRY_POP(queue,f,j);
                ++pop_kind[prepopped];
            }
            Foo g;
            g.thread_id = thread_id;
            g.serial = j+1;
            queue->push( g );
            if( !prepopped ) {
                CALL_BLOCKING_POP(queue,f);
                ++pop_kind[2];
            }
            ASSERT( f.thread_id<=nthread, NULL );
            ASSERT( f.thread_id==nthread || serial[f.thread_id]<f.serial, "partial order violation" );
            serial[f.thread_id] = f.serial;
            sum += f.serial-1;
        }
        Sum[thread_id] = sum;
        for( int k=0; k<3; ++k )
            PopKind[k] += pop_kind[k];
    }
};

void TestPushPop( size_t prefill, ptrdiff_t capacity, int nthread ) {
    ASSERT( nthread>0, "nthread must be positive" );
#if TBB_DEPRECATED
    ptrdiff_t signed_prefill = ptrdiff_t(prefill);
    if( signed_prefill+1>=capacity )
        return;
#endif
    bool success = false;
    for( int k=0; k<3; ++k )
        PopKind[k] = 0;
    for( int trial=0; !success; ++trial ) {
        FooConstructed = 0;
        FooDestroyed = 0;
        Body body(nthread);
        tbb::concurrent_queue<Foo> queue;
#if TBB_DEPRECATED
        queue.set_capacity( capacity );
#endif
        body.queue = &queue;
        for( size_t i=0; i<prefill; ++i ) {
            Foo f;
            f.thread_id = nthread;
            f.serial = 1+int(i);
            queue.push(f);
            ASSERT( unsigned(queue.SIZE())==i+1, NULL );
            ASSERT( !queue.empty(), NULL );
        }
        tbb::tick_count t0 = tbb::tick_count::now();
        NativeParallelFor( nthread, body );
        tbb::tick_count t1 = tbb::tick_count::now();
        double timing = (t1-t0).seconds();
        REMARK("prefill=%d capacity=%d threads=%d time = %g = %g nsec/operation\n", int(prefill), int(capacity), nthread, timing, timing/(2*M*nthread)*1.E9);
        int sum = 0;
        for( int k=0; k<nthread; ++k )
            sum += Sum[k];
        int expected = int(nthread*((M-1)*M/2) + ((prefill-1)*prefill)/2);
        for( int i=int(prefill); --i>=0; ) {
            ASSERT( !queue.empty(), NULL );
            Foo f;
            bool result = queue.try_pop(f);
            ASSERT( result, NULL );
            ASSERT( int(queue.SIZE())==i, NULL );
            sum += f.serial-1;
        }
        ASSERT( queue.empty(), NULL );
        ASSERT( queue.SIZE()==0, NULL );
        if( sum!=expected )
            REPORT("sum=%d expected=%d\n",sum,expected);
        ASSERT( FooConstructed==FooDestroyed, NULL );
        // TODO: checks by counting allocators

        success = true;
        if( nthread>1 && prefill==0 ) {
            // Check that pop_if_present got sufficient exercise
            for( int k=0; k<2; ++k ) {
#if (_WIN32||_WIN64)
                // The TBB library on Windows seems to have a tough time generating
                // the desired interleavings for pop_if_present, so the code tries longer, and settles
                // for fewer desired interleavings.
                const int max_trial = 100;
                const int min_requirement = 20;
#else
                const int min_requirement = 100;
                const int max_trial = 20;
#endif /* _WIN32||_WIN64 */
                if( PopKind[k]<min_requirement ) {
                    if( trial>=max_trial ) {
                        if( Verbose )
                            REPORT("Warning: %d threads had only %ld pop_if_present operations %s after %d trials (expected at least %d). "
                               "This problem may merely be unlucky scheduling. "
                               "Investigate only if it happens repeatedly.\n",
                               nthread, long(PopKind[k]), k==0?"failed":"succeeded", max_trial, min_requirement);
                        else
                            REPORT("Warning: the number of %s pop_if_present operations is less than expected for %d threads. Investigate if it happens repeatedly.\n",
                               k==0?"failed":"succeeded", nthread ); 

                    } else {
                        success = false;
                    }
               }
            }
        }
    }
}

class Bar {
    enum state_t {
        LIVE=0x1234,
        DEAD=0xDEAD
    };
    state_t state;
public:
    ptrdiff_t my_id;
    Bar() : state(LIVE), my_id(-1) {}
    Bar(size_t _i) : state(LIVE), my_id(_i) {}
    Bar( const Bar& a_bar ) : state(LIVE) {
        ASSERT( a_bar.state==LIVE, NULL );
        my_id = a_bar.my_id;
    }
    ~Bar() {
        ASSERT( state==LIVE, NULL );
        state = DEAD;
        my_id = DEAD;
    }
    void operator=( const Bar& a_bar ) {
        ASSERT( a_bar.state==LIVE, NULL );
        ASSERT( state==LIVE, NULL );
        my_id = a_bar.my_id;
    }
    friend bool operator==(const Bar& bar1, const Bar& bar2 ) ;
} ;

bool operator==(const Bar& bar1, const Bar& bar2) {
    ASSERT( bar1.state==Bar::LIVE, NULL );
    ASSERT( bar2.state==Bar::LIVE, NULL );
    return bar1.my_id == bar2.my_id;
}

class BarIterator
{
    Bar* bar_ptr;
    BarIterator(Bar* bp_) : bar_ptr(bp_) {}
public:
    ~BarIterator() {}
    BarIterator& operator=( const BarIterator& other ) {
        bar_ptr = other.bar_ptr;
        return *this;
    }
    Bar& operator*() const {
        return *bar_ptr;
    }
    BarIterator& operator++() {
        ++bar_ptr;
        return *this;
    }
    Bar* operator++(int) {
        Bar* result = &operator*();
        operator++();
        return result;
    }
    friend bool operator==(const BarIterator& bia, const BarIterator& bib) ;
    friend bool operator!=(const BarIterator& bia, const BarIterator& bib) ;
    friend void TestConstructors ();
} ;

bool operator==(const BarIterator& bia, const BarIterator& bib) {
    return bia.bar_ptr==bib.bar_ptr;
}

bool operator!=(const BarIterator& bia, const BarIterator& bib) {
    return bia.bar_ptr!=bib.bar_ptr;
}

#if TBB_USE_EXCEPTIONS
class Bar_exception : public std::bad_alloc {
public:
    virtual const char *what() const throw() { return "making the entry invalid"; }
    virtual ~Bar_exception() throw() {}
};

class BarEx {
    enum state_t {
        LIVE=0x1234,
        DEAD=0xDEAD
    };
    static int count;
public:
    state_t state;
    typedef enum {
        PREPARATION,
        COPY_CONSTRUCT
    } mode_t;
    static mode_t mode;
    ptrdiff_t my_id;
    ptrdiff_t my_tilda_id;
    static int button;
    BarEx() : state(LIVE), my_id(-1), my_tilda_id(-1) {}
    BarEx(size_t _i) : state(LIVE), my_id(_i), my_tilda_id(my_id^(-1)) {}
    BarEx( const BarEx& a_bar ) : state(LIVE) {
        ASSERT( a_bar.state==LIVE, NULL );
        my_id = a_bar.my_id;
        if( mode==PREPARATION ) 
            if( !( ++count % 100 ) ) 
                throw Bar_exception();
        my_tilda_id = a_bar.my_tilda_id;
    }
    ~BarEx() {
        ASSERT( state==LIVE, NULL );
        state = DEAD;
        my_id = DEAD;
    }
    static void set_mode( mode_t m ) { mode = m; }
    void operator=( const BarEx& a_bar ) {
        ASSERT( a_bar.state==LIVE, NULL );
        ASSERT( state==LIVE, NULL );
        my_id = a_bar.my_id;
        my_tilda_id = a_bar.my_tilda_id;
    }
    friend bool operator==(const BarEx& bar1, const BarEx& bar2 ) ;
} ;

int    BarEx::count = 0;
BarEx::mode_t BarEx::mode = BarEx::PREPARATION;

bool operator==(const BarEx& bar1, const BarEx& bar2) {
    ASSERT( bar1.state==BarEx::LIVE, NULL );
    ASSERT( bar2.state==BarEx::LIVE, NULL );
    ASSERT( (bar1.my_id ^ bar1.my_tilda_id) == -1, NULL );
    ASSERT( (bar2.my_id ^ bar2.my_tilda_id) == -1, NULL );
    return bar1.my_id==bar2.my_id && bar1.my_tilda_id==bar2.my_tilda_id;
}
#endif /* TBB_USE_EXCEPTIONS */

#if TBB_DEPRECATED
#define CALL_BEGIN(q,i) (((i)&0x1)?q.begin():q.unsafe_begin())
#define CALL_END(q,i)   (((i)&0x1)?q.end():q.unsafe_end())
#else
#define CALL_BEGIN(q,i) q.unsafe_begin()
#define CALL_END(q,i)   q.unsafe_end()
#endif

void TestConstructors ()
{
    tbb::concurrent_queue<Bar> src_queue;
    tbb::concurrent_queue<Bar>::const_iterator dqb;
    tbb::concurrent_queue<Bar>::const_iterator dqe;
    tbb::concurrent_queue<Bar>::const_iterator iter;

    for( size_t size=0; size<1001; ++size ) {
        for( size_t i=0; i<size; ++i )
            src_queue.push(Bar(i+(i^size)));
        tbb::concurrent_queue<Bar>::const_iterator sqb( CALL_BEGIN(src_queue,size) );
        tbb::concurrent_queue<Bar>::const_iterator sqe( CALL_END(src_queue,size));

        tbb::concurrent_queue<Bar> dst_queue(sqb, sqe);

        ASSERT(src_queue.SIZE()==dst_queue.SIZE(), "different size");

        src_queue.clear();
    }

    Bar bar_array[1001];
    for( size_t size=0; size<1001; ++size ) {
        for( size_t i=0; i<size; ++i )
            bar_array[i] = Bar(i+(i^size));

        const BarIterator sab(bar_array+0);
        const BarIterator sae(bar_array+size);

        tbb::concurrent_queue<Bar> dst_queue2(sab, sae);

        ASSERT( size==unsigned(dst_queue2.SIZE()), NULL );
        ASSERT( sab==BarIterator(bar_array+0), NULL );
        ASSERT( sae==BarIterator(bar_array+size), NULL );

        dqb = CALL_BEGIN(dst_queue2,size);
        dqe = CALL_END(dst_queue2,size);
        BarIterator v_iter(sab);
        for( ; dqb != dqe; ++dqb, ++v_iter )
            ASSERT( *dqb == *v_iter, "unexpected element" );
        ASSERT( v_iter==sae, "different size?" );
    }

    src_queue.clear();

    tbb::concurrent_queue<Bar> dst_queue3( src_queue );
    ASSERT( src_queue.SIZE()==dst_queue3.SIZE(), NULL );
    ASSERT( 0==dst_queue3.SIZE(), NULL );

    int k=0;
    for( size_t i=0; i<1001; ++i ) {
        Bar tmp_bar;
        src_queue.push(Bar(++k));
        src_queue.push(Bar(++k));
        src_queue.try_pop(tmp_bar);

        tbb::concurrent_queue<Bar> dst_queue4( src_queue );

        ASSERT( src_queue.SIZE()==dst_queue4.SIZE(), NULL );

        dqb = CALL_BEGIN(dst_queue4,i);
        dqe = CALL_END(dst_queue4,i);
        iter = CALL_BEGIN(src_queue,i);

        for( ; dqb != dqe; ++dqb, ++iter )
            ASSERT( *dqb == *iter, "unexpected element" );

        ASSERT( iter==CALL_END(src_queue,i), "different size?" );
    }

    tbb::concurrent_queue<Bar> dst_queue5( src_queue );

    ASSERT( src_queue.SIZE()==dst_queue5.SIZE(), NULL );
    dqb = dst_queue5.unsafe_begin();
    dqe = dst_queue5.unsafe_end();
    iter = src_queue.unsafe_begin();
    for( ; dqb != dqe; ++dqb, ++iter )
        ASSERT( *dqb == *iter, "unexpected element" );

    for( size_t i=0; i<100; ++i) {
        Bar tmp_bar;
        src_queue.push(Bar(i+1000));
        src_queue.push(Bar(i+1000));
        src_queue.try_pop(tmp_bar);

        dst_queue5.push(Bar(i+1000));
        dst_queue5.push(Bar(i+1000));
        dst_queue5.try_pop(tmp_bar);
    }

    ASSERT( src_queue.SIZE()==dst_queue5.SIZE(), NULL );
    dqb = dst_queue5.unsafe_begin();
    dqe = dst_queue5.unsafe_end();
    iter = src_queue.unsafe_begin();
    for( ; dqb != dqe; ++dqb, ++iter )
        ASSERT( *dqb == *iter, "unexpected element" );
    ASSERT( iter==src_queue.unsafe_end(), "different size?" );

#if __TBB_THROW_ACROSS_MODULE_BOUNDARY_BROKEN || __TBB_PLACEMENT_NEW_EXCEPTION_SAFETY_BROKEN
    REPORT("Known issue: part of the constructor test is skipped.\n");
#elif TBB_USE_EXCEPTIONS
    k = 0;
#if TBB_DEPRECATED==0
    unsigned
#endif
    int n_elements=0;
    tbb::concurrent_queue<BarEx> src_queue_ex;
    for( size_t size=0; size<1001; ++size ) {
        BarEx tmp_bar_ex;
        int n_successful_pushes=0;
        BarEx::set_mode( BarEx::PREPARATION );
        try {
            src_queue_ex.push(BarEx(k+(k^size)));
            ++n_successful_pushes;
        } catch (...) {
        }
        ++k;
        try {
            src_queue_ex.push(BarEx(k+(k^size)));
            ++n_successful_pushes;
        } catch (...) {
        }
        ++k;
        src_queue_ex.try_pop(tmp_bar_ex);
        n_elements += (n_successful_pushes - 1);
        ASSERT( src_queue_ex.SIZE()==n_elements, NULL);

        BarEx::set_mode( BarEx::COPY_CONSTRUCT );
        tbb::concurrent_queue<BarEx> dst_queue_ex( src_queue_ex );

        ASSERT( src_queue_ex.SIZE()==dst_queue_ex.SIZE(), NULL );

        tbb::concurrent_queue<BarEx>::const_iterator dqb_ex  = CALL_BEGIN(dst_queue_ex, size);
        tbb::concurrent_queue<BarEx>::const_iterator dqe_ex  = CALL_END(dst_queue_ex, size);
        tbb::concurrent_queue<BarEx>::const_iterator iter_ex = CALL_BEGIN(src_queue_ex, size);

        for( ; dqb_ex != dqe_ex; ++dqb_ex, ++iter_ex )
            ASSERT( *dqb_ex == *iter_ex, "unexpected element" );
        ASSERT( iter_ex==CALL_END(src_queue_ex,size), "different size?" );
    }
#endif /* TBB_USE_EXCEPTIONS */
}

template<typename Iterator1, typename Iterator2>
void TestIteratorAux( Iterator1 i, Iterator2 j, int size ) {
    // Now test iteration
    Iterator1 old_i;
    for( int k=0; k<size; ++k ) {
        ASSERT( i!=j, NULL );
        ASSERT( !(i==j), NULL );
        Foo f;
        if( k&1 ) {
            // Test pre-increment
            f = *old_i++;
            // Test assignment
            i = old_i;
        } else {
            // Test post-increment
            f=*i++;
            if( k<size-1 ) {
                // Test "->"
                ASSERT( k+2==i->serial, NULL );
            }
            // Test assignment
            old_i = i;
        }
        ASSERT( k+1==f.serial, NULL );
    }
    ASSERT( !(i!=j), NULL );
    ASSERT( i==j, NULL );
}

template<typename Iterator1, typename Iterator2>
void TestIteratorAssignment( Iterator2 j ) {
    Iterator1 i(j);
    ASSERT( i==j, NULL );
    ASSERT( !(i!=j), NULL );
    Iterator1 k;
    k = j;
    ASSERT( k==j, NULL );
    ASSERT( !(k!=j), NULL );
}

template<typename Iterator, typename T>
void TestIteratorTraits() {
    AssertSameType( static_cast<typename Iterator::difference_type*>(0), static_cast<ptrdiff_t*>(0) );
    AssertSameType( static_cast<typename Iterator::value_type*>(0), static_cast<T*>(0) );
    AssertSameType( static_cast<typename Iterator::pointer*>(0), static_cast<T**>(0) );
    AssertSameType( static_cast<typename Iterator::iterator_category*>(0), static_cast<std::forward_iterator_tag*>(0) );
    T x;
    typename Iterator::reference xr = x;
    typename Iterator::pointer xp = &x;
    ASSERT( &xr==xp, NULL );
}

//! Test the iterators for concurrent_queue
void TestIterator() {
    tbb::concurrent_queue<Foo> queue;
    const tbb::concurrent_queue<Foo>& const_queue = queue;
    for( int j=0; j<500; ++j ) {
        TestIteratorAux( CALL_BEGIN(queue,j)      , CALL_END(queue,j)      , j );
        TestIteratorAux( CALL_BEGIN(const_queue,j), CALL_END(const_queue,j), j );
        TestIteratorAux( CALL_BEGIN(const_queue,j), CALL_END(queue,j)      , j );
        TestIteratorAux( CALL_BEGIN(queue,j)      , CALL_END(const_queue,j), j );
        Foo f;
        f.serial = j+1;
        queue.push(f);
    }
    TestIteratorAssignment<tbb::concurrent_queue<Foo>::const_iterator>( const_queue.unsafe_begin() );
    TestIteratorAssignment<tbb::concurrent_queue<Foo>::const_iterator>( queue.unsafe_begin() );
    TestIteratorAssignment<tbb::concurrent_queue<Foo>::iterator>( queue.unsafe_begin() );
    TestIteratorTraits<tbb::concurrent_queue<Foo>::const_iterator, const Foo>();
    TestIteratorTraits<tbb::concurrent_queue<Foo>::iterator, Foo>();
}

void TestConcurrentQueueType() {
    AssertSameType( tbb::concurrent_queue<Foo>::value_type(), Foo() );
    Foo f;
    const Foo g;
    tbb::concurrent_queue<Foo>::reference r = f;
    ASSERT( &r==&f, NULL );
    ASSERT( !r.is_const(), NULL );
    tbb::concurrent_queue<Foo>::const_reference cr = g;
    ASSERT( &cr==&g, NULL );
    ASSERT( cr.is_const(), NULL );
}

template<typename T>
void TestEmptyQueue() {
    const tbb::concurrent_queue<T> queue;
    ASSERT( queue.SIZE()==0, NULL );
#if TBB_DEPRECATED
    ASSERT( queue.capacity()>0, NULL );
    ASSERT( size_t(queue.capacity())>=size_t(-1)/(sizeof(void*)+sizeof(T)), NULL );
#endif
}

#if TBB_DEPRECATED
#define CALL_TRY_PUSH(q,f,i) (((i)&0x1)?(q).push_if_not_full(f):(q).try_push(f))
void TestFullQueue() {
    for( int n=0; n<10; ++n ) {
        FooConstructed = 0;
        FooDestroyed = 0;
        tbb::concurrent_queue<Foo> queue;
        queue.set_capacity(n);
        for( int i=0; i<=n; ++i ) {
            Foo f;
            f.serial = i;
            bool result = CALL_TRY_PUSH(queue, f, i );
            ASSERT( result==(i<n), NULL );
        }
        for( int i=0; i<=n; ++i ) {
            Foo f;
            bool result = queue.pop_if_present( f );
            ASSERT( result==(i<n), NULL );
            ASSERT( !result || f.serial==i, NULL );
        }
        ASSERT( FooConstructed==FooDestroyed, NULL );
    }
}
#endif /* if TBB_DEPRECATED */

#if TBB_DEPRECATED
#define CALL_PUSH_IF_NOT_FULL(q,v,i) (((i)&0x1)?q.push_if_not_full(v):(q.push(v), true))
#else
#define CALL_PUSH_IF_NOT_FULL(q,v,i) (q.push(v), true)
#endif

void TestClear() {
    FooConstructed = 0;
    FooDestroyed = 0;
    const unsigned int n=5;
        
    tbb::concurrent_queue<Foo> queue;
#if TBB_DEPRECATED
    const int q_capacity=10;
    queue.set_capacity(q_capacity);
#endif
    for( size_t i=0; i<n; ++i ) {
        Foo f;
        f.serial = int(i);
        bool result = CALL_PUSH_IF_NOT_FULL(queue, f, i);
        ASSERT( result, NULL );
    }
    ASSERT( unsigned(queue.SIZE())==n, NULL );
    queue.clear();
    ASSERT( queue.SIZE()==0, NULL );
    for( size_t i=0; i<n; ++i ) {
        Foo f;
        f.serial = int(i);
        bool result = CALL_PUSH_IF_NOT_FULL(queue, f, i);
        ASSERT( result, NULL );
    }
    ASSERT( unsigned(queue.SIZE())==n, NULL );
    queue.clear();
    ASSERT( queue.SIZE()==0, NULL );
    for( size_t i=0; i<n; ++i ) {
        Foo f;
        f.serial = int(i);
        bool result = CALL_PUSH_IF_NOT_FULL(queue, f, i);
        ASSERT( result, NULL );
    }
    ASSERT( unsigned(queue.SIZE())==n, NULL );
}

#if TBB_DEPRECATED
template<typename T>
struct TestNegativeQueueBody: NoAssign {
    tbb::concurrent_queue<T>& queue;
    const int nthread;
    TestNegativeQueueBody( tbb::concurrent_queue<T>& q, int n ) : queue(q), nthread(n) {}
    void operator()( int k ) const {
        if( k==0 ) {
            int number_of_pops = nthread-1;
            // Wait for all pops to pend.
            while( queue.size()>-number_of_pops ) {
                __TBB_Yield();
            }
            for( int i=0; ; ++i ) {
                ASSERT( queue.size()==i-number_of_pops, NULL );
                ASSERT( queue.empty()==(queue.size()<=0), NULL );
                if( i==number_of_pops ) break;
                // Satisfy another pop
                queue.push( T() );
            }
        } else {
            // Pop item from queue
            T item;
            queue.pop(item);
        }
    }
};

//! Test a queue with a negative size.
template<typename T>
void TestNegativeQueue( int nthread ) {
    tbb::concurrent_queue<T> queue;
    NativeParallelFor( nthread, TestNegativeQueueBody<T>(queue,nthread) );
}
#endif /* if TBB_DEPRECATED */

#if TBB_USE_EXCEPTIONS
void TestExceptions() {
    typedef static_counting_allocator<std::allocator<FooEx>, size_t> allocator_t;
    typedef static_counting_allocator<std::allocator<char>, size_t> allocator_char_t;
    typedef tbb::concurrent_queue<FooEx, allocator_t> concur_queue_t;

    enum methods {
        m_push = 0,
        m_pop
    };  

    REMARK("Testing exception safety\n");
    // verify 'clear()' on exception; queue's destructor calls its clear()
    // Do test on queues of two different types at the same time to 
    // catch problem with incorrect sharing between templates.
    {
        concur_queue_t queue0;
        tbb::concurrent_queue<int,allocator_t> queue1;
        for( int i=0; i<2; ++i ) {
            bool caught = false;
            try {
                allocator_char_t::init_counters();
                allocator_char_t::set_limits(N/2);
                for( int k=0; k<N; k++ ) {
                    if( i==0 )
                        queue0.push( FooEx() );
                    else
                        queue1.push( k );
                }
            } catch (...) {
                caught = true;
            }
            ASSERT( caught, "call to push should have thrown exception" );
        }
    }
    REMARK("... queue destruction test passed\n");

    try {
        int n_pushed=0, n_popped=0;
        for(int t = 0; t <= 1; t++)// exception type -- 0 : from allocator(), 1 : from Foo's constructor
        {
            concur_queue_t queue_test;
            for( int m=m_push; m<=m_pop; m++ ) {
                // concurrent_queue internally rebinds the allocator to one with 'char'
                allocator_char_t::init_counters();

                if(t) MaxFooCount = MaxFooCount + 400;
                else allocator_char_t::set_limits(N/2);

                try {
                    switch(m) {
                    case m_push:
                            for( int k=0; k<N; k++ ) {
                                queue_test.push( FooEx() );
                                n_pushed++;
                            }
                            break;
                    case m_pop:
                            n_popped=0;
                            for( int k=0; k<n_pushed; k++ ) {
                                FooEx elt;
                                queue_test.try_pop( elt );
                                n_popped++;
                            }
                            n_pushed = 0;
                            allocator_char_t::set_limits(); 
                            break;
                    }
                    if( !t && m==m_push ) ASSERT(false, "should throw an exception");
                } catch ( Foo_exception & ) {
                    switch(m) {
                    case m_push: {
                                ASSERT( ptrdiff_t(queue_test.SIZE())==n_pushed, "incorrect queue size" );
                                long tc = MaxFooCount;
                                MaxFooCount = 0;
                                for( int k=0; k<(int)tc; k++ ) {
                                    queue_test.push( FooEx() );
                                    n_pushed++;
                                }
                                MaxFooCount = tc;
                            }
                            break;
                    case m_pop:
                            MaxFooCount = 0; // disable exception
                            n_pushed -= (n_popped+1); // including one that threw an exception
                            ASSERT( n_pushed>=0, "n_pushed cannot be less than 0" );
                            for( int k=0; k<1000; k++ ) {
                                queue_test.push( FooEx() );
                                n_pushed++;
                            }
                            ASSERT( !queue_test.empty(), "queue must not be empty" );
                            ASSERT( ptrdiff_t(queue_test.SIZE())==n_pushed, "queue size must be equal to n pushed" );
                            for( int k=0; k<n_pushed; k++ ) {
                                FooEx elt;
                                queue_test.try_pop( elt );
                            }
                            ASSERT( queue_test.empty(), "queue must be empty" );
                            ASSERT( queue_test.SIZE()==0, "queue must be empty" );
                            break;
                    }
                } catch ( std::bad_alloc & ) {
                    allocator_char_t::set_limits(); // disable exception from allocator
                    size_t size = queue_test.SIZE();
                    switch(m) {
                    case m_push:
                            ASSERT( size>0, "incorrect queue size");
                            break;
                    case m_pop:
                            if( !t ) ASSERT( false, "should not throw an exceptin" );
                            break;
                    }
                }
                REMARK("... for t=%d and m=%d, exception test passed\n", t, m);
            }
        }
    } catch(...) {
        ASSERT(false, "unexpected exception");
    }
}
#endif /* TBB_USE_EXCEPTIONS */

template<typename T>
struct TestQueueElements: NoAssign {
    tbb::concurrent_queue<T>& queue;
    const int nthread;
    TestQueueElements( tbb::concurrent_queue<T>& q, int n ) : queue(q), nthread(n) {}
    void operator()( int k ) const {
        for( int i=0; i<1000; ++i ) {
            if( (i&0x1)==0 ) {
                __TBB_ASSERT( T(k)<T(nthread), NULL );
                queue.push( T(k) );
            } else {
                // Pop item from queue
                T item;
                queue.try_pop(item);
                __TBB_ASSERT( item<=T(nthread), NULL );
            }
        }
    }
};

//! Test concurrent queue with primitive data type
template<typename T>
void TestPrimitiveTypes( int nthread, T exemplar )
{
    tbb::concurrent_queue<T> queue;
    for( int i=0; i<100; ++i )
        queue.push( exemplar );
    NativeParallelFor( nthread, TestQueueElements<T>(queue,nthread) );
}

#include "harness_m128.h"

#if HAVE_m128

//! Test concurrent queue with SSE type
/** Type Queue should be a queue of ClassWithSSE. */
template<typename Queue>
void TestSSE() {
    Queue q1;
    for( int i=0; i<100; ++i )
        q1.push(ClassWithSSE(i));

    // Copy the queue
    Queue q2 = q1;
    // Check that elements of the copy are correct
    typename Queue::const_iterator ci = q2.unsafe_begin();
    for( int i=0; i<100; ++i ) {
        ClassWithSSE foo = *ci;
        ASSERT( *ci==ClassWithSSE(i), NULL );
        ++ci;
    }

    for( int i=0; i<101; ++i ) {
        ClassWithSSE tmp;
        bool b = q1.try_pop( tmp );
        ASSERT( b==(i<100), NULL );
        ASSERT( !b || tmp==ClassWithSSE(i), NULL );
    }
}
#endif /* HAVE_m128 */

int TestMain () {
    TestEmptyQueue<char>();
    TestEmptyQueue<Foo>();
#if TBB_DEPRECATED
    TestFullQueue();
#endif
    TestClear();
    TestConcurrentQueueType();
    TestIterator();
    TestConstructors();

    TestPrimitiveTypes( MaxThread, (char)1 );
    TestPrimitiveTypes( MaxThread, (int)-12 );
    TestPrimitiveTypes( MaxThread, (float)-1.2f );
    TestPrimitiveTypes( MaxThread, (double)-4.3 );
#if HAVE_m128
    TestSSE<tbb::concurrent_queue<ClassWithSSE> >();
    TestSSE<tbb::concurrent_bounded_queue<ClassWithSSE> >();
#endif /* HAVE_m128 */

    // Test concurrent operations
    for( int nthread=MinThread; nthread<=MaxThread; ++nthread ) {
#if TBB_DEPRECATED
        TestNegativeQueue<Foo>(nthread);
#endif
        for( size_t prefill=0; prefill<64; prefill+=(1+prefill/3) ) {
            TestPushPop(prefill,ptrdiff_t(-1),nthread);
            TestPushPop(prefill,ptrdiff_t(1),nthread);
            TestPushPop(prefill,ptrdiff_t(2),nthread);
            TestPushPop(prefill,ptrdiff_t(10),nthread);
            TestPushPop(prefill,ptrdiff_t(100),nthread);
        }
    }
#if __TBB_THROW_ACROSS_MODULE_BOUNDARY_BROKEN
    REPORT("Known issue: exception safety test is skipped.\n");
#elif TBB_USE_EXCEPTIONS
    TestExceptions();
#endif /* TBB_USE_EXCEPTIONS */
    return Harness::Done;
}
