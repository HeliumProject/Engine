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

/*
    This file contains the TBB-based implementation of convex hull algortihm.
    It corresponds to the following settings in convex_hull_bench.cpp:
    - USETBB defined to 1
    - USECONCVEC defined to 1
    - INIT_ONCE defined to 0
    - only buffered version is used
*/
#include "convex_hull.h"

#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/parallel_reduce.h"
#include "tbb/blocked_range.h"
#include "tbb/tick_count.h"
#include "tbb/concurrent_vector.h"

typedef util::point<double>               point_t;
typedef tbb::concurrent_vector< point_t > pointVec_t;
typedef tbb::blocked_range<size_t>        range_t;

void appendVector(const point_t* src, size_t srcSize, pointVec_t& dest) {
    std::copy(src, src + srcSize, dest.grow_by(srcSize));
}

void appendVector(const pointVec_t& src, pointVec_t& dest) {
    std::copy(src.begin(), src.end(), dest.grow_by(src.size()));
}

class FillRNDPointsVector_buf {
    pointVec_t          &points;
    mutable unsigned int rseed;
public:
    static const size_t  grainSize = cfg::GENERATE_GS;

    FillRNDPointsVector_buf(pointVec_t& _points)
        : points(_points), rseed(1) {}

    FillRNDPointsVector_buf(const FillRNDPointsVector_buf& other)
        : points(other.points), rseed(other.rseed+1) {}

    void operator()(const range_t& range) const {
        const size_t i_end = range.end();
        size_t count = 0, j = 0;
        point_t tmp_vec[grainSize];
        for(size_t i=range.begin(); i!=i_end; ++i) {
            tmp_vec[j++] = util::GenerateRNDPoint<double>(count, rseed);
        }
        appendVector(tmp_vec, j, points);
    }
};

void initialize(pointVec_t &points) {
    points.clear();

    // In the buffered version, a temporary storage for as much as grainSize elements 
    // is allocated inside the body. Since auto_partitioner may increase effective
    // range size which would cause a crash, simple partitioner has to be used.
    tbb::parallel_for(range_t(0, cfg::MAXPOINTS, FillRNDPointsVector_buf::grainSize),
                      FillRNDPointsVector_buf(points), tbb::simple_partitioner());
}

class FindXExtremum {
public:
    typedef enum {
        minX, maxX
    } extremumType;

    static const size_t  grainSize = cfg::FINDEXT_GS;

    FindXExtremum(const pointVec_t& points_, extremumType exType_)
        : points(points_), exType(exType_), extrXPoint(points[0]) {}

    FindXExtremum(const FindXExtremum& fxex, tbb::split)
        // Can run in parallel with fxex.operator()() or fxex.join().
        // The data race reported by tools is harmless.
        : points(fxex.points), exType(fxex.exType), extrXPoint(fxex.extrXPoint) {}

    void operator()(const range_t& range) {
        const size_t i_end = range.end();
        if(!range.empty()) {
            for(size_t i = range.begin(); i != i_end; ++i) {
                if(closerToExtremum(points[i])) {
                    extrXPoint = points[i];
                }
            }
        }
    }

    void join(const FindXExtremum &rhs) {
        if(closerToExtremum(rhs.extrXPoint)) {
            extrXPoint = rhs.extrXPoint;
        }
    }

    point_t extremeXPoint() {
        return extrXPoint;
    }

private:
    const pointVec_t    &points;
    const extremumType   exType;
    point_t              extrXPoint;
    bool closerToExtremum(const point_t &p) const {
        switch(exType) {
        case minX:
            return p.x<extrXPoint.x; break;
        case maxX:
            return p.x>extrXPoint.x; break;
        }
        return false; // avoid warning
    }
};

template <FindXExtremum::extremumType type>
point_t extremum(const pointVec_t &P) {
    FindXExtremum fxBody(P, type);
    tbb::parallel_reduce(range_t(0, P.size(), FindXExtremum::grainSize), fxBody);
    return fxBody.extremeXPoint();
}

class SplitByCP_buf {
    const pointVec_t    &initialSet;
    pointVec_t          &reducedSet;
    point_t              p1, p2;
    point_t              farPoint;
    double               howFar;
public:
    static const size_t  grainSize = cfg::DIVIDE_GS;

    SplitByCP_buf( point_t _p1, point_t _p2,
        const pointVec_t &_initialSet, pointVec_t &_reducedSet)
        : p1(_p1), p2(_p2),
        initialSet(_initialSet), reducedSet(_reducedSet),
        howFar(0), farPoint(p1) {}

    SplitByCP_buf(SplitByCP_buf& sbcp, tbb::split)
        : p1(sbcp.p1), p2(sbcp.p2),
        initialSet(sbcp.initialSet), reducedSet(sbcp.reducedSet),
        howFar(0), farPoint(p1) {}

    void operator()(const range_t& range) {
        const size_t i_end = range.end();
        size_t j = 0;
        double cp;
        point_t tmp_vec[grainSize];
        for(size_t i = range.begin(); i != i_end; ++i) {
            if( (initialSet[i] != p1) && (initialSet[i] != p2) ) {            
                cp = util::cross_product(p1, p2, initialSet[i]);
                if(cp>0) {
                    tmp_vec[j++] = initialSet[i];
                    if(cp>howFar) {
                        farPoint = initialSet[i];
                        howFar   = cp;
                    }
                }
            }
        }

        appendVector(tmp_vec, j, reducedSet);
    }

    void join(const SplitByCP_buf& rhs) {
        if(rhs.howFar>howFar) {
            howFar   = rhs.howFar;
            farPoint = rhs.farPoint;
        }
    }

    point_t farthestPoint() const {
        return farPoint;
    }
};

point_t divide(const pointVec_t &P, pointVec_t &P_reduced, 
                   const point_t &p1, const point_t &p2) {
    SplitByCP_buf sbcpb(p1, p2, P, P_reduced);
    // Must use simple_partitioner (see the comment in initialize() above)
    tbb::parallel_reduce(range_t(0, P.size(), SplitByCP_buf::grainSize),
                         sbcpb, tbb::simple_partitioner());

    if(util::VERBOSE) {
        std::stringstream ss;
        ss << P.size() << " nodes in bucket"<< ", "
            << "dividing by: [ " << p1 << ", " << p2 << " ], "
            << "farthest node: " << sbcpb.farthestPoint();
        util::OUTPUT.push_back(ss.str());
    }

    return sbcpb.farthestPoint();
}

void divide_and_conquer(const pointVec_t &P, pointVec_t &H,
                            point_t p1, point_t p2) {
    if (P.size()<2) {
        H.push_back(p1);
        appendVector(P, H);
    }
    else {
        pointVec_t P_reduced;
        pointVec_t H1, H2;

        point_t p_far = divide(P, P_reduced, p1, p2);

        divide_and_conquer(P_reduced, H1, p1, p_far);
        divide_and_conquer(P_reduced, H2, p_far, p2);

        appendVector(H1, H);
        appendVector(H2, H);
    }
}

void quickhull(const pointVec_t &points, pointVec_t &hull) {
    hull.clear();

    point_t p_maxx = extremum<FindXExtremum::maxX>(points);
    point_t p_minx = extremum<FindXExtremum::minX>(points);

    pointVec_t H;

    divide_and_conquer(points, hull, p_maxx, p_minx);
    divide_and_conquer(points, H, p_minx, p_maxx);

    appendVector(H, hull);
}

int main(int argc, char* argv[]) {
    util::ParseInputArgs(argc, argv);

    pointVec_t      points;
    pointVec_t      hull;
    int             nthreads;
    util::my_time_t tm_init, tm_start, tm_end;

    std::cout << " Starting TBB-bufferred version of QUICK HULL algorithm" << std::endl;

    for(nthreads=cfg::NUM_THREADS_START; nthreads<=cfg::NUM_THREADS_END;
        ++nthreads) {
        tbb::task_scheduler_init init(nthreads);
        tm_init = util::gettime();
        initialize(points);
        tm_start = util::gettime();
        quickhull(points, hull);
        tm_end = util::gettime();

        util::WriteResults(nthreads, util::time_diff(tm_init, tm_start),
            util::time_diff(tm_start, tm_end));
    }

    return 0;
}
