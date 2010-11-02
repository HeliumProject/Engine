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

/* Some tests in this source file are based on PPL tests provided by Microsoft. */

#define __TBB_EXTRA_DEBUG 1
#include "tbb/concurrent_unordered_map.h"
#include "tbb/parallel_for.h"
#include "tbb/tick_count.h"
#include <stdio.h>
#include "harness.h"
#include "harness_allocator.h"

using namespace std;

typedef local_counting_allocator<debug_allocator<std::pair<const int,int>,std::allocator> > MyAllocator;
typedef tbb::concurrent_unordered_map<int, int, tbb::tbb_hash<int>, std::equal_to<int>, MyAllocator> Mycumap;
//typedef tbb::concurrent_unordered_map<int, int> Mycumap;
//typedef concurrent_unordered_multimap<int, int> Mycummap;

#define CheckAllocatorE(t,a,f) CheckAllocator(t,a,f,true,__LINE__)
#define CheckAllocatorA(t,a,f) CheckAllocator(t,a,f,false,__LINE__)
template<typename MyTable>
inline void CheckAllocator(MyTable &table, size_t expected_allocs, size_t expected_frees, bool exact = true, int line = 0) {
    typename MyTable::allocator_type a = table.get_allocator();
    REMARK("#%d checking allocators: items %u/%u, allocs %u/%u\n", line,
        unsigned(a.items_allocated), unsigned(a.items_freed), unsigned(a.allocations), unsigned(a.frees) );
    ASSERT( a.items_allocated == a.allocations, NULL); ASSERT( a.items_freed == a.frees, NULL);
    if(exact) {
        ASSERT( a.allocations == expected_allocs, NULL); ASSERT( a.frees == expected_frees, NULL);
    } else {
        ASSERT( a.allocations >= expected_allocs, NULL); ASSERT( a.frees >= expected_frees, NULL);
        ASSERT( a.allocations - a.frees == expected_allocs - expected_frees, NULL );
    }
}

template <typename K, typename V = std::pair<const K, K> >
struct ValueFactory {
    static V make(const K &value) { return V(value, value); }
    static K get(const V& value) { return value.second; }
};

template <typename T>
struct ValueFactory<T, T> {
    static T make(const T &value) { return value; }
    static T get(const T &value) { return value; }
};

template <typename T>
struct Value : ValueFactory<typename T::key_type, typename T::value_type> {};

#if _MSC_VER
#pragma warning(disable: 4189) // warning 4189 -- local variable is initialized but not referenced
#pragma warning(disable: 4127) // warning 4127 -- while (true) has a constant expression in it
#endif

template<typename Iterator, typename RangeType>
std::pair<int,int> CheckRecursiveRange(RangeType range) {
    std::pair<int,int> sum(0, 0); // count, sum
    for( Iterator i = range.begin(), e = range.end(); i != e; ++i ) {
        ++sum.first; sum.second += i->second;
    }
    if( range.is_divisible() ) {
        RangeType range2( range, tbb::split() );
        std::pair<int,int> sum1 = CheckRecursiveRange<Iterator, RangeType>( range );
        std::pair<int,int> sum2 = CheckRecursiveRange<Iterator, RangeType>( range2 );
        sum1.first += sum2.first; sum1.second += sum2.second;
        ASSERT( sum == sum1, "Mismatched ranges after division");
    }
    return sum;
}

template <typename T>
struct SpecialTests {
    static void Test() {}
};

template <>
struct SpecialTests <Mycumap>
{
    static void Test()
    {
        Mycumap cont(0);
        const Mycumap &ccont(cont);

        // mapped_type& operator[](const key_type& k);
        cont[1] = 2;

        // bool empty() const;    
        ASSERT(!ccont.empty(), "Concurrent container empty after adding an element");

        // size_type size() const;
        ASSERT(ccont.size() == 1, "Concurrent container size incorrect");

        ASSERT(cont[1] == 2, "Concurrent container size incorrect");

        // mapped_type& at( const key_type& k );
        // const mapped_type& at(const key_type& k) const;
        ASSERT(cont.at(1) == 2, "Concurrent container size incorrect");
        ASSERT(ccont.at(1) == 2, "Concurrent container size incorrect");

        // iterator find(const key_type& k);
        Mycumap::const_iterator it = cont.find(1);
        ASSERT(it != cont.end() && Value<Mycumap>::get(*(it)) == 2, "Element with key 1 not properly found");

        REMARK("passed -- specialized concurrent unordered map tests\n");
    }
};

template<typename T>
void test_basic(const char * str)
{
    T cont;
    const T &ccont(cont);

    // bool empty() const;
    ASSERT(ccont.empty(), "Concurrent container not empty after construction");

    // size_type size() const;
    ASSERT(ccont.size() == 0, "Concurrent container not empty after construction");

    // size_type max_size() const;
    ASSERT(ccont.max_size() > 0, "Concurrent container max size invalid");

    //iterator begin();
    //iterator end();
    ASSERT(cont.begin() == cont.end(), "Concurrent container iterators invalid after construction");
    ASSERT(ccont.begin() == ccont.end(), "Concurrent container iterators invalid after construction");
    ASSERT(cont.cbegin() == cont.cend(), "Concurrent container iterators invalid after construction");

    //std::pair<iterator, bool> insert(const value_type& obj);
    std::pair<typename T::iterator, bool> ins = cont.insert(Value<T>::make(1));
    ASSERT(ins.second == true && Value<T>::get(*(ins.first)) == 1, "Element 1 not properly inserted");

    // bool empty() const;
    ASSERT(!ccont.empty(), "Concurrent container empty after adding an element");

    // size_type size() const;
    ASSERT(ccont.size() == 1, "Concurrent container size incorrect");

    std::pair<typename T::iterator, bool> ins2 = cont.insert(Value<T>::make(1));

    if (T::allow_multimapping)
    {
        // std::pair<iterator, bool> insert(const value_type& obj);
        ASSERT(ins2.second == true && Value<T>::get(*(ins2.first)) == 1, "Element 1 not properly inserted");

        // size_type size() const;
        ASSERT(ccont.size() == 2, "Concurrent container size incorrect");

        // size_type count(const key_type& k) const;
        ASSERT(ccont.count(1) == 2, "Concurrent container count(1) incorrect");

        // std::pair<iterator, iterator> equal_range(const key_type& k);
        std::pair<typename T::iterator, typename T::iterator> range = cont.equal_range(1);
        typename T::iterator it = range.first;
        ASSERT(it != cont.end() && Value<T>::get(*it) == 1, "Element 1 not properly found");
        unsigned int count = 0;
        for (; it != range.second; it++)
        {
            count++;
            ASSERT(Value<T>::get(*it) == 1, "Element 1 not properly found");
        }

        ASSERT(count == 2, "Range doesn't have the right number of elements");
    }
    else
    {
        // std::pair<iterator, bool> insert(const value_type& obj);
        ASSERT(ins2.second == false && ins2.first == ins.first, "Element 1 should not be re-inserted");

        // size_type size() const;
        ASSERT(ccont.size() == 1, "Concurrent container size incorrect");

        // size_type count(const key_type& k) const;
        ASSERT(ccont.count(1) == 1, "Concurrent container count(1) incorrect");

        // std::pair<const_iterator, const_iterator> equal_range(const key_type& k) const;
        // std::pair<iterator, iterator> equal_range(const key_type& k);
        std::pair<typename T::iterator, typename T::iterator> range = cont.equal_range(1);
        typename T::iterator i = range.first;
        ASSERT(i != cont.end() && Value<T>::get(*i) == 1, "Element 1 not properly found");
        ASSERT(++i == range.second, "Range doesn't have the right number of elements");
    }

    // const_iterator find(const key_type& k) const;
    // iterator find(const key_type& k);
    typename T::iterator it = cont.find(1);
    ASSERT(it != cont.end() && Value<T>::get(*(it)) == 1, "Element 1 not properly found");
    ASSERT(ccont.find(1) == it, "Element 1 not properly found");

    // iterator insert(const_iterator hint, const value_type& obj);
    typename T::iterator it2 = cont.insert(ins.first, Value<T>::make(2));
    ASSERT(Value<T>::get(*it2) == 2, "Element 2 not properly inserted");

    // T(const T& _Umap)
    T newcont = ccont;
    ASSERT(T::allow_multimapping ? (newcont.size() == 3) : (newcont.size() == 2), "Copy construction did not copy the elements properly");

    // size_type unsafe_erase(const key_type& k);
    typename T::size_type size = cont.unsafe_erase(1);
    ASSERT(T::allow_multimapping ? (size == 2) : (size == 1), "Erase did not remove the right number of elements");

    // iterator unsafe_erase(const_iterator position);
    typename T::iterator it4 = cont.unsafe_erase(cont.find(2));
    ASSERT(it4 == cont.end() && cont.size() == 0, "Erase did not remove the last element properly");

    // template<class InputIterator> void insert(InputIterator first, InputIterator last);
    cont.insert(newcont.begin(), newcont.end());
    ASSERT(T::allow_multimapping ? (cont.size() == 3) : (cont.size() == 2), "Range insert did not copy the elements properly");

    // iterator unsafe_erase(const_iterator first, const_iterator last);
    std::pair<typename T::iterator, typename T::iterator> range2 = newcont.equal_range(1);
    newcont.unsafe_erase(range2.first, range2.second);
    ASSERT(newcont.size() == 1, "Range erase did not erase the elements properly");

    // void clear();
    newcont.clear();
    ASSERT(newcont.begin() == newcont.end() && newcont.size() == 0, "Clear did not clear the container");

    // T& operator=(const T& _Umap)
    newcont = ccont;
    ASSERT(T::allow_multimapping ? (newcont.size() == 3) : (newcont.size() == 2), "Assignment operator did not copy the elements properly");

    // void rehash(size_type n);
    newcont.rehash(16);
    ASSERT(T::allow_multimapping ? (newcont.size() == 3) : (newcont.size() == 2), "Rehash should not affect the container elements");

    // float load_factor() const;
    // float max_load_factor() const;
    ASSERT(ccont.load_factor() <= ccont.max_load_factor(), "Load factor invalid");

    // void max_load_factor(float z);
    cont.max_load_factor(16.0f);
    ASSERT(ccont.max_load_factor() == 16.0f, "Max load factor not properly changed");

    // hasher hash_function() const;
    ccont.hash_function();

    // key_equal key_eq() const;
    ccont.key_eq();

    cont.clear();
    CheckAllocatorA(cont, 1, 0); // one dummy is always allocated
    for (int i = 0; i < 256; i++)
    {
        std::pair<typename T::iterator, bool> ins3 = cont.insert(Value<T>::make(i));
        ASSERT(ins3.second == true && Value<T>::get(*(ins3.first)) == i, "Element 1 not properly inserted");
    }
    ASSERT(cont.size() == 256, "Wrong number of elements inserted");
    ASSERT(256 == CheckRecursiveRange<typename T::iterator>(cont.range()).first, NULL);
    ASSERT(256 == CheckRecursiveRange<typename T::const_iterator>(ccont.range()).first, NULL);

    // size_type unsafe_bucket_count() const;
    ASSERT(ccont.unsafe_bucket_count() == 16, "Wrong number of buckets");

    // size_type unsafe_max_bucket_count() const;
    ASSERT(ccont.unsafe_max_bucket_count() > 65536, "Wrong max number of buckets");

    for (unsigned int i = 0; i < 256; i++)
    {
        typename T::size_type buck = ccont.unsafe_bucket(i);

        // size_type unsafe_bucket(const key_type& k) const;
        ASSERT(buck < 16, "Wrong bucket mapping");
    }

    for (unsigned int i = 0; i < 16; i++)
    {
        // size_type unsafe_bucket_size(size_type n);
        ASSERT(cont.unsafe_bucket_size(i) == 16, "Wrong number elements in a bucket");

        // local_iterator unsafe_begin(size_type n);
        // const_local_iterator unsafe_begin(size_type n) const;
        // local_iterator unsafe_end(size_type n);
        // const_local_iterator unsafe_end(size_type n) const;
        // const_local_iterator unsafe_cbegin(size_type n) const;
        // const_local_iterator unsafe_cend(size_type n) const;
        unsigned int count = 0;
        for (typename T::iterator bit = cont.unsafe_begin(i); bit != cont.unsafe_end(i); bit++)
        {
            count++;
        }
        ASSERT(count == 16, "Bucket iterators are invalid");
    }

    // void swap(T&);
    cont.swap(newcont);
    ASSERT(newcont.size() == 256, "Wrong number of elements after swap");
    ASSERT(newcont.count(200) == 1, "Element with key 200 not present after swap");
    ASSERT(newcont.count(16) == 1, "Element with key 16 not present after swap");
    ASSERT(newcont.count(99) == 1, "Element with key 99 not present after swap");
    ASSERT(T::allow_multimapping ? (cont.size() == 3) : (cont.size() == 2), "Wrong number of elements after swap");

    REMARK("passed -- basic %S tests\n", str);

#if defined (VERBOSE)
    REMARK("container dump debug:\n");
    cont._Dump();
    REMARK("container dump release:\n");
    cont.dump();
    REMARK("\n");
#endif

    SpecialTests<T>::Test();
}

void test_machine() {
    ASSERT(__TBB_ReverseByte(0)==0, NULL );
    ASSERT(__TBB_ReverseByte(1)==0x80, NULL );
    ASSERT(__TBB_ReverseByte(0xFE)==0x7F, NULL );
    ASSERT(__TBB_ReverseByte(0xFF)==0xFF, NULL );
}

template<typename T>
class FillTable: NoAssign {
    T &table;
    const int items;
    typedef std::pair<typename T::iterator, bool> pairIB;
public:
    FillTable(T &t, int i) : table(t), items(i) {
        ASSERT( !(items&1) && items > 100, NULL);
    }
    void operator()(int threadn) const {
        if( threadn == 0 ) { // Fill even keys forward (single thread)
            bool last_inserted = true;
            for( int i = 0; i < items; i+=2 ) {
                pairIB pib = table.insert(Value<T>::make(i));
                ASSERT(Value<T>::get(*(pib.first)) == i, "Element not properly inserted");
                ASSERT( last_inserted || !pib.second, "Previous key was not inserted but this is inserted" );
                last_inserted = pib.second;
            }
        } else if( threadn == 1 ) { // Fill even keys backward (single thread)
            bool last_inserted = true;
            for( int i = items-2; i >= 0; i-=2 ) {
                pairIB pib = table.insert(Value<T>::make(i));
                ASSERT(Value<T>::get(*(pib.first)) == i, "Element not properly inserted");
                ASSERT( last_inserted || !pib.second, "Previous key was not inserted but this is inserted" );
                last_inserted = pib.second;
            }
        } else if( !(threadn&1) ) { // Fill odd keys forward (multiple threads)
            for( int i = 1; i < items; i+=2 ) {
                pairIB pib = table.insert(Value<T>::make(i));
                ASSERT(Value<T>::get(*(pib.first)) == i, "Element not properly inserted");
            }
        } else { // Check odd keys backward (multiple threads)
            bool last_found = false;
            for( int i = items-1; i >= 0; i-=2 ) {
                typename T::iterator it = table.find(i);
                if( it != table.end() ) { // found
                    ASSERT(Value<T>::get(*it) == i, "Element not properly inserted");
                    last_found = true;
                } else ASSERT( !last_found, "Previous key was found but this is not" );
            }
        }
    }
};

typedef tbb::atomic<unsigned char> AtomicByte;

template<typename RangeType>
struct ParallelTraverseBody: NoAssign {
    const int n;
    AtomicByte* const array;
    ParallelTraverseBody( AtomicByte an_array[], int a_n ) : 
        n(a_n), array(an_array)
    {}
    void operator()( const RangeType& range ) const {
        for( typename RangeType::iterator i = range.begin(); i!=range.end(); ++i ) {
            int k = i->first;
            ASSERT( k == i->second, NULL );
            ASSERT( 0<=k && k<n, NULL ); 
            array[k]++;
        }
    }
};

void CheckRange( AtomicByte array[], int n ) {
    for( int k=0; k<n; ++k ) {
        if( array[k] != 1 ) {
            REPORT("array[%d]=%d\n", k, int(array[k]));
            ASSERT(false,NULL);
        }
    }
}

template<typename T>
class CheckTable: NoAssign {
    T &table;
public:
    CheckTable(T &t) : NoAssign(), table(t) {}
    void operator()(int i) const {
        int c = (int)table.count( i );
        ASSERT( c, "must exist" );
    }
};

template<typename T>
void test_concurrent(const char *tablename) {
#if TBB_USE_ASSERT
    int items = 2000;
#else
    int items = 100000;
#endif
    T table(items/1000);
    tbb::tick_count t0 = tbb::tick_count::now();
    NativeParallelFor( 16/*min 6*/, FillTable<T>(table, items) );
    tbb::tick_count t1 = tbb::tick_count::now();
    REMARK( "time for filling '%s' by %d items = %g\n", tablename, items, (t1-t0).seconds() );
    ASSERT( int(table.size()) == items, NULL);

    AtomicByte* array = new AtomicByte[items];
    memset( array, 0, items*sizeof(AtomicByte) );

    typename T::range_type r = table.range();
    ASSERT(items == CheckRecursiveRange<typename T::iterator>(r).first, NULL);
    tbb::parallel_for( r, ParallelTraverseBody<typename T::const_range_type>( array, items ));
    CheckRange( array, items );

    const T &const_table = table;
    memset( array, 0, items*sizeof(AtomicByte) );
    typename T::const_range_type cr = const_table.range();
    ASSERT(items == CheckRecursiveRange<typename T::const_iterator>(cr).first, NULL);
    tbb::parallel_for( cr, ParallelTraverseBody<typename T::const_range_type>( array, items ));
    CheckRange( array, items );
    delete[] array;

    tbb::parallel_for( 0, items, CheckTable<T>( table ) );
  
    table.clear();
    CheckAllocatorA(table, items+1, items); // one dummy is always allocated
}

int TestMain () {
    test_machine();
    test_basic<Mycumap>("concurrent unordered map");
    test_concurrent<Mycumap>("concurrent unordered map");
    return Harness::Done;
}
