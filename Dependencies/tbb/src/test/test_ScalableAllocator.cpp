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

// Test whether scalable_allocator complies with the requirements in 20.1.5 of ISO C++ Standard (1998).

#define HARNESS_NO_PARSE_COMMAND_LINE 1

#include "tbb/scalable_allocator.h"

// the actual body of the test is there:
#include "test_allocator.h"

#if _MSC_VER
#include "tbb/machine/windows_api.h"
#endif /* _MSC_VER */

int TestMain () {
#if _MSC_VER && !__TBBMALLOC_NO_IMPLICIT_LINKAGE
    #ifdef _DEBUG
        ASSERT(!GetModuleHandle("tbbmalloc.dll") && GetModuleHandle("tbbmalloc_debug.dll"),
            "test linked with wrong (non-debug) tbbmalloc library");
    #else
        ASSERT(!GetModuleHandle("tbbmalloc_debug.dll") && GetModuleHandle("tbbmalloc.dll"),
            "test linked with wrong (debug) tbbmalloc library");
    #endif
#endif /* _MSC_VER && !__TBBMALLOC_NO_IMPLICIT_LINKAGE */
    int result = TestMain<tbb::scalable_allocator<void> >();
    ASSERT( !result, NULL );
    return Harness::Done;
}
