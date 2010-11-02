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

#ifndef __CONVEX_HULL_H__
#define __CONVEX_HULL_H__

#define _SCL_SECURE_NO_DEPRECATE
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <functional>
#include <assert.h>
#include "tbb/tick_count.h"

using namespace std;

namespace cfg {
    // convex hull problem parameter defaults
    const long    NP  = 5000000;  // problem size
    const int     SNT = 1;        // minimal number of threads
    const int     ENT = 8;        // maximal number of threads

    // convex hull problem user set parameters
    long   MAXPOINTS         = NP;
    int    NUM_THREADS_START = SNT;
    int    NUM_THREADS_END   = ENT;

    // convex hull grain sizes for 3 subproblems. Be sure 16*GS < 512Kb
    const size_t GENERATE_GS = 25000;
    const size_t FINDEXT_GS  = 25000;
    const size_t DIVIDE_GS   = 25000;
};

namespace util {
    bool                     VERBOSE = false;
    vector<string> OUTPUT;

    // utility functionality
    void ParseInputArgs(int argc, char* argv[]) {
        int numArgs = 1;
        if(argc>numArgs) {
            char delim = ':';
            if(!strcmp(argv[numArgs], "-h")) {
                cout << " Program usage is:" << endl
                    << " " << argv[0] << " [NP] [SNT" << delim << "ENT] [-v]"
                    << endl << endl
                    << " where:" << endl
                    << " NP  - number of points" << endl
                    << " SNT - start with this number of threads" << endl
                    << " ENT - end with this number of threads" << endl
                    << "  -v - turns verbose ON" << endl;
                exit(0);
            } else {
                while(argc>numArgs) {
                    char* endptr;
                    if(!strcmp(argv[numArgs], "-v")) {
                        VERBOSE = true;
                    } else if(!strchr(argv[numArgs], delim)) {
                        cfg::MAXPOINTS = strtol(argv[numArgs], &endptr, 0);
                        if(*endptr!='\0') {
                            cout << " wrong parameter format for Number of Points" << endl;
                            exit(1);
                        }
                        if(cfg::MAXPOINTS<=0) {
                            cout
                                << "  wrong value set for Number of Points" << endl
                                << "  using default value: " << endl
                                << "  Number of Points = " << cfg::NP << endl;
                            cfg::MAXPOINTS = cfg::NP;
                        }
                    } else {
                        cfg::NUM_THREADS_START=(int)strtol(argv[numArgs], &endptr, 0);
                        if(*endptr==delim) {
                            cfg::NUM_THREADS_END = (int)strtol(endptr+1, &endptr, 0);
                        } else {
                            cout << " wrong parameter format for Number of Threads" << endl;
                            exit(1);
                        }
                        if(*endptr!='\0') {
                            cout << " wrong parameter format for Number of Threads" << endl;
                            exit(1);
                        }    
                        if((cfg::NUM_THREADS_START<=0)
                            || (cfg::NUM_THREADS_END<cfg::NUM_THREADS_START)) {
                                cout
                                    << "  wrong values set for Number of Threads" << endl
                                    << "  using default values: " << endl
                                    << "  start NT = " << cfg::SNT << endl
                                    << "  end   NT = " << cfg::ENT << endl;
                                cfg::NUM_THREADS_START=cfg::SNT;
                                cfg::NUM_THREADS_END  =cfg::ENT;
                        }
                    }
                    ++numArgs;
                }
            }
        }
    }

    template <typename T>
    struct point {
        T x;
        T y;
        point() : x(T()), y(T()) {}
        point(T _x, T _y) : x(_x), y(_y) {}
        //why do we need below line? it fails to compile with suncc
	    //point(const point<T>& _P) : x(_P.x), y(_P.y) {} 
	};

    int random(unsigned int& rseed) {
#if __linux__ || __APPLE__ || __FreeBSD__ 
            return rand_r(&rseed);
#elif _WIN32 || __sun
            return rand();
#else
#error Unknown/unsupported OS?
#endif // __linux__ || __APPLE__ || __FreeBSD__ 
    }

    template < typename T >
    point<T> GenerateRNDPoint(size_t& count, unsigned int& rseed) {
        /* generates random points on 2D plane so that the cluster
        is somewhat circle shaped */
        const size_t maxsize=500;
        T x = random(rseed)*2.0/(double)RAND_MAX - 1;
        T y = random(rseed)*2.0/(double)RAND_MAX - 1;
        T r = (x*x + y*y);
        if(r>1) {
            count++;
            if(count>10) {
                if (random(rseed)/(double)RAND_MAX > 0.5)
                    x /= r;
                if (random(rseed)/(double)RAND_MAX > 0.5)
                    y /= r;
                count = 0;
            }
            else {
                x /= r;
                y /= r;
            }
        }

        x = (x+1)*0.5*maxsize;
        y = (y+1)*0.5*maxsize;

        return point<T>(x,y);
    }

    template <typename Index>
    struct edge {
        Index start;
        Index end;
        edge(Index _p1, Index _p2) : start(_p1), end(_p2) {};
    };

    template <typename T>
    ostream& operator <<(ostream& _ostr, point<T> _p) {
        return _ostr << '(' << _p.x << ',' << _p.y << ')';
    }

    template <typename T>
    istream& operator >>(istream& _istr, point<T> _p) {
        return _istr >> _p.x >> _p.y;
    }

    template <typename T>
    bool operator ==(point<T> p1, point<T> p2) {
        return (p1.x == p2.x && p1.y == p2.y);
    }

    template <typename T>
    bool operator !=(point<T> p1, point<T> p2) {
        return !(p1 == p2);
    }

    template <typename T>
    double cross_product(const point<T>& start, const point<T>& end1, const point<T>& end2) {
        return ((end1.x-start.x)*(end2.y-start.y)-(end2.x-start.x)*(end1.y-start.y));
    }

    // Timing functions are based on TBB to always obtain wall-clock time
    typedef tbb::tick_count my_time_t;

    my_time_t gettime() {
        return tbb::tick_count::now();
    }

    double time_diff(my_time_t start, my_time_t end) {
        return (end-start).seconds();
    }

    void WriteResults(int nthreads, double initTime, double calcTime) {
        if(VERBOSE) {
            cout << " Step by step hull construction:" << endl;
            for(size_t i = 0; i < OUTPUT.size(); ++i)
                cout << OUTPUT[i] << endl;
        }

        cout
            << "  Number of nodes:" << cfg::MAXPOINTS
            << "  Number of threads:" << nthreads 
            << "  Initialization time:" << setw(10) << setprecision(3) << initTime 
            << "  Calculation time:" << setw(10) << setprecision(3) << calcTime
            << endl;
    }
};

#endif // __CONVEX_HULL_H__
