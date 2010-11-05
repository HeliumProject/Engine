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

// Header that sets HAVE_m128 if we have type __m128

#if (__SSE__||_M_IX86) && !defined(__sun)
#include <xmmintrin.h>
#define HAVE_m128 1

//! Class for testing safety of using __m128
/** Uses circuitous logic forces compiler to put __m128 objects on stack while
    executing various methods, and thus tempt it to use aligned loads and stores
    on the stack. */
//  Do not create file-scope objects of the class, because MinGW (as of May 2010)
//  did not always provide proper stack alignment in destructors of such objects.
class ClassWithSSE {
    static const int n = 16;
    __m128 field[n];
    void init( int start );
public:
    ClassWithSSE() {init(-n);} 
    ClassWithSSE( int i ) {init(i);}
    void operator=( const ClassWithSSE& src ) {
        __m128 stack[n];
        for( int i=0; i<n; ++i )
            stack[i^5] = src.field[i];
        for( int i=0; i<n; ++i )
            field[i^5] = stack[i];
    }
    ~ClassWithSSE() {init(-2*n);}
    friend bool operator==( const ClassWithSSE& x, const ClassWithSSE& y ) {
        for( int i=0; i<4*n; ++i )
            if( ((const float*)x.field)[i]!=((const float*)y.field)[i] )
                return false;
        return true;
    }
    friend bool operator!=( const ClassWithSSE& x, const ClassWithSSE& y ) {
        return !(x==y);
    }
};

void ClassWithSSE::init( int start ) {
    __m128 stack[n];
    for( int i=0; i<n; ++i ) {
        // Declaring value as a one-element array instead of a scalar quites 
        // gratuitous warnings about possible use of "value" before it was set.
        __m128 value[1];
        for( int j=0; j<4; ++j )
            ((float*)value)[j] = float(n*start+4*i+j);
        stack[i^5] = value[0];
    }
    for( int i=0; i<n; ++i )
        field[i^5] = stack[i];
}

#endif /* __SSE__||_M_IX86 */
