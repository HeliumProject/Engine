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

// Tests for compatibility with the host's STL.

#include "harness.h"

template<typename Container>
void TestSequence() {
    Container c;
    for( int i=0; i<1000; ++i )
        c.push_back(i*i);    
    typename Container::const_iterator p = c.begin();
    for( int i=0; i<1000; ++i ) {
        ASSERT( *p==i*i, NULL );
        ++p;
    }
}

template<typename Set>
void TestSet() {
    Set s;
    typedef typename Set::value_type value_type;
    for( int i=0; i<100; ++i ) 
        s.insert(value_type(3*i));
    for( int i=0; i<300; ++i ) {
        ASSERT( s.erase(i)==size_t(i%3==0), NULL );
    }
}

template<typename Map>
void TestMap() {
    Map m;
    typedef typename Map::value_type value_type;
    for( int i=0; i<100; ++i ) 
        m.insert(value_type(i,i*i));
    for( int i=0; i<100; ++i )
        ASSERT( m.find(i)->second==i*i, NULL );
}

#if !TBB_USE_EXCEPTIONS && _MSC_VER
    // Suppress "C++ exception handler used, but unwind semantics are not enabled" warning in STL headers
    #pragma warning (push)
    #pragma warning (disable: 4530)
#endif

#include <deque>
#include <list>
#include <map>
#include <set>
#include <vector>

#if !TBB_USE_EXCEPTIONS && _MSC_VER
    #pragma warning (pop)
#endif

template<typename Allocator>
void TestAllocatorWithSTL() {
    typedef typename Allocator::template rebind<int>::other Ai;
    typedef typename Allocator::template rebind<const int>::other Aci;
    typedef typename Allocator::template rebind<std::pair<const int, int> >::other Acii;
    typedef typename Allocator::template rebind<std::pair<int, int> >::other Aii;

    // Sequenced containers
    TestSequence<std::deque <int,Ai> >();
    TestSequence<std::list  <int,Ai> >();
    TestSequence<std::vector<int,Ai> >();

    // Associative containers
    TestSet<std::set     <int, std::less<int>, Ai> >();
    TestSet<std::multiset<int, std::less<int>, Ai> >();
    TestMap<std::map     <int, int, std::less<int>, Acii> >();
    TestMap<std::multimap<int, int, std::less<int>, Acii> >();

#if _MSC_VER
    // Test compatibility with Microsoft's implementation of std::allocator for some cases that
    // are undefined according to the ISO standard but permitted by Microsoft.
    TestSequence<std::deque <const int,Aci> >();
#if _CPPLIB_VER>=500
    TestSequence<std::list  <const int,Aci> >();
#endif
    TestSequence<std::vector<const int,Aci> >();
    TestSet<std::set<const int, std::less<int>, Aci> >();
    TestMap<std::map<int, int, std::less<int>, Aii> >();
    TestMap<std::map<const int, int, std::less<int>, Acii> >();
    TestMap<std::multimap<int, int, std::less<int>, Aii> >();
    TestMap<std::multimap<const int, int, std::less<int>, Acii> >();
#endif /* _MSC_VER */
}
