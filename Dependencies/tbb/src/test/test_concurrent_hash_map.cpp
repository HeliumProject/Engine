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

#ifndef TBB_USE_PERFORMANCE_WARNINGS
#define TBB_USE_PERFORMANCE_WARNINGS 1
#endif

// Our tests usually include the header under test first.  But this test needs
// to use the preprocessor to edit the identifier runtime_warning in concurrent_hash_map.h.
// Hence we include a few other headers before doing the abusive edit.
#include "tbb/tbb_stddef.h" /* Defines runtime_warning */
#include "harness_assert.h" /* Prerequisite for defining hooked_warning */

// The symbol internal::runtime_warning is normally an entry point into the TBB library.
// Here for sake of testing, we define it to be hooked_warning, a routine peculiar to this unit test.
#define runtime_warning hooked_warning

static bool bad_hashing = false;

namespace tbb { 
    namespace internal {
        static void hooked_warning( const char* /*format*/, ... ) {
            ASSERT(bad_hashing, "unexpected runtime_warning: bad hashing");
        }
    } // namespace internal
} // namespace tbb
#define __TBB_EXTRA_DEBUG 1 // enables additional checks
#include "tbb/concurrent_hash_map.h"

// Restore runtime_warning as an entry point into the TBB library.
#undef runtime_warning

namespace Jungle {
    struct Tiger {};
    size_t tbb_hasher( const Tiger& ) {return 0;}
}

#if !defined(_MSC_VER) || _MSC_VER>=1400 || __INTEL_COMPILER
void test_ADL() {
    tbb::tbb_hash_compare<Jungle::Tiger>::hash(Jungle::Tiger()); // Instantiation chain finds tbb_hasher via Argument Dependent Lookup
}
#endif

struct UserDefinedKeyType {
};

namespace tbb {
    // Test whether tbb_hash_compare can be partially specialized as stated in Reference manual.
    template<> struct tbb_hash_compare<UserDefinedKeyType> {
        size_t hash( UserDefinedKeyType ) const {return 0;}
        bool equal( UserDefinedKeyType /*x*/, UserDefinedKeyType /*y*/ ) {return true;}
    };
}

tbb::concurrent_hash_map<UserDefinedKeyType,int> TestInstantiationWithUserDefinedKeyType;

// Test whether a sufficient set of headers were included to instantiate a concurernt_hash_map. OSS Bug #120 (& #130):
// http://www.threadingbuildingblocks.org/bug_desc.php?id=120
tbb::concurrent_hash_map<std::pair<std::pair<int,std::string>,const char*>,int> TestInstantiation;

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/atomic.h"
#include "tbb/tick_count.h"
#include "harness.h"
#include "harness_allocator.h"

class MyException : public std::bad_alloc {
public:
    virtual const char *what() const throw() { return "out of items limit"; }
    virtual ~MyException() throw() {}
};

/** Has tightly controlled interface so that we can verify
    that concurrent_hash_map uses only the required interface. */
class MyKey {
private:
    void operator=( const MyKey&  );    // Deny access
    int key;
    friend class MyHashCompare;
    friend class YourHashCompare;
public:
    static MyKey make( int i ) {
        MyKey result;
        result.key = i;
        return result;
    }
    int value_of() const {return key;}
};

tbb::atomic<long> MyDataCount;
long MyDataCountLimit = 0;

class MyData {
protected:
    friend class MyData2;
    int data;
    enum state_t {
        LIVE=0x1234,
        DEAD=0x5678
    } my_state;
    void operator=( const MyData& );    // Deny access
public:
    MyData(int i = 0) {
        my_state = LIVE;
        data = i;
        if(MyDataCountLimit && MyDataCount + 1 >= MyDataCountLimit)
            __TBB_THROW( MyException() );
        ++MyDataCount;
    }
    MyData( const MyData& other ) {
        ASSERT( other.my_state==LIVE, NULL );
        my_state = LIVE;
        data = other.data;
        if(MyDataCountLimit && MyDataCount + 1 >= MyDataCountLimit)
            __TBB_THROW( MyException() );
        ++MyDataCount;
    }
    ~MyData() {
        --MyDataCount;
        my_state = DEAD;
    }
    static MyData make( int i ) {   
        MyData result;
        result.data = i;
        return result;
    }
    int value_of() const {
        ASSERT( my_state==LIVE, NULL );
        return data;
    }
    void set_value( int i ) {
        ASSERT( my_state==LIVE, NULL );
        data = i;
    }
    bool operator==( const MyData& other ) const {
        ASSERT( other.my_state==LIVE, NULL );
        ASSERT( my_state==LIVE, NULL );
        return data == other.data;
    }
};

class MyData2 : public MyData {
public:
    MyData2( ) {}
    MyData2( const MyData& other ) {
        ASSERT( other.my_state==LIVE, NULL );
        ASSERT( my_state==LIVE, NULL );
        data = other.data;
    }
    void operator=( const MyData& other ) {
        ASSERT( other.my_state==LIVE, NULL );
        ASSERT( my_state==LIVE, NULL );
        data = other.data;
    }
    void operator=( const MyData2& other ) {
        ASSERT( other.my_state==LIVE, NULL );
        ASSERT( my_state==LIVE, NULL );
        data = other.data;
    }
    bool operator==( const MyData2& other ) const {
        ASSERT( other.my_state==LIVE, NULL );
        ASSERT( my_state==LIVE, NULL );
        return data == other.data;
    }
};

class MyHashCompare {
public:
    bool equal( const MyKey& j, const MyKey& k ) const {
        return j.key==k.key;
    }
    unsigned long hash( const MyKey& k ) const {
        return k.key;
    }   
};

class YourHashCompare {
public:
    bool equal( const MyKey& j, const MyKey& k ) const {
        return j.key==k.key;
    }
    unsigned long hash( const MyKey& ) const {
        return 1;
    }   
};

typedef local_counting_allocator<std::allocator<MyData> > MyAllocator;
typedef tbb::concurrent_hash_map<MyKey,MyData,MyHashCompare,MyAllocator> MyTable;
typedef tbb::concurrent_hash_map<MyKey,MyData2,MyHashCompare> MyTable2;
typedef tbb::concurrent_hash_map<MyKey,MyData,YourHashCompare> YourTable;

template<typename MyTable>
inline void CheckAllocator(MyTable &table, size_t expected_allocs, size_t expected_frees, bool exact = true) {
    size_t items_allocated = table.get_allocator().items_allocated, items_freed = table.get_allocator().items_freed;
    size_t allocations = table.get_allocator().allocations, frees = table.get_allocator().frees;
    REMARK("checking allocators: items %u/%u, allocs %u/%u\n",
            unsigned(items_allocated), unsigned(items_freed), unsigned(allocations), unsigned(frees) );
    ASSERT( items_allocated == allocations, NULL); ASSERT( items_freed == frees, NULL);
    if(exact) {
        ASSERT( allocations == expected_allocs, NULL); ASSERT( frees == expected_frees, NULL);
    } else {
        ASSERT( allocations >= expected_allocs, NULL); ASSERT( frees >= expected_frees, NULL);
        ASSERT( allocations - frees == expected_allocs - expected_frees, NULL );
    }
}

inline bool UseKey( size_t i ) {
    return (i&3)!=3;
}

struct Insert {
    static void apply( MyTable& table, int i ) {
        if( UseKey(i) ) {
            if( i&4 ) {
                MyTable::accessor a;
                table.insert( a, MyKey::make(i) );
                if( i&1 )
                    (*a).second.set_value(i*i);
                else
                    a->second.set_value(i*i);
            } else
                if( i&1 ) {
                    MyTable::accessor a;
                    table.insert( a, std::make_pair(MyKey::make(i), MyData(i*i)) );
                    ASSERT( (*a).second.value_of()==i*i, NULL );
                } else {
                    MyTable::const_accessor ca;
                    table.insert( ca, std::make_pair(MyKey::make(i), MyData(i*i)) );
                    ASSERT( ca->second.value_of()==i*i, NULL );
                }
        }
    }
};

struct Find {
    static void apply( MyTable& table, int i ) {
        MyTable::accessor a;
        const MyTable::accessor& ca = a;
        bool b = table.find( a, MyKey::make(i) );
        ASSERT( b==!a.empty(), NULL );
        if( b ) {
            if( !UseKey(i) )
                REPORT("Line %d: unexpected key %d present\n",__LINE__,i);
            AssertSameType( &*a, static_cast<MyTable::value_type*>(0) );
            ASSERT( ca->second.value_of()==i*i, NULL );
            ASSERT( (*ca).second.value_of()==i*i, NULL );
            if( i&1 )
                ca->second.set_value( ~ca->second.value_of() );
            else
                (*ca).second.set_value( ~ca->second.value_of() );
        } else {
            if( UseKey(i) ) 
                REPORT("Line %d: key %d missing\n",__LINE__,i);
        }
    }
};

struct FindConst {
    static void apply( const MyTable& table, int i ) {
        MyTable::const_accessor a;
        const MyTable::const_accessor& ca = a;
        bool b = table.find( a, MyKey::make(i) );
        ASSERT( b==(table.count(MyKey::make(i))>0), NULL );
        ASSERT( b==!a.empty(), NULL );
        ASSERT( b==UseKey(i), NULL );
        if( b ) {
            AssertSameType( &*ca, static_cast<const MyTable::value_type*>(0) );
            ASSERT( ca->second.value_of()==~(i*i), NULL );
            ASSERT( (*ca).second.value_of()==~(i*i), NULL );
        }
    }
};

tbb::atomic<int> EraseCount;

struct Erase {
    static void apply( MyTable& table, int i ) {
        bool b;
        if(i&4) {
            if(i&8) {
                MyTable::const_accessor a;
                b = table.find( a, MyKey::make(i) ) && table.erase( a );
            } else {
                MyTable::accessor a;
                b = table.find( a, MyKey::make(i) ) && table.erase( a );
            }
        } else
            b = table.erase( MyKey::make(i) );
        if( b ) ++EraseCount;
        ASSERT( table.count(MyKey::make(i)) == 0, NULL );
    }
};

static const int IE_SIZE = 2;
tbb::atomic<YourTable::size_type> InsertEraseCount[IE_SIZE];

struct InsertErase  {
    static void apply( YourTable& table, int i ) {
        if ( i%3 ) {
            int key = i%IE_SIZE;
            if ( table.insert( std::make_pair(MyKey::make(key), MyData2()) ) ) 
                ++InsertEraseCount[key];
        } else {
            int key = i%IE_SIZE;
            if( i&1 ) {
                YourTable::accessor res;
                if(table.find( res, MyKey::make(key) ) && table.erase( res ) )
                    --InsertEraseCount[key];
            } else {
                YourTable::const_accessor res;
                if(table.find( res, MyKey::make(key) ) && table.erase( res ) )
                    --InsertEraseCount[key];
            }
        }
    }
};

// Test for the deadlock discussed at:
// http://softwarecommunity.intel.com/isn/Community/en-US/forums/permalink/30253302/30253302/ShowThread.aspx#30253302
struct InnerInsert {
    static void apply( YourTable& table, int i ) {
        YourTable::accessor a1, a2;
        if(i&1) __TBB_Yield();
        table.insert( a1, MyKey::make(1) );
        __TBB_Yield();
        table.insert( a2, MyKey::make(1 + (1<<30)) ); // the same chain
        table.erase( a2 ); // if erase by key it would lead to deadlock for single thread
    }
};

template<typename Op, typename MyTable>
class TableOperation: NoAssign {
    MyTable& my_table;
public:
    void operator()( const tbb::blocked_range<int>& range ) const {
        for( int i=range.begin(); i!=range.end(); ++i ) 
            Op::apply(my_table,i);
    }
    TableOperation( MyTable& table ) : my_table(table) {}
};

template<typename Op, typename TableType>
void DoConcurrentOperations( TableType& table, int n, const char* what, int nthread ) {
    REMARK("testing %s with %d threads\n",what,nthread);
    tbb::tick_count t0 = tbb::tick_count::now();
    tbb::parallel_for( tbb::blocked_range<int>(0,n,100), TableOperation<Op,TableType>(table) );
    tbb::tick_count t1 = tbb::tick_count::now();
    REMARK("time for %s = %g with %d threads\n",what,(t1-t0).seconds(),nthread);
}

//! Test traversing the table with an iterator.
void TraverseTable( MyTable& table, size_t n, size_t expected_size ) {
    REMARK("testing traversal\n");
    size_t actual_size = table.size();
    ASSERT( actual_size==expected_size, NULL );
    size_t count = 0;
    bool* array = new bool[n];
    memset( array, 0, n*sizeof(bool) );
    const MyTable& const_table = table;
    MyTable::const_iterator ci = const_table.begin();
    for( MyTable::iterator i = table.begin(); i!=table.end(); ++i ) {
        // Check iterator
        int k = i->first.value_of();
        ASSERT( UseKey(k), NULL );
        ASSERT( (*i).first.value_of()==k, NULL );
        ASSERT( 0<=k && size_t(k)<n, "out of bounds key" );
        ASSERT( !array[k], "duplicate key" );
        array[k] = true;
        ++count;

        // Check lower/upper bounds
        std::pair<MyTable::iterator, MyTable::iterator> er = table.equal_range(i->first);
        std::pair<MyTable::const_iterator, MyTable::const_iterator> cer = const_table.equal_range(i->first);
        ASSERT(cer.first == er.first && cer.second == er.second, NULL);
        ASSERT(cer.first == i, NULL);
        ASSERT(std::distance(cer.first, cer.second) == 1, NULL);

        // Check const_iterator
        MyTable::const_iterator cic = ci++;
        ASSERT( cic->first.value_of()==k, NULL );
        ASSERT( (*cic).first.value_of()==k, NULL );
    }
    ASSERT( ci==const_table.end(), NULL );
    delete[] array;
    if( count!=expected_size ) {
        REPORT("Line %d: count=%ld but should be %ld\n",__LINE__,long(count),long(expected_size));
    }
}

typedef tbb::atomic<unsigned char> AtomicByte;

template<typename RangeType>
struct ParallelTraverseBody: NoAssign {
    const size_t n;
    AtomicByte* const array;
    ParallelTraverseBody( AtomicByte array_[], size_t n_ ) : 
        n(n_), 
        array(array_)
    {}
    void operator()( const RangeType& range ) const {
        for( typename RangeType::iterator i = range.begin(); i!=range.end(); ++i ) {
            int k = i->first.value_of();
            ASSERT( 0<=k && size_t(k)<n, NULL ); 
            ++array[k];
        }
    }
};

void Check( AtomicByte array[], size_t n, size_t expected_size ) {
    if( expected_size )
        for( size_t k=0; k<n; ++k ) {
            if( array[k] != int(UseKey(k)) ) {
                REPORT("array[%d]=%d != %d=UseKey(%d)\n",
                       int(k), int(array[k]), int(UseKey(k)), int(k));
                ASSERT(false,NULL);
            }
        }
}

//! Test travering the tabel with a parallel range
void ParallelTraverseTable( MyTable& table, size_t n, size_t expected_size ) {
    REMARK("testing parallel traversal\n");
    ASSERT( table.size()==expected_size, NULL );
    AtomicByte* array = new AtomicByte[n];

    memset( array, 0, n*sizeof(AtomicByte) );
    MyTable::range_type r = table.range(10);
    tbb::parallel_for( r, ParallelTraverseBody<MyTable::range_type>( array, n ));
    Check( array, n, expected_size );

    const MyTable& const_table = table;
    memset( array, 0, n*sizeof(AtomicByte) );
    MyTable::const_range_type cr = const_table.range(10);
    tbb::parallel_for( cr, ParallelTraverseBody<MyTable::const_range_type>( array, n ));
    Check( array, n, expected_size );

    delete[] array;
}

void TestInsertFindErase( int nthread ) {
    int n=250000; 

    // compute m = number of unique keys
    int m = 0;       
    for( int i=0; i<n; ++i )
        m += UseKey(i);
 
    MyAllocator a; a.items_freed = a.frees = 100;
    ASSERT( MyDataCount==0, NULL );
    MyTable table(a);
    TraverseTable(table,n,0);
    ParallelTraverseTable(table,n,0);
    CheckAllocator(table, 0, 100);

    DoConcurrentOperations<Insert,MyTable>(table,n,"insert",nthread);
    ASSERT( MyDataCount==m, NULL );
    TraverseTable(table,n,m);
    ParallelTraverseTable(table,n,m);
    CheckAllocator(table, m, 100);

    DoConcurrentOperations<Find,MyTable>(table,n,"find",nthread);
    ASSERT( MyDataCount==m, NULL );
    CheckAllocator(table, m, 100);

    DoConcurrentOperations<FindConst,MyTable>(table,n,"find(const)",nthread);
    ASSERT( MyDataCount==m, NULL );
    CheckAllocator(table, m, 100);

    EraseCount=0;
    DoConcurrentOperations<Erase,MyTable>(table,n,"erase",nthread);
    ASSERT( EraseCount==m, NULL );
    ASSERT( MyDataCount==0, NULL );
    TraverseTable(table,n,0);
    CheckAllocator(table, m, m+100);

    bad_hashing = true;
    table.clear();
    bad_hashing = false;

    if(nthread > 1) {
        YourTable ie_table;
        for( int i=0; i<IE_SIZE; ++i )
            InsertEraseCount[i] = 0;        
        DoConcurrentOperations<InsertErase,YourTable>(ie_table,n/2,"insert_erase",nthread);
        for( int i=0; i<IE_SIZE; ++i )
            ASSERT( InsertEraseCount[i]==ie_table.count(MyKey::make(i)), NULL );

        DoConcurrentOperations<InnerInsert,YourTable>(ie_table,2000,"inner insert",nthread);
    }
}

volatile int Counter;

class AddToTable: NoAssign {
    MyTable& my_table;
    const int my_nthread;
    const int my_m;
public:
    AddToTable( MyTable& table, int nthread, int m ) : my_table(table), my_nthread(nthread), my_m(m) {}
    void operator()( int ) const {
        for( int i=0; i<my_m; ++i ) {
            // Busy wait to synchronize threads
            int j = 0;
            while( Counter<i ) {
                if( ++j==1000000 ) {
                    // If Counter<i after a million iterations, then we almost surely have
                    // more logical threads than physical threads, and should yield in 
                    // order to let suspended logical threads make progress.
                    j = 0;
                    __TBB_Yield();
                }
            }
            // Now all threads attempt to simultaneously insert a key.
            int k;
            {
                MyTable::accessor a;
                MyKey key = MyKey::make(i);
                if( my_table.insert( a, key ) ) 
                    a->second.set_value( 1 );
                else 
                    a->second.set_value( a->second.value_of()+1 );      
                k = a->second.value_of();
            }
            if( k==my_nthread ) 
                Counter=i+1;
        }
    }
};

class RemoveFromTable: NoAssign {
    MyTable& my_table;
    const int my_nthread;
    const int my_m;
public:
    RemoveFromTable( MyTable& table, int nthread, int m ) : my_table(table), my_nthread(nthread), my_m(m) {}
    void operator()(int) const {
        for( int i=0; i<my_m; ++i ) {
            bool b;
            if(i&4) {
                if(i&8) {
                    MyTable::const_accessor a;
                    b = my_table.find( a, MyKey::make(i) ) && my_table.erase( a );
                } else {
                    MyTable::accessor a;
                    b = my_table.find( a, MyKey::make(i) ) && my_table.erase( a );
                }
            } else
                b = my_table.erase( MyKey::make(i) );
            if( b ) ++EraseCount;
        }
    }
};

//! Test for memory leak in concurrent_hash_map (TR #153).
void TestConcurrency( int nthread ) {
    REMARK("testing multiple insertions/deletions of same key with %d threads\n", nthread);
    {
        ASSERT( MyDataCount==0, NULL );
        MyTable table;
        const int m = 1000;
        Counter = 0;
        tbb::tick_count t0 = tbb::tick_count::now();
        NativeParallelFor( nthread, AddToTable(table,nthread,m) );
        tbb::tick_count t1 = tbb::tick_count::now();
        REMARK("time for %u insertions = %g with %d threads\n",unsigned(MyDataCount),(t1-t0).seconds(),nthread);
        ASSERT( MyDataCount==m, "memory leak detected" );

        EraseCount = 0;
        t0 = tbb::tick_count::now();
        NativeParallelFor( nthread, RemoveFromTable(table,nthread,m) );
        t1 = tbb::tick_count::now();
        REMARK("time for %u deletions = %g with %d threads\n",unsigned(EraseCount),(t1-t0).seconds(),nthread);
        ASSERT( MyDataCount==0, "memory leak detected" );
        ASSERT( EraseCount==m, "return value of erase() is broken" );

        CheckAllocator(table, m, m, /*exact*/nthread <= 1);
    }
    ASSERT( MyDataCount==0, "memory leak detected" );
}

void TestTypes() {
    AssertSameType( static_cast<MyTable::key_type*>(0), static_cast<MyKey*>(0) );
    AssertSameType( static_cast<MyTable::mapped_type*>(0), static_cast<MyData*>(0) );
    AssertSameType( static_cast<MyTable::value_type*>(0), static_cast<std::pair<const MyKey,MyData>*>(0) );
    AssertSameType( static_cast<MyTable::accessor::value_type*>(0), static_cast<MyTable::value_type*>(0) );
    AssertSameType( static_cast<MyTable::const_accessor::value_type*>(0), static_cast<const MyTable::value_type*>(0) );
    AssertSameType( static_cast<MyTable::size_type*>(0), static_cast<size_t*>(0) );
    AssertSameType( static_cast<MyTable::difference_type*>(0), static_cast<ptrdiff_t*>(0) );
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

template<typename Iterator1, typename Iterator2>
void TestIteratorAssignment( Iterator2 j ) {
    Iterator1 i(j), k;
    ASSERT( i==j, NULL ); ASSERT( !(i!=j), NULL );
    k = j;
    ASSERT( k==j, NULL ); ASSERT( !(k!=j), NULL );
}

template<typename Range1, typename Range2>
void TestRangeAssignment( Range2 r2 ) {
    Range1 r1(r2); r1 = r2;
}
//------------------------------------------------------------------------
// Test for copy constructor and assignment
//------------------------------------------------------------------------

template<typename MyTable>
static void FillTable( MyTable& x, int n ) {
    for( int i=1; i<=n; ++i ) {
        MyKey key( MyKey::make(-i) ); // hash values must not be specified in direct order
        typename MyTable::accessor a;
        bool b = x.insert(a,key); 
        ASSERT(b, NULL);
        a->second.set_value( i*i );
    }
}

template<typename MyTable>
static void CheckTable( const MyTable& x, int n ) {
    ASSERT( x.size()==size_t(n), "table is different size than expected" );
    ASSERT( x.empty()==(n==0), NULL );
    ASSERT( x.size()<=x.max_size(), NULL );
    for( int i=1; i<=n; ++i ) {
        MyKey key( MyKey::make(-i) );
        typename MyTable::const_accessor a;
        bool b = x.find(a,key); 
        ASSERT( b, NULL ); 
        ASSERT( a->second.value_of()==i*i, NULL );
    }
    int count = 0;
    int key_sum = 0;
    for( typename MyTable::const_iterator i(x.begin()); i!=x.end(); ++i ) {
        ++count;
        key_sum += -i->first.value_of();
    }
    ASSERT( count==n, NULL );
    ASSERT( key_sum==n*(n+1)/2, NULL );
}

static void TestCopy() {
    REMARK("testing copy\n");
    MyTable t1;
    for( int i=0; i<10000; i=(i<100 ? i+1 : i*3) ) {
        MyDataCount = 0;

        FillTable(t1,i);
        // Do not call CheckTable(t1,i) before copying, it enforces rehashing

        MyTable t2(t1);
        // Check that copy constructor did not mangle source table.
        CheckTable(t1,i);
        swap(t1, t2);
        CheckTable(t1,i);
        ASSERT( !(t1 != t2), NULL );

        // Clear original table
        t2.clear();
        swap(t2, t1);
        CheckTable(t1,0);

        // Verify that copy of t1 is correct, even after t1 is cleared.
        CheckTable(t2,i);
        t2.clear();
        t1.swap( t2 );
        CheckTable(t1,0);
        CheckTable(t2,0);
        ASSERT( MyDataCount==0, "data leak?" );
    }
}

void TestAssignment() {
    REMARK("testing assignment\n");
    for( int i=0; i<1000; i=(i<30 ? i+1 : i*5) ) {
        for( int j=0; j<1000; j=(j<30 ? j+1 : j*7) ) {
            MyTable t1;
            MyTable t2;
            FillTable(t1,i);
            FillTable(t2,j);
            ASSERT( (t1 == t2) == (i == j), NULL );
            CheckTable(t2,j);

            MyTable& tref = t2=t1; 
            ASSERT( &tref==&t2, NULL );
            ASSERT( t1 == t2, NULL );
            CheckTable(t1,i);
            CheckTable(t2,i);

            t1.clear();
            CheckTable(t1,0);
            CheckTable(t2,i);
            ASSERT( MyDataCount==i, "data leak?" );

            t2.clear();
            CheckTable(t1,0);
            CheckTable(t2,0);
            ASSERT( MyDataCount==0, "data leak?" );
        }
    }
}

void TestIteratorsAndRanges() {
    REMARK("testing iterators compliance\n");
    TestIteratorTraits<MyTable::iterator,MyTable::value_type>();
    TestIteratorTraits<MyTable::const_iterator,const MyTable::value_type>();

    MyTable v;
    MyTable const &u = v;

    TestIteratorAssignment<MyTable::const_iterator>( u.begin() );
    TestIteratorAssignment<MyTable::const_iterator>( v.begin() );
    TestIteratorAssignment<MyTable::iterator>( v.begin() );
    // doesn't compile as expected: TestIteratorAssignment<typename V::iterator>( u.begin() );

    // check for non-existing 
    ASSERT(v.equal_range(MyKey::make(-1)) == std::make_pair(v.end(), v.end()), NULL);
    ASSERT(u.equal_range(MyKey::make(-1)) == std::make_pair(u.end(), u.end()), NULL);

    REMARK("testing ranges compliance\n");
    TestRangeAssignment<MyTable::const_range_type>( u.range() );
    TestRangeAssignment<MyTable::const_range_type>( v.range() );
    TestRangeAssignment<MyTable::range_type>( v.range() );
    // doesn't compile as expected: TestRangeAssignment<typename V::range_type>( u.range() );

    REMARK("testing construction and insertion from iterators range\n");
    FillTable( v, 1000 );
    MyTable2 t(v.begin(), v.end());
    v.rehash();
    CheckTable(t, 1000);
    t.insert(v.begin(), v.end()); // do nothing
    CheckTable(t, 1000);
    t.clear();
    t.insert(v.begin(), v.end()); // restore
    CheckTable(t, 1000);

    REMARK("testing comparison\n");
    typedef tbb::concurrent_hash_map<MyKey,MyData2,YourHashCompare,MyAllocator> YourTable1;
    typedef tbb::concurrent_hash_map<MyKey,MyData2,YourHashCompare> YourTable2;
    YourTable1 t1;
    FillTable( t1, 10 );
    CheckTable(t1, 10 );
    YourTable2 t2(t1.begin(), t1.end());
    MyKey key( MyKey::make(-5) ); MyData2 data;
    ASSERT(t2.erase(key), NULL);
    YourTable2::accessor a;
    ASSERT(t2.insert(a, key), NULL);
    data.set_value(0);   a->second = data;
    ASSERT( t1 != t2, NULL);
    data.set_value(5*5); a->second = data;
    ASSERT( t1 == t2, NULL);
}

void TestRehash() {
    REMARK("testing rehashing\n");
    MyTable w;
    w.insert( std::make_pair(MyKey::make(-5), MyData()) );
    w.rehash(); // without this, assertion will fail
    MyTable::iterator it = w.begin();
    int i = 0; // check for non-rehashed buckets
    for( ; it != w.end(); i++ )
        w.count( (it++)->first );
    ASSERT( i == 1, NULL );
    for( i=0; i<1000; i=(i<29 ? i+1 : i*2) ) {
        for( int j=max(256+i, i*2); j<10000; j*=3 ) {
            MyTable v;
            FillTable( v, i );
            ASSERT(int(v.size()) == i, NULL);
            ASSERT(int(v.bucket_count()) <= j, NULL);
            v.rehash( j );
            ASSERT(int(v.bucket_count()) >= j, NULL);
            CheckTable( v, i );
        }
    }
}

#if TBB_USE_EXCEPTIONS
void TestExceptions() {
    typedef local_counting_allocator<tbb::tbb_allocator<MyData2> > allocator_t;
    typedef tbb::concurrent_hash_map<MyKey,MyData2,MyHashCompare,allocator_t> ThrowingTable;
    enum methods {
        zero_method = 0,
        ctor_copy, op_assign, op_insert,
        all_methods
    };
    REMARK("testing exception-safety guarantees\n");
    ThrowingTable src;
    FillTable( src, 1000 );
    ASSERT( MyDataCount==1000, NULL );

    try {
        for(int t = 0; t < 2; t++) // exception type
        for(int m = zero_method+1; m < all_methods; m++)
        {
            allocator_t a;
            if(t) MyDataCountLimit = 101;
            else a.set_limits(101);
            ThrowingTable victim(a);
            MyDataCount = 0;

            try {
                switch(m) {
                case ctor_copy: {
                        ThrowingTable acopy(src, a);
                    } break;
                case op_assign: {
                        victim = src;
                    } break;
                case op_insert: {
                        FillTable( victim, 1000 );
                    } break;
                default:;
                }
                ASSERT(false, "should throw an exception");
            } catch(std::bad_alloc &e) {
                MyDataCountLimit = 0;
                size_t size = victim.size();
                switch(m) {
                case op_assign:
                    ASSERT( MyDataCount==100, "data leak?" );
                    ASSERT( size>=100, NULL );
                    CheckAllocator(victim, 100+t, t);
                case ctor_copy:
                    CheckTable(src, 1000);
                    break;
                case op_insert:
                    ASSERT( size==size_t(100-t), NULL );
                    ASSERT( MyDataCount==100-t, "data leak?" );
                    CheckTable(victim, 100-t);
                    CheckAllocator(victim, 100, t);
                    break;

                default:; // nothing to check here
                }
                REMARK("Exception %d: %s\t- ok ()\n", m, e.what());
            }
            catch ( ... ) {
                ASSERT ( __TBB_EXCEPTION_TYPE_INFO_BROKEN, "Unrecognized exception" );
            }
        }
    } catch(...) {
        ASSERT(false, "unexpected exception");
    }
    src.clear(); MyDataCount = 0;
}
#endif /* TBB_USE_EXCEPTIONS */

//------------------------------------------------------------------------
// Test driver
//------------------------------------------------------------------------

#include "tbb/task_scheduler_init.h"

int TestMain () {
    if( MinThread<0 ) {
        REPORT("ERROR: must use at least one thread\n");
        exit(1);
    }

    // Do serial tests
    TestTypes();
    TestCopy();
    TestRehash();
    TestAssignment();
    TestIteratorsAndRanges();
#if TBB_USE_EXCEPTIONS
    TestExceptions();
#endif /* TBB_USE_EXCEPTIONS */

    // Do concurrency tests.
    for( int nthread=MinThread; nthread<=MaxThread; ++nthread ) {
        tbb::task_scheduler_init init( nthread );
        TestInsertFindErase( nthread );
        TestConcurrency( nthread );
    }
    // check linking
    if(bad_hashing) { //should be false
        tbb::internal::runtime_warning("none\nERROR: it must not be executed");
    }

    return Harness::Done;
}
