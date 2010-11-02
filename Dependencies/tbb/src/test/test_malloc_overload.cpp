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


#if __linux__
#define MALLOC_REPLACEMENT_AVAILABLE 1
#elif _WIN32 && !__MINGW32__ && !__MINGW64__
#define MALLOC_REPLACEMENT_AVAILABLE 2
#include "tbb/tbbmalloc_proxy.h"
#endif

#if MALLOC_REPLACEMENT_AVAILABLE

#if _WIN32 || _WIN64
// As the test is intentionally build with /EHs-, suppress multiple VS2005's 
// warnings like C4530: C++ exception handler used, but unwind semantics are not enabled
#if defined(_MSC_VER) && !__INTEL_COMPILER
/* ICC 10.1 and 11.0 generates code that uses std::_Raise_handler,
   but it's only defined in libcpmt(d), which the test doesn't linked with.
 */
#define _HAS_EXCEPTIONS 0
#endif
// to use strdup and putenv w/o warnings
#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif
#include "harness_report.h"
#include "harness_assert.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <new>

#if __linux__
#include <dlfcn.h>
#include <unistd.h> // for sysconf
#include <stdint.h> // for uintptr_t

#elif _WIN32
#include <stddef.h>
#if __MINGW32__
#include <unistd.h>
#else
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#endif

#endif /* OS selection */

#if _WIN32
// On Windows, the tricky way to print "done" is necessary to create 
// dependence on msvcpXX.dll, for sake of a regression test.
// On Linux, C++ RTL headers are undesirable because of breaking strict ANSI mode.
#if defined(_MSC_VER) && _MSC_VER >= 1300 && _MSC_VER <= 1310 && !defined(__INTEL_COMPILER)
/* Fixing compilation error reported by VS2003 for exception class
   when _HAS_EXCEPTIONS is 0: 
   bad_cast that inherited from exception is not in std namespace.
*/
using namespace std;
#endif
#include <string>
#endif


template<typename T>
static inline T alignDown(T arg, uintptr_t alignment) {
    return T( (uintptr_t)arg  & ~(alignment-1));
}
template<typename T>
static inline bool isAligned(T arg, uintptr_t alignment) {
    return 0==((uintptr_t)arg &  (alignment-1));
}

/* Below is part of MemoryAllocator.cpp. */

class BackRefIdx { // composite index to backreference array
private:
    uint16_t master;      // index in BackRefMaster
    uint16_t largeObj:1;  // is this object "large"?
    uint16_t offset  :15; // offset from beginning of BackRefBlock
public:
    BackRefIdx() : master((uint16_t)-1) {}
    bool isInvalid() { return master == (uint16_t)-1; }
    bool isLargeObject() const { return largeObj; }
    uint16_t getMaster() const { return master; }
    uint16_t getOffset() const { return offset; }

    // only newBackRef can modify BackRefIdx
    static BackRefIdx newBackRef(bool largeObj);
};

struct LargeMemoryBlock {
    LargeMemoryBlock *next,          // ptrs in list of cached blocks
                     *prev;
    uintptr_t         age;           // age of block while in cache
    size_t            objectSize;    // the size requested by a client
    size_t            unalignedSize; // the size requested from getMemory
    bool              fromMapMemory;
    BackRefIdx        backRefIdx;    // cached here, used copy is in LargeObjectHdr
};

struct LargeObjectHdr {
    LargeMemoryBlock *memoryBlock;
    /* Have to duplicate it here from CachedObjectHdr, 
       as backreference must be checked without further pointer dereference.
       Points to LargeObjectHdr. */
    BackRefIdx       backRefIdx;
};

/*
 * Objects of this size and larger are considered large objects.
 */
const uint32_t minLargeObjectSize = 8065;

/* end of inclusion from MemoryAllocator.cpp */

/* Correct only for large blocks, i.e. not smaller then minLargeObjectSize */
static bool scalableMallocLargeBlock(void *object, size_t size)
{
    ASSERT(size >= minLargeObjectSize, NULL);
#if MALLOC_REPLACEMENT_AVAILABLE == 2
    // Check that _msize works correctly
    ASSERT(_msize(object) >= size, NULL);
#endif

    LargeMemoryBlock *lmb = ((LargeObjectHdr*)object-1)->memoryBlock;
    return uintptr_t(lmb)<uintptr_t(((LargeObjectHdr*)object-1)) && lmb->objectSize==size;
}

struct BigStruct {
    char f[minLargeObjectSize];
};

int main(int , char *[]) {
    void *ptr, *ptr1;

#if MALLOC_REPLACEMENT_AVAILABLE == 1
    if (NULL == dlsym(RTLD_DEFAULT, "scalable_malloc")) {
        REPORT("libtbbmalloc not found\nfail\n");
        return 1;
    }
#endif

/* On Windows, memory block size returned by _msize() is sometimes used 
   to calculate the size for an extended block. Substituting _msize, 
   scalable_msize initially returned 0 for regions not allocated by the scalable 
   allocator, which led to incorrect memory reallocation and subsequent crashes.
   It was found that adding a new environment variable triggers the error.
*/
    ASSERT(getenv("PATH"), "We assume that PATH is set everywhere.");
    char *pathCopy = strdup(getenv("PATH"));
    const char *newEnvName = "__TBBMALLOC_OVERLOAD_REGRESSION_TEST_FOR_REALLOC_AND_MSIZE";
    char *newEnv = (char*)malloc(3 + strlen(newEnvName));
    
    ASSERT(!getenv(newEnvName), "Environment variable should not be used before.");
    strcpy(newEnv, newEnvName);
    strcat(newEnv, "=1");
    int r = putenv(newEnv);
    ASSERT(!r, NULL);
    char *path = getenv("PATH");
    ASSERT(path && 0==strcmp(path, pathCopy), "Environment was changed erroneously.");
    free(pathCopy);
    free(newEnv);

    ptr = malloc(minLargeObjectSize);
    ASSERT(ptr!=NULL && scalableMallocLargeBlock(ptr, minLargeObjectSize), NULL);
    free(ptr);

    ptr = calloc(minLargeObjectSize, 2);
    ASSERT(ptr!=NULL && scalableMallocLargeBlock(ptr, minLargeObjectSize*2), NULL);
    ptr1 = realloc(ptr, minLargeObjectSize*10);
    ASSERT(ptr1!=NULL && scalableMallocLargeBlock(ptr1, minLargeObjectSize*10), NULL);
    free(ptr1);

#if MALLOC_REPLACEMENT_AVAILABLE == 1

    int ret = posix_memalign(&ptr, 1024, 3*minLargeObjectSize);
    ASSERT(0==ret && ptr!=NULL && scalableMallocLargeBlock(ptr, 3*minLargeObjectSize), NULL);
    free(ptr);

    ptr = memalign(128, 4*minLargeObjectSize);
    ASSERT(ptr!=NULL && scalableMallocLargeBlock(ptr, 4*minLargeObjectSize), NULL);
    free(ptr);

    ptr = valloc(minLargeObjectSize);
    ASSERT(ptr!=NULL && scalableMallocLargeBlock(ptr, minLargeObjectSize), NULL);
    free(ptr);

    long memoryPageSize = sysconf(_SC_PAGESIZE);
    int sz = 1024*minLargeObjectSize;
    ptr = pvalloc(sz);
    ASSERT(ptr!=NULL &&                // align size up to the page size
           scalableMallocLargeBlock(ptr, ((sz-1) | (memoryPageSize-1)) + 1), NULL);
    free(ptr);

    struct mallinfo info = mallinfo();
    // right now mallinfo initialized by zero
    ASSERT(!info.arena && !info.ordblks && !info.smblks && !info.hblks 
           && !info.hblkhd && !info.usmblks && !info.fsmblks 
           && !info.uordblks && !info.fordblks && !info.keepcost, NULL);

#elif MALLOC_REPLACEMENT_AVAILABLE == 2

    ptr = _aligned_malloc(minLargeObjectSize,16);
    ASSERT(ptr!=NULL && scalableMallocLargeBlock(ptr, minLargeObjectSize), NULL);

    ptr1 = _aligned_realloc(ptr, minLargeObjectSize*10,16);
    ASSERT(ptr1!=NULL && scalableMallocLargeBlock(ptr1, minLargeObjectSize*10), NULL);
    _aligned_free(ptr1);

#endif

    BigStruct *f = new BigStruct;
    ASSERT(f!=NULL && scalableMallocLargeBlock(f, sizeof(BigStruct)), NULL);
    delete f;

    f = new BigStruct[10];
    ASSERT(f!=NULL && scalableMallocLargeBlock(f, 10*sizeof(BigStruct)), NULL);
    delete []f;

    f = new(std::nothrow) BigStruct;
    ASSERT(f!=NULL && scalableMallocLargeBlock(f, sizeof(BigStruct)), NULL);
    delete f;

    f = new(std::nothrow) BigStruct[2];
    ASSERT(f!=NULL && scalableMallocLargeBlock(f, 2*sizeof(BigStruct)), NULL);
    delete []f;

#if _WIN32
    std::string stdstring = "done";
    const char* s = stdstring.c_str();
#else
    const char* s = "done";
#endif
    REPORT("%s\n", s);
    return 0;
}

#define HARNESS_NO_PARSE_COMMAND_LINE 1
#define HARNESS_CUSTOM_MAIN 1
#include "harness.h"

#else  /* !MALLOC_REPLACEMENT_AVAILABLE */
#include <stdio.h>

int main(int , char *[]) {
    printf("skip\n");
    return 0;
}
#endif /* !MALLOC_REPLACEMENT_AVAILABLE */
