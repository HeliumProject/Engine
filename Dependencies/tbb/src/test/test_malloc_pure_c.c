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

#ifdef __cplusplus
#error For testing purpose, this file should be compiled with a C compiler, not C++
#endif /*__cplusplus */

#include "tbb/scalable_allocator.h"
#include <stdio.h>
#include <assert.h>

/*
 *  The test is to check if the scalable_allocator.h and its functions
 *  can be used from pure C programs; also some regression checks are done
 */

int main(void) {
    size_t i, j;
    void *p1, *p2;
    for( i=0; i<=1<<16; ++i) {
        p1 = scalable_malloc(i);
        if( !p1 )
            printf("Warning: there should be memory but scalable_malloc returned NULL\n");
        scalable_free(p1);
    }
    p1 = p2 = NULL;
    for( i=1024*1024; ; i/=2 )
    {
        scalable_free(p1);
        p1 = scalable_realloc(p2, i);
        p2 = scalable_calloc(i, 32);
        if (p2) {
            if (i<sizeof(size_t)) {
                for (j=0; j<i; j++)
                    assert(0==*((char*)p2+j));
            } else {
                for (j=0; j<i; j+=sizeof(size_t))
                    assert(0==*((size_t*)p2+j));
            }
        }
        scalable_free(p2);
        p2 = scalable_malloc(i);
        if (i==0) break;
    }
    for( i=1; i<1024*1024; i*=2 )
    {
        scalable_free(p1);
        p1 = scalable_realloc(p2, i);
        p2 = scalable_malloc(i);
    }
    scalable_free(p1);
    scalable_free(p2);
    printf("done\n");
    return 0;
}
