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

#include "tbb/concurrent_vector.h"
#include "tbb/tbb_allocator.h"
#include "tbb/cache_aligned_allocator.h"
#include "tbb/tbb_exception.h"
#include <cstdio>
#include <cstdlib>
#include "harness_report.h"
#include "harness_assert.h"
#include "harness_allocator.h"

#if TBB_USE_EXCEPTIONS
static bool known_issue_verbose = false;
#define KNOWN_ISSUE(msg) if(!known_issue_verbose) known_issue_verbose = true, REPORT(msg)
#endif /* TBB_USE_EXCEPTIONS */

tbb::atomic<long> FooCount;
long MaxFooCount = 0;

//! Problem size
const size_t N = 500000;

//! Exception for concurrent_vector
class Foo_exception : public std::bad_alloc {
public:
    virtual const char *what() const throw() { return "out of Foo limit"; }
    virtual ~Foo_exception() throw() {}
};

static const int initial_value_of_bar = 42;
struct Foo {
    int my_bar;
public:
    enum State {
        ZeroInitialized=0,
        DefaultInitialized=0xDEFAUL,
        CopyInitialized=0xC0314,
        Destroyed=0xDEADF00
    } state;
    bool is_valid() const {
        return state==DefaultInitialized||state==CopyInitialized;
    }
    bool is_valid_or_zero() const {
        return is_valid()||(state==ZeroInitialized && !my_bar);
    }
    int& zero_bar() {
        ASSERT( is_valid_or_zero(), NULL );
        return my_bar;
    }
    int& bar() {
        ASSERT( is_valid(), NULL );
        return my_bar;
    }
    int bar() const {
        ASSERT( is_valid(), NULL );
        return my_bar;
    }
    Foo( int barr = initial_value_of_bar ) {
        my_bar = barr;
        if(MaxFooCount && FooCount >= MaxFooCount)
            __TBB_THROW( Foo_exception() );
        FooCount++;
        state = DefaultInitialized;
    }
    Foo( const Foo& foo ) {
        my_bar = foo.my_bar;
        ASSERT( foo.is_valid_or_zero(), "bad source for copy" );
        if(MaxFooCount && FooCount >= MaxFooCount)
            __TBB_THROW( Foo_exception() );
        FooCount++;
        state = CopyInitialized;
    }
    ~Foo() {
        ASSERT( is_valid_or_zero(), NULL );
        my_bar = ~initial_value_of_bar;
        if(state != ZeroInitialized) --FooCount;
        state = Destroyed;
    }
    bool operator==(const Foo &f) const { return my_bar == f.my_bar; }
    bool operator<(const Foo &f) const { return my_bar < f.my_bar; }
    bool is_const() const {return true;}
    bool is_const() {return false;}
protected:
    char reserve[1];
    void operator=( const Foo& ) {}
};

class FooWithAssign: public Foo {
public:
    void operator=( const FooWithAssign& x ) {
        my_bar = x.my_bar;
        ASSERT( x.is_valid_or_zero(), "bad source for assignment" );
        ASSERT( is_valid(), NULL );
    } 
    bool operator==(const Foo &f) const { return my_bar == f.my_bar; }
    bool operator<(const Foo &f) const { return my_bar < f.my_bar; }
};

class FooIterator: public std::iterator<std::input_iterator_tag,FooWithAssign> {
    int x_bar;
public:
    FooIterator(int x) {
        x_bar = x;
    }
    FooIterator &operator++() {
        x_bar++; return *this;
    }
    FooWithAssign operator*() {
        FooWithAssign foo; foo.bar() = x_bar;
        return foo;
    }
    bool operator!=(const FooIterator &i) { return x_bar != i.x_bar; }
};

inline void NextSize( int& s ) {
    if( s<=32 ) ++s;
    else s += s/10;
}

//! Check vector have expected size and filling
template<typename vector_t>
static void CheckVector( const vector_t& cv, size_t expected_size, size_t old_size ) {
    ASSERT( cv.capacity()>=expected_size, NULL );
    ASSERT( cv.size()==expected_size, NULL );
    ASSERT( cv.empty()==(expected_size==0), NULL );
    for( int j=0; j<int(expected_size); ++j ) {
        if( cv[j].bar()!=~j )
            REPORT("ERROR on line %d for old_size=%ld expected_size=%ld j=%d\n",__LINE__,long(old_size),long(expected_size),j);
    }
}

//! Test of assign, grow, copying with various sizes
void TestResizeAndCopy() {
    typedef static_counting_allocator<debug_allocator<Foo,std::allocator>, std::size_t> allocator_t;
    typedef tbb::concurrent_vector<Foo, allocator_t> vector_t;
    allocator_t::init_counters();
    for( int old_size=0; old_size<=128; NextSize( old_size ) ) {
        for( int new_size=0; new_size<=1280; NextSize( new_size ) ) {
            long count = FooCount;
            vector_t v;
            ASSERT( count==FooCount, NULL );
            v.assign(old_size/2, Foo() );
            ASSERT( count+old_size/2==FooCount, NULL );
            for( int j=0; j<old_size/2; ++j )
                ASSERT( v[j].state == Foo::CopyInitialized, NULL);
            v.assign(FooIterator(0), FooIterator(old_size));
            v.resize(new_size, Foo(33) );
            ASSERT( count+new_size==FooCount, NULL );
            for( int j=0; j<new_size; ++j ) {
                int expected = j<old_size ? j : 33;
                if( v[j].bar()!=expected ) 
                    REPORT("ERROR on line %d for old_size=%ld new_size=%ld v[%ld].bar()=%d != %d\n",__LINE__,long(old_size),long(new_size),long(j),v[j].bar(), expected);
            }
            ASSERT( v.size()==size_t(new_size), NULL );
            for( int j=0; j<new_size; ++j ) {
                v[j].bar() = ~j;
            }
            const vector_t& cv = v;
            // Try copy constructor
            vector_t copy_of_v(cv);
            CheckVector(cv,new_size,old_size);
            ASSERT( !(v != copy_of_v), NULL );
            v.clear();
            ASSERT( v.empty(), NULL );
            swap(v, copy_of_v);
            ASSERT( copy_of_v.empty(), NULL );
            CheckVector(v,new_size,old_size);
        }
    }
    ASSERT( allocator_t::items_allocated == allocator_t::items_freed, NULL);
    ASSERT( allocator_t::allocations == allocator_t::frees, NULL);
}

//! Test reserve, compact, capacity
void TestCapacity() {
    typedef static_counting_allocator<debug_allocator<Foo,tbb::cache_aligned_allocator>, std::size_t> allocator_t;
    typedef tbb::concurrent_vector<Foo, allocator_t> vector_t;
    allocator_t::init_counters();
    for( size_t old_size=0; old_size<=11000; old_size=(old_size<5 ? old_size+1 : 3*old_size) ) {
        for( size_t new_size=0; new_size<=11000; new_size=(new_size<5 ? new_size+1 : 3*new_size) ) {
            long count = FooCount; 
            {
                vector_t v; v.reserve(old_size);
                ASSERT( v.capacity()>=old_size, NULL );
                v.reserve( new_size );
                ASSERT( v.capacity()>=old_size, NULL );
                ASSERT( v.capacity()>=new_size, NULL );
                ASSERT( v.empty(), NULL );
                size_t fill_size = 2*new_size;
                for( size_t i=0; i<fill_size; ++i ) {
                    ASSERT( size_t(FooCount)==count+i, NULL );
#if TBB_DEPRECATED
                    size_t j = v.grow_by(1);
#else
                    size_t j = v.grow_by(1) - v.begin();
#endif
                    ASSERT( j==i, NULL );
                    v[j].bar() = int(~j);
                }
                vector_t copy_of_v(v); // should allocate first segment with same size as for shrink_to_fit()
                if(__TBB_Log2(/*reserved size*/old_size|1) > __TBB_Log2(fill_size|1) )
                    ASSERT( v.capacity() != copy_of_v.capacity(), NULL );
                v.shrink_to_fit();
                ASSERT( v.capacity() == copy_of_v.capacity(), NULL );
                CheckVector(v, new_size*2, old_size); // check vector correctness
                ASSERT( v==copy_of_v, NULL ); // TODO: check also segments layout equality
            }
            ASSERT( FooCount==count, NULL );
        }
    } 
    ASSERT( allocator_t::items_allocated == allocator_t::items_freed, NULL);
    ASSERT( allocator_t::allocations == allocator_t::frees, NULL);
}

struct AssignElement {
    typedef tbb::concurrent_vector<int>::range_type::iterator iterator;
    iterator base;
    void operator()( const tbb::concurrent_vector<int>::range_type& range ) const {
        for( iterator i=range.begin(); i!=range.end(); ++i ) {
            if( *i!=0 )
                REPORT("ERROR for v[%ld]\n", long(i-base));
            *i = int(i-base);
        }
    }
    AssignElement( iterator base_ ) : base(base_) {}
};

struct CheckElement {
    typedef tbb::concurrent_vector<int>::const_range_type::iterator iterator;
    iterator base;
    void operator()( const tbb::concurrent_vector<int>::const_range_type& range ) const {
        for( iterator i=range.begin(); i!=range.end(); ++i )
            if( *i != int(i-base) )
                REPORT("ERROR for v[%ld]\n", long(i-base));
    }
    CheckElement( iterator base_ ) : base(base_) {}
};

#include "tbb/tick_count.h"
#include "tbb/parallel_for.h"
#include "harness.h"

//! Test parallel access by iterators
void TestParallelFor( int nthread ) {
    typedef tbb::concurrent_vector<int> vector_t;
    vector_t v;
    v.resize(N);
    tbb::tick_count t0 = tbb::tick_count::now();
    REMARK("Calling parallel_for with %ld threads\n",long(nthread));
    tbb::parallel_for( v.range(10000), AssignElement(v.begin()) );
    tbb::tick_count t1 = tbb::tick_count::now();
    const vector_t& u = v;
    tbb::parallel_for( u.range(10000), CheckElement(u.begin()) );
    tbb::tick_count t2 = tbb::tick_count::now();
    REMARK("Time for parallel_for: assign time = %8.5f, check time = %8.5f\n",
               (t1-t0).seconds(),(t2-t1).seconds());
    for( long i=0; size_t(i)<v.size(); ++i )
        if( v[i]!=i )
            REPORT("ERROR for v[%ld]\n", i);
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

template<typename Range1, typename Range2>
void TestRangeAssignment( Range2 r2 ) {
    Range1 r1(r2); r1 = r2;
}

template<typename Iterator, typename T>
void TestIteratorTraits() {
    AssertSameType( static_cast<typename Iterator::difference_type*>(0), static_cast<ptrdiff_t*>(0) ); 
    AssertSameType( static_cast<typename Iterator::value_type*>(0), static_cast<T*>(0) ); 
    AssertSameType( static_cast<typename Iterator::pointer*>(0), static_cast<T**>(0) ); 
    AssertSameType( static_cast<typename Iterator::iterator_category*>(0), static_cast<std::random_access_iterator_tag*>(0) );
    T x;
    typename Iterator::reference xr = x;
    typename Iterator::pointer xp = &x;
    ASSERT( &xr==xp, NULL );
}

template<typename Vector, typename Iterator>
void CheckConstIterator( const Vector& u, int i, const Iterator& cp ) {
    typename Vector::const_reference pref = *cp;
    if( pref.bar()!=i )
        REPORT("ERROR for u[%ld] using const_iterator\n", long(i));
    typename Vector::difference_type delta = cp-u.begin();
    ASSERT( delta==i, NULL );
    if( u[i].bar()!=i )
        REPORT("ERROR for u[%ld] using subscripting\n", long(i));
    ASSERT( u.begin()[i].bar()==i, NULL );
}

template<typename Iterator1, typename Iterator2, typename V> 
void CheckIteratorComparison( V& u ) {
    V u2 = u;
    Iterator1 i = u.begin();

    for( int i_count=0; i_count<100; ++i_count ) {
        Iterator2 j = u.begin();
        Iterator2 i2 = u2.begin();
        for( int j_count=0; j_count<100; ++j_count ) {
            ASSERT( (i==j)==(i_count==j_count), NULL );
            ASSERT( (i!=j)==(i_count!=j_count), NULL );
            ASSERT( (i-j)==(i_count-j_count), NULL );
            ASSERT( (i<j)==(i_count<j_count), NULL );
            ASSERT( (i>j)==(i_count>j_count), NULL );
            ASSERT( (i<=j)==(i_count<=j_count), NULL );
            ASSERT( (i>=j)==(i_count>=j_count), NULL );
            ASSERT( !(i==i2), NULL ); 
            ASSERT( i!=i2, NULL ); 
            ++j;
            ++i2;
        }
        ++i;
    }
}

//! Test sequential iterators for vector type V.
/** Also does timing. */
template<typename T>
void TestSequentialFor() {
    typedef tbb::concurrent_vector<FooWithAssign> V;
    V v(N);
    ASSERT(v.grow_by(0) == v.grow_by(0, FooWithAssign()), NULL);

    // Check iterator 
    tbb::tick_count t0 = tbb::tick_count::now();
    typename V::iterator p = v.begin();
    ASSERT( !(*p).is_const(), NULL );
    ASSERT( !p->is_const(), NULL );
    for( int i=0; size_t(i)<v.size(); ++i, ++p ) {
        if( (*p).state!=Foo::DefaultInitialized )
            REPORT("ERROR for v[%ld]\n", long(i));
        typename V::reference pref = *p;
        pref.bar() = i;
        typename V::difference_type delta = p-v.begin();
        ASSERT( delta==i, NULL );
        ASSERT( -delta<=0, "difference type not signed?" );
    }
    tbb::tick_count t1 = tbb::tick_count::now();
    
    // Check const_iterator going forwards
    const V& u = v;
    typename V::const_iterator cp = u.begin();
    ASSERT( cp == v.cbegin(), NULL );
    ASSERT( (*cp).is_const(), NULL );
    ASSERT( cp->is_const(), NULL );
    ASSERT( *cp == v.front(), NULL);
    for( int i=0; size_t(i)<u.size(); ++i ) {
        CheckConstIterator(u,i,cp);
        V::const_iterator &cpr = ++cp;
        ASSERT( &cpr == &cp, "preincrement not returning a reference?");
    }
    tbb::tick_count t2 = tbb::tick_count::now();
    REMARK("Time for serial for:  assign time = %8.5f, check time = %8.5f\n",
               (t1-t0).seconds(),(t2-t1).seconds());

    // Now go backwards
    cp = u.end();
    ASSERT( cp == v.cend(), NULL );
    for( int i=int(u.size()); i>0; ) {
        --i;
        V::const_iterator &cpr = --cp;
        ASSERT( &cpr == &cp, "predecrement not returning a reference?");
        if( i>0 ) {
            typename V::const_iterator cp_old = cp--;
            int here = (*cp_old).bar();
            ASSERT( here==u[i].bar(), NULL );
            typename V::const_iterator cp_new = cp++;
            int prev = (*cp_new).bar();
            ASSERT( prev==u[i-1].bar(), NULL );
        }
        CheckConstIterator(u,i,cp);
    }

    // Now go forwards and backwards
    ptrdiff_t k = 0;
    cp = u.begin();
    for( size_t i=0; i<u.size(); ++i ) {
        CheckConstIterator(u,int(k),cp);
        typename V::difference_type delta = i*3 % u.size();
        if( 0<=k+delta && size_t(k+delta)<u.size() ) {
            V::const_iterator &cpr = (cp += delta);
            ASSERT( &cpr == &cp, "+= not returning a reference?");
            k += delta; 
        } 
        delta = i*7 % u.size();
        if( 0<=k-delta && size_t(k-delta)<u.size() ) {
            if( i&1 ) { 
                V::const_iterator &cpr = (cp -= delta);
                ASSERT( &cpr == &cp, "-= not returning a reference?");
            } else
                cp = cp - delta;        // Test operator-
            k -= delta; 
        } 
    }
    
    for( int i=0; size_t(i)<u.size(); i=(i<50?i+1:i*3) )
        for( int j=-i; size_t(i+j)<u.size(); j=(j<50?j+1:j*5) ) {
            ASSERT( (u.begin()+i)[j].bar()==i+j, NULL );
            ASSERT( (v.begin()+i)[j].bar()==i+j, NULL );
            ASSERT((v.cbegin()+i)[j].bar()==i+j, NULL );
            ASSERT( (i+u.begin())[j].bar()==i+j, NULL );
            ASSERT( (i+v.begin())[j].bar()==i+j, NULL );
            ASSERT((i+v.cbegin())[j].bar()==i+j, NULL );
        }

    CheckIteratorComparison<typename V::iterator, typename V::iterator>(v);
    CheckIteratorComparison<typename V::iterator, typename V::const_iterator>(v);
    CheckIteratorComparison<typename V::const_iterator, typename V::iterator>(v);
    CheckIteratorComparison<typename V::const_iterator, typename V::const_iterator>(v);

    TestIteratorAssignment<typename V::const_iterator>( u.begin() );
    TestIteratorAssignment<typename V::const_iterator>( v.begin() );
    TestIteratorAssignment<typename V::const_iterator>( v.cbegin() );
    TestIteratorAssignment<typename V::iterator>( v.begin() );
    // doesn't compile as expected: TestIteratorAssignment<typename V::iterator>( u.begin() );

    TestRangeAssignment<typename V::const_range_type>( u.range() );
    TestRangeAssignment<typename V::const_range_type>( v.range() );
    TestRangeAssignment<typename V::range_type>( v.range() );
    // doesn't compile as expected: TestRangeAssignment<typename V::range_type>( u.range() );

    // Check reverse_iterator 
    typename V::reverse_iterator rp = v.rbegin();
    for( size_t i=v.size(); i>0; --i, ++rp ) {
        typename V::reference pref = *rp;
        ASSERT( size_t(pref.bar())==i-1, NULL );
        ASSERT( rp!=v.rend(), NULL );
    }
    ASSERT( rp==v.rend(), NULL );
    
    // Check const_reverse_iterator 
    typename V::const_reverse_iterator crp = u.rbegin();
    ASSERT( crp == v.crbegin(), NULL );
    ASSERT( *crp == v.back(), NULL);
    for( size_t i=v.size(); i>0; --i, ++crp ) {
        typename V::const_reference cpref = *crp;
        ASSERT( size_t(cpref.bar())==i-1, NULL );
        ASSERT( crp!=u.rend(), NULL );
    }
    ASSERT( crp == u.rend(), NULL );
    ASSERT( crp == v.crend(), NULL );

    TestIteratorAssignment<typename V::const_reverse_iterator>( u.rbegin() );
    TestIteratorAssignment<typename V::reverse_iterator>( v.rbegin() );

    // test compliance with C++ Standard 2003, clause 23.1.1p9
    {
        tbb::concurrent_vector<int> v1, v2(1, 100);
        v1.assign(1, 100); ASSERT(v1 == v2, NULL);
        ASSERT(v1.size() == 1 && v1[0] == 100, "used integral iterators");
    }

    // cross-allocator tests
#if !defined(_WIN64) || defined(_CPPLIB_VER)
    typedef local_counting_allocator<std::allocator<int>, size_t> allocator1_t;
    typedef tbb::cache_aligned_allocator<void> allocator2_t;
    typedef tbb::concurrent_vector<FooWithAssign, allocator1_t> V1;
    typedef tbb::concurrent_vector<FooWithAssign, allocator2_t> V2;
    V1 v1( v ); // checking cross-allocator copying
    V2 v2( 10 ); v2 = v1; // checking cross-allocator assignment
    ASSERT( (v1 == v) && !(v2 != v), NULL);
    ASSERT( !(v1 < v) && !(v2 > v), NULL);
    ASSERT( (v1 <= v) && (v2 >= v), NULL);
#endif
}

static const size_t Modulus = 7;

typedef static_counting_allocator<debug_allocator<Foo> > MyAllocator;
typedef tbb::concurrent_vector<Foo, MyAllocator> MyVector;

template<typename MyVector>
class GrowToAtLeast: NoAssign {
    MyVector& my_vector;
public:
    void operator()( const tbb::blocked_range<size_t>& range ) const {
        for( size_t i=range.begin(); i!=range.end(); ++i ) {
            size_t n = my_vector.size();
            size_t req = (i % (2*n+1))+1;
#if TBB_DEPRECATED
            my_vector.grow_to_at_least(req);
#else
            typename MyVector::iterator p = my_vector.grow_to_at_least(req);
            if( p-my_vector.begin() < typename MyVector::difference_type(req) )
                ASSERT( p->state == Foo::DefaultInitialized || p->state == Foo::ZeroInitialized, NULL);
#endif
            ASSERT( my_vector.size()>=req, NULL );
        }
    }
    GrowToAtLeast( MyVector& vector ) : my_vector(vector) {}
};

void TestConcurrentGrowToAtLeast() {
    typedef static_counting_allocator< tbb::zero_allocator<Foo> > MyAllocator;
    typedef tbb::concurrent_vector<Foo, MyAllocator> MyVector;
    MyAllocator::init_counters();
    MyVector v(2, Foo(), MyAllocator());
    for( size_t s=1; s<1000; s*=10 ) {
        tbb::parallel_for( tbb::blocked_range<size_t>(0,10000*s,s), GrowToAtLeast<MyVector>(v), tbb::simple_partitioner() );
    }
    v.clear();
    ASSERT( 0 == v.get_allocator().frees, NULL);
    v.shrink_to_fit();
    size_t items_allocated = v.get_allocator().items_allocated,
           items_freed = v.get_allocator().items_freed;
    size_t allocations = v.get_allocator().allocations,
           frees = v.get_allocator().frees;
    ASSERT( items_allocated == items_freed, NULL);
    ASSERT( allocations == frees, NULL);
}

//! Test concurrent invocations of method concurrent_vector::grow_by
template<typename MyVector>
class GrowBy: NoAssign {
    MyVector& my_vector;
public:
    void operator()( const tbb::blocked_range<int>& range ) const {
        ASSERT( range.begin() < range.end(), NULL );
#if TBB_DEPRECATED
        for( int i=range.begin(); i!=range.end(); ++i )
#else
        int i = range.begin(), h = (range.end() - i) / 2;
        typename MyVector::iterator s = my_vector.grow_by(h);
        for( h += i; i < h; ++i, ++s )
            s->bar() = i;
        for(; i!=range.end(); ++i )
#endif
        {
            if( i&1 ) {
#if TBB_DEPRECATED
                typename MyVector::reference element = my_vector[my_vector.grow_by(1)]; 
                element.bar() = i;
#else
                my_vector.grow_by(1)->bar() = i;
#endif
            } else {
                typename MyVector::value_type f;
                f.bar() = i;
#if TBB_DEPRECATED
                size_t r;
#else
                typename MyVector::iterator r;
#endif
                if( i&2 )
                    r = my_vector.push_back( f );
                else
                    r = my_vector.grow_by(1, f);
#if TBB_DEPRECATED
                ASSERT( my_vector[r].bar()==i, NULL );
#else
                ASSERT( r->bar()==i, NULL );
#endif
            }
        }
    }
    GrowBy( MyVector& vector ) : my_vector(vector) {}
};

//! Test concurrent invocations of method concurrent_vector::grow_by
void TestConcurrentGrowBy( int nthread ) {
    MyAllocator::init_counters();
    {
        int m = 100000; MyAllocator a;
        MyVector v( a );
        tbb::parallel_for( tbb::blocked_range<int>(0,m,100), GrowBy<MyVector>(v), tbb::simple_partitioner() );
        ASSERT( v.size()==size_t(m), NULL );

        // Verify that v is a permutation of 0..m
        int inversions = 0, def_inits = 0, copy_inits = 0;
        bool* found = new bool[m];
        memset( found, 0, m );
        for( int i=0; i<m; ++i ) {
            if( v[i].state == Foo::DefaultInitialized ) ++def_inits;
            else if( v[i].state == Foo::CopyInitialized ) ++copy_inits;
            else {
                REMARK("i: %d ", i);
                ASSERT( false, "v[i] seems not initialized");
            }
            int index = v[i].bar();
            ASSERT( !found[index], NULL );
            found[index] = true;
            if( i>0 )
                inversions += v[i].bar()<v[i-1].bar();
        }
        for( int i=0; i<m; ++i ) {
            ASSERT( found[i], NULL );
            ASSERT( nthread>1 || v[i].bar()==i, "sequential execution is wrong" );
        }
        delete[] found;
        REMARK("Initialization by default constructor: %d, by copy: %d\n", def_inits, copy_inits);
        ASSERT( def_inits >= m/2, NULL );
        ASSERT( copy_inits >= m/4, NULL );
        if( nthread>1 && inversions<m/20 )
            REPORT("Warning: not much concurrency in TestConcurrentGrowBy (%d inversions)\n", inversions);
    }
    size_t items_allocated = MyAllocator::items_allocated,
           items_freed = MyAllocator::items_freed;
    size_t allocations = MyAllocator::allocations,
           frees = MyAllocator::frees;
    ASSERT( items_allocated == items_freed, NULL);
    ASSERT( allocations == frees, NULL);
}

//! Test the assignment operator and swap
void TestAssign() {
    typedef tbb::concurrent_vector<FooWithAssign, local_counting_allocator<std::allocator<FooWithAssign>, size_t > > vector_t;
    local_counting_allocator<std::allocator<FooWithAssign>, size_t > init_alloc;
    init_alloc.allocations = 100;
    for( int dst_size=1; dst_size<=128; NextSize( dst_size ) ) {
        for( int src_size=2; src_size<=128; NextSize( src_size ) ) {
            vector_t u(FooIterator(0), FooIterator(src_size), init_alloc);
            for( int i=0; i<src_size; ++i )
                ASSERT( u[i].bar()==i, NULL );
            vector_t v(dst_size, FooWithAssign(), init_alloc);
            for( int i=0; i<dst_size; ++i ) {
                ASSERT( v[i].state==Foo::CopyInitialized, NULL );
                v[i].bar() = ~i;
            }
            ASSERT( v != u, NULL);
            v.swap(u);
            CheckVector(u, dst_size, src_size);
            u.swap(v);
            // using assignment
            v = u;
            ASSERT( v == u, NULL);
            u.clear();
            ASSERT( u.size()==0, NULL );
            ASSERT( v.size()==size_t(src_size), NULL );
            for( int i=0; i<src_size; ++i )
                ASSERT( v[i].bar()==i, NULL );
            ASSERT( 0 == u.get_allocator().frees, NULL);
            u.shrink_to_fit(); // deallocate unused memory
            size_t items_allocated = u.get_allocator().items_allocated,
                   items_freed = u.get_allocator().items_freed;
            size_t allocations = u.get_allocator().allocations,
                   frees = u.get_allocator().frees + 100;
            ASSERT( items_allocated == items_freed, NULL);
            ASSERT( allocations == frees, NULL);
        }
    }
}

// Test the comparison operators
#if !TBB_USE_EXCEPTIONS && _MSC_VER
    // Suppress "C++ exception handler used, but unwind semantics are not enabled" warning in STL headers
    #pragma warning (push)
    #pragma warning (disable: 4530)
#endif

#include <string>

#if !TBB_USE_EXCEPTIONS && _MSC_VER
    #pragma warning (pop)
#endif

void TestComparison() {
    std::string str[3]; str[0] = "abc";
    str[1].assign("cba");
    str[2].assign("abc"); // same as 0th
    tbb::concurrent_vector<char> var[3];
    var[0].assign(str[0].begin(), str[0].end());
    var[1].assign(str[0].rbegin(), str[0].rend());
    var[2].assign(var[1].rbegin(), var[1].rend()); // same as 0th
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            ASSERT( (var[i] == var[j]) == (str[i] == str[j]), NULL );
            ASSERT( (var[i] != var[j]) == (str[i] != str[j]), NULL );
            ASSERT( (var[i] < var[j]) == (str[i] < str[j]), NULL );
            ASSERT( (var[i] > var[j]) == (str[i] > str[j]), NULL );
            ASSERT( (var[i] <= var[j]) == (str[i] <= str[j]), NULL );
            ASSERT( (var[i] >= var[j]) == (str[i] >= str[j]), NULL );
        }
    }
}

//------------------------------------------------------------------------
// Regression test for problem where on oversubscription caused
// concurrent_vector::grow_by to run very slowly (TR#196).
//------------------------------------------------------------------------

#include "tbb/task_scheduler_init.h"
#include <math.h>

typedef unsigned long Number;

static tbb::concurrent_vector<Number> Primes;

class FindPrimes {
    bool is_prime( Number val ) const {
        int limit, factor = 3;
        if( val<5u ) 
            return val==2;
        else {
            limit = long(sqrtf(float(val))+0.5f);
            while( factor<=limit && val % factor )
                ++factor;
            return factor>limit;
        }
    }
public:
    void operator()( const tbb::blocked_range<Number>& r ) const {
        for( Number i=r.begin(); i!=r.end(); ++i ) { 
            if( i%2 && is_prime(i) ) {
#if TBB_DEPRECATED
                Primes[Primes.grow_by(1)] = i;
#else
                Primes.push_back( i );
#endif
            }
        }
    }
};

double TimeFindPrimes( int nthread ) {
    Primes.clear();
    Primes.reserve(1000000);// TODO: or compact()?
    tbb::task_scheduler_init init(nthread);
    tbb::tick_count t0 = tbb::tick_count::now();
    tbb::parallel_for( tbb::blocked_range<Number>(0,1000000,500), FindPrimes() );
    tbb::tick_count t1 = tbb::tick_count::now();
    return (t1-t0).seconds();
}

void TestFindPrimes() {
    // Time fully subscribed run.
    double t2 = TimeFindPrimes( tbb::task_scheduler_init::automatic );

    // Time parallel run that is very likely oversubscribed.  
#if _XBOX
    double t128 = TimeFindPrimes(32);  //XBOX360 can't handle too many threads
#else    
    double t128 = TimeFindPrimes(128);
#endif
    REMARK("TestFindPrimes: t2==%g t128=%g k=%g\n", t2, t128, t128/t2);

    // We allow the 128-thread run a little extra time to allow for thread overhead.
    // Theoretically, following test will fail on machine with >128 processors.
    // But that situation is not going to come up in the near future,
    // and the generalization to fix the issue is not worth the trouble.
    if( t128 > 1.3*t2 ) {
        REPORT("Warning: grow_by is pathetically slow: t2==%g t128=%g k=%g\n", t2, t128, t128/t2);
    } 
}

//------------------------------------------------------------------------
// Test compatibility with STL sort.
//------------------------------------------------------------------------

#include <algorithm>

void TestSort() {
    for( int n=0; n<100; n=n*3+1 ) {
        tbb::concurrent_vector<int> array(n);
        for( int i=0; i<n; ++i )
            array.at(i) = (i*7)%n;
        std::sort( array.begin(), array.end() );
        for( int i=0; i<n; ++i )
            ASSERT( array[i]==i, NULL );
    }
}

#if TBB_USE_EXCEPTIONS
//------------------------------------------------------------------------
// Test exceptions safety (from allocator and items constructors)
//------------------------------------------------------------------------
void TestExceptions() {
    typedef static_counting_allocator<debug_allocator<FooWithAssign>, std::size_t> allocator_t;
    typedef tbb::concurrent_vector<FooWithAssign, allocator_t> vector_t;

    enum methods {
        zero_method = 0,
        ctor_copy, ctor_size, assign_nt, assign_ir, op_equ, reserve, compact, grow,
        all_methods
    };
    ASSERT( !FooCount, NULL );

    try {
        vector_t src(FooIterator(0), FooIterator(N)); // original data

        for(int t = 0; t < 2; ++t) // exception type
        for(int m = zero_method+1; m < all_methods; ++m)
        {
            ASSERT( FooCount == N, "Previous iteration miss some Foo's de-/initialization" );
            allocator_t::init_counters();
            if(t) MaxFooCount = FooCount + N/4;
            else allocator_t::set_limits(N/4);
            vector_t victim;
            try {
                switch(m) {
                case ctor_copy: {
                        vector_t acopy(src);
                    } break; // auto destruction after exception is checked by ~Foo
                case ctor_size: {
                        vector_t sized(N);
                    } break; // auto destruction after exception is checked by ~Foo
                // Do not test assignment constructor due to reusing of same methods as below 
                case assign_nt: {
                        victim.assign(N, FooWithAssign());
                    } break;
                case assign_ir: {
                        victim.assign(FooIterator(0), FooIterator(N));
                    } break;
                case op_equ: {
                        victim.reserve(2); victim = src; // fragmented assignment
                    } break;
                case reserve: {
                        try {
                            victim.reserve(victim.max_size()+1);
                        } catch(std::length_error &) {
                        } catch(...) {
                            KNOWN_ISSUE("ERROR: unrecognized exception - known compiler issue\n");
                        }
                        victim.reserve(N);
                    } break;
                case compact: {
                        if(t) MaxFooCount = 0; else allocator_t::set_limits(); // reset limits
                        victim.reserve(2); victim = src; // fragmented assignment
                        if(t) MaxFooCount = FooCount + 10; else allocator_t::set_limits(1, false); // block any allocation, check NULL return from allocator
                        victim.shrink_to_fit(); // should start defragmenting first segment
                    } break;
                case grow: {
                        tbb::task_scheduler_init init(2);
                        if(t) MaxFooCount = FooCount + 31; // these numbers help to reproduce the live lock for versions < TBB2.2
                        try {
                            tbb::parallel_for( tbb::blocked_range<int>(0, N, 70), GrowBy<vector_t>(victim) );
                        } catch(...) {
#if TBB_USE_CAPTURED_EXCEPTION
                            throw tbb::bad_last_alloc();
#else
                            throw;
#endif
                        }
                    } break;
                default:;
                }
                if(!t || m != reserve) ASSERT(false, "should throw an exception");
            } catch(std::bad_alloc &e) {
                allocator_t::set_limits(); MaxFooCount = 0;
                size_t capacity = victim.capacity();
                size_t size = victim.size();
#if TBB_DEPRECATED
                size_t req_size = victim.grow_by(0);
#else
                size_t req_size = victim.grow_by(0) - victim.begin();
#endif
                ASSERT( size <= capacity, NULL);
                ASSERT( req_size >= size, NULL);
                switch(m) {
                case reserve:
                    if(t) ASSERT(false, NULL);
                case assign_nt:
                case assign_ir:
                    if(!t) {
                        ASSERT(capacity < N/2, "unexpected capacity");
                        ASSERT(size == 0, "unexpected size");
                        break;
                    } else {
                        ASSERT(size == N, "unexpected size");
                        ASSERT(capacity >= N, "unexpected capacity");
                        int i;
                        for(i = 1; ; ++i)
                            if(!victim[i].zero_bar()) break;
                            else ASSERT(victim[i].bar() == (m == assign_ir)? i : initial_value_of_bar, NULL);
                        for(; size_t(i) < size; ++i) ASSERT(!victim[i].zero_bar(), NULL);
                        ASSERT(size_t(i) == size, NULL);
                        break;
                    }
                case grow:
                case op_equ:
                    if(!t) {
                        ASSERT(capacity > 0, NULL);
                        ASSERT(capacity < N, "unexpected capacity");
                    }
                    {
                        vector_t copy_of_victim(victim);
                        ASSERT(copy_of_victim.size() > 0, NULL);
                        for(int i = 0; ; ++i) {
                            try {
                                FooWithAssign &foo = victim.at(i);
                                if( !foo.is_valid_or_zero() ) {
                                    std::printf("i: %d size: %u req_size: %u  state: %d\n", i, unsigned(size), unsigned(req_size), foo.state);
                                }
                                int bar = foo.zero_bar();
                                if(m != grow) ASSERT( bar == i || (t && bar == 0), NULL);
                                if(size_t(i) < copy_of_victim.size()) ASSERT( copy_of_victim[i].bar() == bar, NULL);
                            } catch(std::range_error &) { // skip broken segment
                                ASSERT( size_t(i) < req_size, NULL );
                                if(m == op_equ) break;
                            } catch(std::out_of_range &){
                                ASSERT( i > 0, NULL ); break;
                            } catch(...) {
                                KNOWN_ISSUE("ERROR: unrecognized exception - known compiler issue\n"); break;
                            }
                        }
                        vector_t copy_of_victim2(10); copy_of_victim2 = victim;
                        ASSERT(copy_of_victim == copy_of_victim2, "assignment doesn't match copying");
                        if(m == op_equ) {
                            try {
                                victim = copy_of_victim2;
                            } catch(tbb::bad_last_alloc &) { break;
                            } catch(...) {
                                KNOWN_ISSUE("ERROR: unrecognized exception - known compiler issue\n"); break;
                            }
                            ASSERT(t, NULL);
                        }
                    } break;
                case compact:
                    ASSERT(capacity > 0, "unexpected capacity");
                    ASSERT(victim == src, "shrink_to_fit() is broken");
                    break;

                default:; // nothing to check here
                }
                REMARK("Exception %d: %s\t- ok\n", m, e.what());
            }
        }
    } catch(...) {
        ASSERT(false, "unexpected exception");
    }
}
#endif /* TBB_USE_EXCEPTIONS */

//------------------------------------------------------------------------
// Test SSE
//------------------------------------------------------------------------
#include "harness_m128.h"

#if HAVE_m128

void TestSSE() {
    tbb::concurrent_vector<ClassWithSSE> v;
    for( int i=0; i<100; ++i ) {
        v.push_back(ClassWithSSE(i));
        for( int j=0; i<i; ++j ) 
            ASSERT( v[j]==ClassWithSSE(j), NULL );
    }
}
#endif /* HAVE_m128 */

//------------------------------------------------------------------------

int TestMain () {
    if( MinThread<1 ) {
        REPORT("ERROR: MinThread=%d, but must be at least 1\n",MinThread); MinThread = 1;
    }
#if !TBB_DEPRECATED
    TestIteratorTraits<tbb::concurrent_vector<Foo>::iterator,Foo>();
    TestIteratorTraits<tbb::concurrent_vector<Foo>::const_iterator,const Foo>();
    TestSequentialFor<FooWithAssign> ();
    TestResizeAndCopy();
    TestAssign();
#if HAVE_m128
    TestSSE();
#endif /* HAVE_m128 */    
#endif
    TestCapacity();
    ASSERT( !FooCount, NULL );
    for( int nthread=MinThread; nthread<=MaxThread; ++nthread ) {
        tbb::task_scheduler_init init( nthread );
        TestParallelFor( nthread );
        TestConcurrentGrowToAtLeast();
        TestConcurrentGrowBy( nthread );
    }
    ASSERT( !FooCount, NULL );
#if !TBB_DEPRECATED
    TestComparison();
    TestFindPrimes();
    TestSort();
#if __TBB_THROW_ACROSS_MODULE_BOUNDARY_BROKEN
    REPORT("Known issue: exception safety test is skipped.\n");
#elif TBB_USE_EXCEPTIONS
    TestExceptions();
#endif /* TBB_USE_EXCEPTIONS */
#endif /* !TBB_DEPRECATED */
    ASSERT( !FooCount, NULL );
    REMARK("sizeof(concurrent_vector<int>) == %d\n", (int)sizeof(tbb::concurrent_vector<int>));
    return Harness::Done;
}
