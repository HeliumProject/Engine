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

#include "tbb/atomic.h"

#ifndef harness_barrier_H
#define harness_barrier_H

namespace Harness {

class SpinBarrier
{
    unsigned numThreads;
    tbb::atomic<unsigned> numThreadsFinished; /* threads reached barrier in this epoch */
    tbb::atomic<unsigned> epoch;   /* how many times this barrier used - XXX move to a separate cache line */

    struct DummyCallback {
        void operator() () const {}
    };

    SpinBarrier( const SpinBarrier& );    // no copy ctor
    void operator=( const SpinBarrier& ); // no assignment 
public:
    SpinBarrier( unsigned nthreads = 0 ) { initialize(nthreads); };

    void initialize( unsigned nthreads ) {
        numThreads = nthreads;
        numThreadsFinished = 0;
        epoch = 0;
    };

    // onOpenBarrierCallback is called by last thread arrived on a barrier
    template<typename Callback>
    bool wait(const Callback &onOpenBarrierCallback)
    { // return true if last thread
        unsigned myEpoch = epoch;
        int threadsLeft = numThreads - numThreadsFinished.fetch_and_increment() - 1;
        ASSERT(threadsLeft>=0, "Broken barrier");
        if (threadsLeft > 0) {
            /* not the last threading reaching barrier, wait until epoch changes & return 0 */
            tbb::internal::spin_wait_while_eq(epoch, myEpoch);
            return false;
        }
        /* No more threads left to enter, so I'm the last one reaching this epoch;
           reset the barrier, increment epoch, and return non-zero */
        onOpenBarrierCallback();
        numThreadsFinished = 0;
        epoch = myEpoch+1; /* wakes up threads waiting to exit this epoch */
        return true;
    }
    bool wait()
    {
        return wait(DummyCallback());
    }
};

}

#endif //harness_barrier_H
