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

#include "tbb/blocked_range.h"
#include "harness_assert.h"

// First test as much as we can without including other headers.
// Doing so should catch problems arising from failing to include headers.

class AbstractValueType {
    AbstractValueType() {}
    int value;
public: 
    friend AbstractValueType MakeAbstractValueType( int i );
    friend int GetValueOf( const AbstractValueType& v ) {return v.value;}
};

AbstractValueType MakeAbstractValueType( int i ) {
    AbstractValueType x;
    x.value = i;
    return x;
}

std::size_t operator-( const AbstractValueType& u, const AbstractValueType& v ) {
    return GetValueOf(u)-GetValueOf(v);
}

bool operator<( const AbstractValueType& u, const AbstractValueType& v ) {
    return GetValueOf(u)<GetValueOf(v);
}

AbstractValueType operator+( const AbstractValueType& u, std::size_t offset ) {
    return MakeAbstractValueType(GetValueOf(u)+int(offset));
}

static void SerialTest() {
    for( int x=-10; x<10; ++x )
        for( int y=-10; y<10; ++y ) {
            AbstractValueType i = MakeAbstractValueType(x);
            AbstractValueType j = MakeAbstractValueType(y);
            for( std::size_t k=1; k<10; ++k ) {
                typedef tbb::blocked_range<AbstractValueType> range_type;
                range_type r( i, j, k );
                AssertSameType( r.empty(), true );
                AssertSameType( range_type::size_type(), std::size_t() );
                AssertSameType( static_cast<range_type::const_iterator*>(0), static_cast<AbstractValueType*>(0) );
                AssertSameType( r.begin(), MakeAbstractValueType(0) );
                AssertSameType( r.end(), MakeAbstractValueType(0) );
                ASSERT( r.empty()==(y<=x), NULL );
                ASSERT( r.grainsize()==k, NULL );
                if( x<=y ) {
                    AssertSameType( r.is_divisible(), true );
                    ASSERT( r.is_divisible()==(std::size_t(y-x)>k), NULL );
                    ASSERT( r.size()==std::size_t(y-x), NULL );
                    if( r.is_divisible() ) {
                        tbb::blocked_range<AbstractValueType> r2(r,tbb::split());
                        ASSERT( GetValueOf(r.begin())==x, NULL );
                        ASSERT( GetValueOf(r.end())==GetValueOf(r2.begin()), NULL );
                        ASSERT( GetValueOf(r2.end())==y, NULL );
                        ASSERT( r.grainsize()==k, NULL );
                        ASSERT( r2.grainsize()==k, NULL );
                    }
                }
            }
        }
}

#include "tbb/parallel_for.h"
#include "harness.h"

const int N = 1<<22;

unsigned char Array[N];

struct Striker {
    // Note: we use <int> here instead of <long> in order to test for Quad 407676
    void operator()( const tbb::blocked_range<int>& r ) const {
        for( tbb::blocked_range<int>::const_iterator i=r.begin(); i!=r.end(); ++i )
            ++Array[i];
    }
};

void ParallelTest() {
    for( int i=0; i<N; i=i<3 ? i+1 : i*3 ) {
        const tbb::blocked_range<int> r( 0, i, 10 );
        tbb::parallel_for( r, Striker() );
        for( int k=0; k<N; ++k ) {
            ASSERT( Array[k]==(k<i), NULL );
            Array[k] = 0;
        }
    }
}

#include "tbb/task_scheduler_init.h"

int TestMain () {
    SerialTest();
    for( int p=MinThread; p<=MaxThread; ++p ) {
        tbb::task_scheduler_init init(p);
        ParallelTest();
    }
    return Harness::Done;
}
