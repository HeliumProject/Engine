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

// Example program that computes number of prime numbers up to n, 
// where n is a command line argument.  The algorithm here is a 
// fairly efficient version of the sieve of Eratosthenes. 
// The parallel version demonstrates how to use parallel_reduce,
// and in particular how to exploit lazy splitting.

#include <cassert>
#include <cstdio>
#include <cstring>
#include <math.h>
#include <cstdlib>
#include <cctype>
#include "tbb/parallel_reduce.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"

using namespace std;
using namespace tbb;

typedef unsigned long Number;

//! If true, then print primes on stdout.
static bool PrintPrimes = false;

//! Grainsize parameter
static Number GrainSize = 1000;

class Multiples {
    inline Number strike( Number start, Number limit, Number stride ) {
        // Hoist "my_is_composite" into register for sake of speed.
        bool* is_composite = my_is_composite;
        assert( stride>=2 );
        for( ;start<limit; start+=stride ) 
            is_composite[start] = true;
        return start;
    }
    //! Window into conceptual sieve 
    bool* my_is_composite;

    //! Indexes into window
    /** my_striker[k] is an index into my_composite corresponding to
        an odd multiple multiple of my_factor[k]. */
    Number* my_striker;

    //! Prime numbers less than m.
    Number* my_factor;
public:
    //! Number of factors in my_factor.
    Number n_factor;
    Number m;
    Multiples( Number n ) : 
        is_forked_copy(false) 
    {
        m = Number(sqrt(double(n)));
        // Round up to even
        m += m&1;
        my_is_composite = new bool[m/2];
        my_striker = new Number[m/2];
        my_factor = new Number[m/2];
        n_factor = 0;
        memset( my_is_composite, 0, m/2 );
        for( Number i=3; i<m; i+=2 ) {
            if( !my_is_composite[i/2] ) {
                if( PrintPrimes )
                    printf("%d\n",(int)i);
                my_striker[n_factor] = strike( i/2, m/2, i );
                my_factor[n_factor++] = i;
            }
        }
    }

    //! Find primes in range [start,window_size), advancing my_striker as we go.
    /** Returns number of primes found. */
    Number find_primes_in_window( Number start, Number window_size ) {
        bool* is_composite = my_is_composite;
        memset( is_composite, 0, window_size/2 );
        for( size_t k=0; k<n_factor; ++k )
            my_striker[k] = strike( my_striker[k]-m/2, window_size/2, my_factor[k] );
        Number count = 0;
        for( Number k=0; k<window_size/2; ++k ) {
            if( !is_composite[k] ) {
                if( PrintPrimes )
                    printf("%ld\n",long(start+2*k+1));
                ++count;
            }
        }
        return count;
    }

    ~Multiples() {
        if( !is_forked_copy )
            delete[] my_factor;
        delete[] my_striker;
        delete[] my_is_composite;
    }

    //------------------------------------------------------------------------
    // Begin extra members required by parallel version
    //------------------------------------------------------------------------

    //! True if this instance was forked from another instance.
    const bool is_forked_copy;

    Multiples( const Multiples& f, split ) :
        n_factor(f.n_factor),
        m(f.m),
        my_is_composite(NULL),
        my_striker(NULL),
        my_factor(f.my_factor),
        is_forked_copy(true)
    {}

    bool is_initialized() const {
        return my_is_composite!=NULL;
    }

    void initialize( Number start ) { 
        assert( start>=1 );
        my_is_composite = new bool[m/2];
        my_striker = new Number[m/2];
        for( size_t k=0; k<n_factor; ++k ) {
            Number f = my_factor[k];
            Number p = (start-1)/f*f % m;
            my_striker[k] = (p&1 ? p+2*f : p+f)/2;
            assert( m/2<=my_striker[k] );
        }
    }
    //------------------------------------------------------------------------
    // End extra methods required by parallel version
    //------------------------------------------------------------------------
};

//! Count number of primes between 0 and n
/** This is the serial version. */
Number SerialCountPrimes( Number n ) {
    // Two is special case
    Number count = n>=2;
    if( n>=3 ) {
        Multiples multiples(n);
        count += multiples.n_factor;
        if( PrintPrimes ) 
            printf("---\n");
        Number window_size = multiples.m;
        for( Number j=multiples.m; j<=n; j+=window_size ) { 
            if( j+window_size>n+1 ) 
                window_size = n+1-j;
            count += multiples.find_primes_in_window( j, window_size );
        }
    }
    return count;
}

//! Range of a sieve window.
class SieveRange {
    //! Width of full-size window into sieve.
    const Number my_stride;

    //! Always multiple of my_stride
    Number my_begin;

    //! One past last number in window.
    Number my_end;

    //! Width above which it is worth forking.
    const Number my_grainsize;

    bool assert_okay() const {
        assert( my_begin%my_stride==0 );
        assert( my_begin<=my_end );
        assert( my_stride<=my_grainsize );
        return true;
    } 
public:
    //------------------------------------------------------------------------
    // Begin signatures required by parallel_reduce
    //------------------------------------------------------------------------
    bool is_divisible() const {return my_end-my_begin>my_grainsize;}
    bool empty() const {return my_end<=my_begin;}
    SieveRange( SieveRange& r, split ) : 
        my_stride(r.my_stride), 
        my_grainsize(r.my_grainsize),
        my_end(r.my_end)
    {
        assert( r.is_divisible() );
        assert( r.assert_okay() );
        Number middle = r.my_begin + (r.my_end-r.my_begin+r.my_stride-1)/2;
        middle = middle/my_stride*my_stride;
        my_begin = middle;
        r.my_end = middle;
        assert( assert_okay() );
        assert( r.assert_okay() );
    }
    //------------------------------------------------------------------------
    // End of signatures required by parallel_reduce
    //------------------------------------------------------------------------
    Number begin() const {return my_begin;}
    Number end() const {return my_end;}
    SieveRange( Number begin, Number end, Number stride, Number grainsize ) :
        my_begin(begin),
        my_end(end),
        my_stride(stride),      
        my_grainsize(grainsize<stride?stride:grainsize)
    {
        assert( assert_okay() );
    }
};

//! Loop body for parallel_reduce.
/** parallel_reduce splits the sieve into subsieves.
    Each subsieve handles a subrange of [0..n]. */
class Sieve {
public:
    //! Prime multiples to consider, and working storage for this subsieve.
    Multiples multiples;

    //! Number of primes found so far by this subsieve.
    Number count;

    //! Construct Sieve for counting primes in [0..n].
    Sieve( Number n ) : 
        multiples(n),
        count(0)
    {}

    //------------------------------------------------------------------------
    // Begin signatures required by parallel_reduce
    //------------------------------------------------------------------------
    void operator()( const SieveRange& r ) {
        Number m = multiples.m;
        if( multiples.is_initialized() ) { 
            // Simply reuse "multiples" structure from previous window
            // This works because parallel_reduce always applies
            // *this from left to right.
        } else {
            // Need to initialize "multiples" because *this is a forked copy
            // that needs to be set up to start at r.begin().
            multiples.initialize( r.begin() );
        }
        Number window_size = m;
        for( Number j=r.begin(); j<r.end(); j+=window_size ) { 
            assert( j%multiples.m==0 );
            if( j+window_size>r.end() ) 
                window_size = r.end()-j;
            count += multiples.find_primes_in_window( j, window_size );
        }
    }
    void join( Sieve& other ) {
        count += other.count;
    }
    Sieve( Sieve& other, split ) : 
        multiples(other.multiples,split()),
        count(0)
    {}
    //------------------------------------------------------------------------
    // End of signatures required by parallel_reduce
    //------------------------------------------------------------------------
};

//! Count number of primes between 0 and n
/** This is the parallel version. */
Number ParallelCountPrimes( Number n ) {
    // Two is special case
    Number count = n>=2;
    if( n>=3 ) {
        Sieve s(n);
        count += s.multiples.n_factor;
        if( PrintPrimes ) 
            printf("---\n");
        // Explicit grain size and simple_partitioner() used here instead of automatic grainsize 
        // determination becase we want SieveRange to be decomposed down to GrainSize or smaller.  
        // Doing so improves odds that the working set fits in cache when evaluating Sieve::operator().
        parallel_reduce( SieveRange( s.multiples.m, n, s.multiples.m, GrainSize ), s, simple_partitioner() );
        count += s.count;
    }
    return count;
}

//------------------------------------------------------------------------
// Code below this line constitutes the driver that calls SerialCountPrimes
// and ParallelCountPrimes.
//------------------------------------------------------------------------

//! A closed range of Number.
struct NumberRange {
    Number low;
    Number high;
    void set_from_string( const char* s );
    NumberRange( Number low_, Number high_ ) : low(low_), high(high_) {}
};

void NumberRange::set_from_string( const char* s ) {
    char* end;
    high = low = strtol(s,&end,0);
    switch( *end ) {
    case ':': 
        high = strtol(end+1,0,0); 
        break;
    case '\0':
        break;
    default:
        printf("unexpected character = %c\n",*end);
    }
    
}

//! Number of threads to use.
static NumberRange NThread(0,4);

//! If true, then at end wait for user to hit return
static bool PauseFlag = false;

//! Parse the command line.
static Number ParseCommandLine( int argc, char* argv[] ) {
    Number n = 100000000;
    int i = 1;
    if( i<argc && strcmp( argv[i], "pause" )==0 ) {
        PauseFlag = true;
        ++i;
    }
    if( i<argc && !isdigit(argv[i][0]) ) { 
        // Command line is garbled.
        fprintf(stderr,"Usage: %s [['pause'] n [nthread [grainsize]]]\n", argv[0]);
        fprintf(stderr,"where n is a positive integer [%lu]\n",n);
        fprintf(stderr,"      nthread is a non-negative integer, or range of the form low:high [%ld:%lu]\n",NThread.low,NThread.high);
        fprintf(stderr,"      grainsize is an optional postive integer [%lu]\n",GrainSize);
        exit(1);
    }
    if( i<argc )
        n = strtol(argv[i++],0,0);
    if( i<argc )
        NThread.set_from_string(argv[i++]);
    if( i<argc )
        GrainSize = strtol(argv[i++],0,0);
    return n;
}

static void WaitForUser() {
    char c;
    printf("Press return to continue\n");
    do {
        c = getchar();
    } while( c!='\n' );
}

int main( int argc, char* argv[] ) {
    Number n = ParseCommandLine(argc,argv);

    // Try different numbers of threads
    for( Number p=NThread.low; p<=NThread.high; ++p ) {
        task_scheduler_init init(task_scheduler_init::deferred);
        // If p!=0, we are doing a parallel run
        if( p ) 
            init.initialize(p);

        Number count;
        tick_count t0 = tick_count::now();
        if( p==0 ) {
            count = SerialCountPrimes(n);
        } else {
            count = ParallelCountPrimes(n);
        }
        tick_count t1 = tick_count::now();

        printf("#primes from [2..%lu] = %lu (%.2f sec with ",
            (unsigned long)n, (unsigned long)count, (t1-t0).seconds());
        if( p ) 
            printf("%lu-way parallelism)\n", p );
        else 
            printf("serial code)\n");
    }
    if( PauseFlag ) {
        WaitForUser();
    }
    return 0;
}
