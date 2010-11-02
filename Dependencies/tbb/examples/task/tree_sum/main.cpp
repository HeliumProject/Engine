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

#include "common.h"
#include "tbb/tick_count.h"
#include "tbb/task.h"
#include "tbb/task_scheduler_init.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

// The performance of this example can be significantly better when
// the objects are allocated by the scalable_allocator instead of the
// default "operator new".  The reason is that the scalable_allocator
// typically packs small objects more tightly than the default "operator new",
// resulting in a smaller memory footprint, and thus more efficient use of
// cache and virtual memory.  Also the scalable_allocator works faster for
// multi-threaded allocations.
//
// Pass -stdmalloc as the 1st command line parameter to use the default "operator new"
// and see the performance difference.

#include "tbb/scalable_allocator.h"

using namespace std;

static double Pi = 3.14159265358979;

const bool tbbmalloc = true;
const bool stdmalloc = false;

template<bool use_tbbmalloc>
class TreeMaker {

    class SubTreeCreationTask: public tbb::task {
        TreeNode*& my_root;
        bool is_continuation;
        typedef TreeMaker<use_tbbmalloc> MyTreeMaker;

    public:
        SubTreeCreationTask( TreeNode*& root, long number_of_nodes ) : my_root(root), is_continuation(false) {
            my_root = MyTreeMaker::allocate_node();
            my_root->node_count = number_of_nodes;
            my_root->value = Value(Pi*number_of_nodes);
        }

        tbb::task* execute() {
            tbb::task* next = NULL;
            if( !is_continuation ) {
                long subtree_size = my_root->node_count - 1;
                if( subtree_size<1000 ) { /* grainsize */
                    my_root->left  = MyTreeMaker::do_in_one_thread(subtree_size/2);
                    my_root->right = MyTreeMaker::do_in_one_thread(subtree_size - subtree_size/2);
                } else {
                    // Create tasks before spawning any of them.
                    tbb::task* a = new( allocate_child() ) SubTreeCreationTask(my_root->left,subtree_size/2);
                    tbb::task* b = new( allocate_child() ) SubTreeCreationTask(my_root->right,subtree_size - subtree_size/2);
                    recycle_as_continuation();
                    is_continuation = true;
                    set_ref_count(2);
                    spawn(*b);
                    next = a;
                }
            } 
            return next;
        }
    };

public:
    static TreeNode* allocate_node() {
        return use_tbbmalloc? tbb::scalable_allocator<TreeNode>().allocate(1) : new TreeNode;
    }

    static TreeNode* do_in_one_thread( long number_of_nodes ) {
        if( number_of_nodes==0 ) {
            return NULL;
        } else {
            TreeNode* n = allocate_node();
            n->node_count = number_of_nodes;
            n->value = Value(Pi*number_of_nodes);
            --number_of_nodes;
            n->left  = do_in_one_thread( number_of_nodes/2 ); 
            n->right = do_in_one_thread( number_of_nodes - number_of_nodes/2 );
            return n;
        }
    }

    static TreeNode* do_in_parallel( long number_of_nodes ) {
        TreeNode* root_node;
        SubTreeCreationTask& a = *new(tbb::task::allocate_root()) SubTreeCreationTask(root_node, number_of_nodes);
        tbb::task::spawn_root_and_wait(a);
        return root_node;
    }

    static TreeNode* create_and_time( long number_of_nodes ) {
        tbb::tick_count t0, t1;
        TreeNode* root = allocate_node();
        root->node_count = number_of_nodes;
        root->value = Value(Pi*number_of_nodes);
        --number_of_nodes;

        t0 = tbb::tick_count::now();
        root->left  = do_in_one_thread( number_of_nodes/2 );
        t1 = tbb::tick_count::now();
        printf ("%24s: time = %.1f msec\n", "half created serially", (t1-t0).seconds()*1000);

        t0 = tbb::tick_count::now();
        root->right = do_in_parallel( number_of_nodes - number_of_nodes/2 );
        t1 = tbb::tick_count::now();
        printf ("%24s: time = %.1f msec\n", "half done in parallel", (t1-t0).seconds()*1000);

        return root;
    }
};

int main( int argc, char *argv[] ) {
    // Parse command line parameters
    // The format is: <exe_name> [-stdmalloc] [num_of_nodes [num_of_threads]]
    bool use_tbbmalloc = true;
    int arg_idx = 1;
    if( argc>1 && strcmp(argv[1], "-stdmalloc")==0 ) {
        use_tbbmalloc = false;
        arg_idx = 2;
    }
    long number_of_nodes = argc>arg_idx ? strtol(argv[arg_idx],0,0) : 10000000;
    ++arg_idx;
    int nthread = argc>arg_idx ? strtol(argv[arg_idx],0,0) : tbb::task_scheduler_init::automatic;

    // Start up scheduler
    // For production, no argument should be provided to the constructor, so that
    // the application gets the number of threads that are physically available.
    tbb::task_scheduler_init init(nthread);

    TreeNode* root;
    if( use_tbbmalloc ) {
        printf("Tree creation using TBB scalable allocator\n");
        root = TreeMaker<tbbmalloc>::create_and_time( number_of_nodes );
    } else {
        printf("Tree creation using standard operator new\n");
        root = TreeMaker<stdmalloc>::create_and_time( number_of_nodes );
    }

    // Warm up caches
    SerialSumTree(root);
    printf("Calculations:\n");
    const char* which;
    for( int i=0; i<3; ++i ) {
        tbb::tick_count t0 = tbb::tick_count::now();
        Value result;
        switch( i ) {
            case 0: 
                which = "SerialSumTree";
                result = SerialSumTree(root); 
                break;
            case 1: 
                which = "SimpleParallelSumTree";
                result = SimpleParallelSumTree(root); 
                break;
            case 2: 
                which = "OptimizedParallelSumTree";
                result = OptimizedParallelSumTree(root); 
                break;
        }
        tbb::tick_count t1 = tbb::tick_count::now();
        printf ("%24s: time = %.1f msec, sum=%g\n", which, (t1-t0).seconds()*1000, result);
    }
    return 0;
}
