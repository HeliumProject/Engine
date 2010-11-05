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

/* Regression test against bug in TBB allocator, manifested when 
   dynamic library calls atexit or register dtors of static objects.
   If the allocator is not initialized yet, we can got deadlock, 
   because allocator library has static object dtors as well, they
   registred during allocator initialization, and atexit is protected 
   by non-recursive mutex in some GLIBCs.
 */

#if _USRDLL

#include <stdlib.h>

#if _WIN32||_WIN64
// isMallocOverloaded must be defined in DLL to linker not drop the dependence
// to the DLL.
extern __declspec(dllexport) bool isMallocOverloaded();

bool isMallocOverloaded()
{
    return true;
}

#else

#include <dlfcn.h>

bool isMallocOverloaded()
{
    return dlsym(RTLD_DEFAULT, "__TBB_malloc_proxy");
}

#endif    

#ifndef _PGO_INSTRUMENT
void dummyFunction() {}

class Foo {
public:
    Foo() {
        // add a lot of exit handlers to cause memory allocation
        for (int i=0; i<1024; i++)
            atexit(dummyFunction);
    }
};

static Foo f;
#endif

#else // _USRDLL
#include "harness.h"

#if _WIN32||_WIN64
extern __declspec(dllimport)
#endif
bool isMallocOverloaded();

int TestMain () {
#ifdef _PGO_INSTRUMENT
    REPORT("Known issue: test_malloc_atexit hangs if compiled with -prof-genx\n");
    return Harness::Skipped;
#else
    return isMallocOverloaded()? Harness::Done : Harness::Skipped;
#endif
}

#endif // _USRDLL
