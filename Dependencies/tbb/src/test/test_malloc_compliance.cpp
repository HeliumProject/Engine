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

const int MByte = 1048576; //1MB
bool __tbb_test_errno = false;

/* _WIN32_WINNT should be defined at the very beginning, 
   because other headers might include <windows.h>
*/

#if _WIN32 || _WIN64 && !__MINGW64__
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#include "tbb/machine/windows_api.h"
#include <stdio.h>
#include "harness_report.h"

void limitMem( int limit )
{
    static HANDLE hJob = NULL;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobInfo;

    jobInfo.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_MEMORY;
    jobInfo.ProcessMemoryLimit = limit? limit*MByte : 2*1024LL*MByte;
    if (NULL == hJob) {
        if (NULL == (hJob = CreateJobObject(NULL, NULL))) {
            REPORT("Can't assign create job object: %ld\n", GetLastError());
            exit(1);
        }
        if (0 == AssignProcessToJobObject(hJob, GetCurrentProcess())) {
            REPORT("Can't assign process to job object: %ld\n", GetLastError());
            exit(1);
        }
    }
    if (0 == SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, 
                                     &jobInfo, sizeof(jobInfo))) {
        REPORT("Can't set limits: %ld\n", GetLastError());
        exit(1);
    }
}
// Do not test errno with static VC runtime
#else
#include <sys/resource.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>  // uint64_t on FreeBSD, needed for rlim_t
#include "harness_report.h"

void limitMem( int limit )
{
    rlimit rlim;
    rlim.rlim_cur = limit? limit*MByte : (rlim_t)RLIM_INFINITY;
    rlim.rlim_max = (rlim_t)RLIM_INFINITY;
    int ret = setrlimit(RLIMIT_AS,&rlim);
    if (0 != ret) {
        REPORT("Can't set limits: errno %d\n", errno);
        exit(1);
    }
}
#endif 

#define ASSERT_ERRNO(cond, msg)  ASSERT( !__tbb_test_errno || (cond), msg )
#define CHECK_ERRNO(cond) (__tbb_test_errno && (cond))

#include <time.h>
#include <errno.h>
#define __TBB_NO_IMPLICIT_LINKAGE 1
#include "tbb/scalable_allocator.h"
#include "tbb/tbb_machine.h"

#define HARNESS_CUSTOM_MAIN 1
#include "harness.h"
#include "harness_barrier.h"
#if __linux__
#include <stdint.h> // uintptr_t
#endif
#if _WIN32 || _WIN64
#include <malloc.h> // _aligned_(malloc|free|realloc)
#endif

#if !TBB_USE_EXCEPTIONS && _MSC_VER
    // Suppress "C++ exception handler used, but unwind semantics are not enabled" warning in STL headers
    #pragma warning (push)
    #pragma warning (disable: 4530)
#endif

#include <vector>

#if !TBB_USE_EXCEPTIONS && _MSC_VER
    #pragma warning (pop)
#endif

const size_t COUNT_ELEM_CALLOC = 2;
const int COUNT_TESTS = 1000;
const int COUNT_ELEM = 25000;
const size_t MAX_SIZE = 1000;
const int COUNTEXPERIMENT = 10000;

const char strError[]="failed";
const char strOk[]="done";

typedef unsigned int UINT;
typedef unsigned char UCHAR;
typedef unsigned long DWORD;
typedef unsigned char BYTE;


typedef void* TestMalloc(size_t size);
typedef void* TestCalloc(size_t num, size_t size);
typedef void* TestRealloc(void* memblock, size_t size);
typedef void  TestFree(void* memblock);
typedef int   TestPosixMemalign(void **memptr, size_t alignment, size_t size);
typedef void* TestAlignedMalloc(size_t size, size_t alignment);
typedef void* TestAlignedRealloc(void* memblock, size_t size, size_t alignment);
typedef void  TestAlignedFree(void* memblock);

TestMalloc*  Tmalloc;
TestCalloc*  Tcalloc;
TestRealloc* Trealloc;
TestFree*    Tfree;
TestAlignedFree* Taligned_free;
// call alignment-related function via pointer and check result's alignment
int   Tposix_memalign(void **memptr, size_t alignment, size_t size);
void* Taligned_malloc(size_t size, size_t alignment);
void* Taligned_realloc(void* memblock, size_t size, size_t alignment);

// pointers to alignment-related functions used while testing
TestPosixMemalign*  Rposix_memalign;
TestAlignedMalloc*  Raligned_malloc;
TestAlignedRealloc* Raligned_realloc;

bool error_occurred = false;

#if __APPLE__
// Tests that use the variable are skipped on Mac OS* X
#else
static bool perProcessLimits = true;
#endif

const size_t POWERS_OF_2 = 20;

#if __linux__  && __ia64__
/* Can't use Intel compiler intrinsic due to internal error reported by
   10.1 compiler */
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

int32_t __TBB_machine_fetchadd4__TBB_full_fence (volatile void *ptr, int32_t value)
{
    pthread_mutex_lock(&counter_mutex);
    int32_t result = *(int32_t*)ptr;
    *(int32_t*)ptr = result + value;
    pthread_mutex_unlock(&counter_mutex);
    return result;
}

void __TBB_machine_pause(int32_t /*delay*/) {}

#elif (_WIN32||_WIN64) && defined(_M_AMD64) && !__MINGW64__

void __TBB_machine_pause(__int32 /*delay*/ ) {}

#endif

struct MemStruct
{
    void* Pointer;
    UINT Size;

    MemStruct() : Pointer(NULL), Size(0) {}
    MemStruct(void* Pointer, UINT Size) : Pointer(Pointer), Size(Size) {}
};

class CMemTest: NoAssign
{
    UINT CountErrors;
    bool FullLog;
    Harness::SpinBarrier *limitBarrier;
    static bool firstTime;

public:
    CMemTest(Harness::SpinBarrier *limitBarrier, bool isVerbose=false) :
        CountErrors(0), limitBarrier(limitBarrier)
        {
            srand((UINT)time(NULL));
            FullLog=isVerbose;
            rand();
        }
    void InvariantDataRealloc(bool aligned); //realloc does not change data
    void NULLReturn(UINT MinSize, UINT MaxSize, int total_threads); // NULL pointer + check errno
    void UniquePointer(); // unique pointer - check with padding
    void AddrArifm(); // unique pointer - check with pointer arithmetic
    bool ShouldReportError();
    void Free_NULL(); // 
    void Zerofilling(); // check if arrays are zero-filled
    void TestAlignedParameters();
    void RunAllTests(int total_threads);
    ~CMemTest() {}
};

class Limit {
    int limit;
public:
    Limit(int limit) : limit(limit) {}
    void operator() () const {
        limitMem(limit);
    }
};

int argC;
char** argV;

struct RoundRobin: NoAssign {
    const long number_of_threads;
    mutable CMemTest test;

    RoundRobin( long p, Harness::SpinBarrier *limitBarrier, bool verbose ) :
        number_of_threads(p), test(limitBarrier, verbose) {}
    void operator()( int /*id*/ ) const 
        {
            test.RunAllTests(number_of_threads);
        }
};

bool CMemTest::firstTime = true;

static void setSystemAllocs()
{
    Tmalloc=malloc;
    Trealloc=realloc;
    Tcalloc=calloc;
    Tfree=free;
#if (_WIN32 || _WIN64) && !__MINGW64__ && !__MINGW32__
    Raligned_malloc=_aligned_malloc;
    Raligned_realloc=_aligned_realloc;
    Taligned_free=_aligned_free;
    Rposix_memalign=0;
#elif  __APPLE__ || __sun || __MINGW64__ || __MINGW32__ //  Max OS X MinGW and Solaris don't have posix_memalign
    Raligned_malloc=0;
    Raligned_realloc=0;
    Taligned_free=0;
    Rposix_memalign=0;
#else 
    Raligned_malloc=0;
    Raligned_realloc=0;
    Taligned_free=0;
    Rposix_memalign=posix_memalign;
#endif
}

// check that realloc works as free and as malloc
void ReallocParam()
{
    const int ITERS = 1000;
    int i;
    void *bufs[ITERS];

    bufs[0] = Trealloc(NULL, 30*MByte);
    ASSERT(bufs[0], "Can't get memory to start the test.");
  
    for (i=1; i<ITERS; i++)
    {
        bufs[i] = Trealloc(NULL, 30*MByte);
        if (NULL == bufs[i])
            break;
    }
    ASSERT(i<ITERS, "Limits should be decreased for the test to work.");
  
    Trealloc(bufs[0], 0);
    /* There is a race for the free space between different threads at 
       this point. So, have to run the test sequentially.
    */
    bufs[0] = Trealloc(NULL, 30*MByte);
    ASSERT(bufs[0], NULL);
  
    for (int j=0; j<i; j++)
        Trealloc(bufs[j], 0);
}

HARNESS_EXPORT
int main(int argc, char* argv[]) {
    argC=argc;
    argV=argv;
    MaxThread = MinThread = 1;
    Tmalloc=scalable_malloc;
    Trealloc=scalable_realloc;
    Tcalloc=scalable_calloc;
    Tfree=scalable_free;
    Rposix_memalign=scalable_posix_memalign;
    Raligned_malloc=scalable_aligned_malloc;
    Raligned_realloc=scalable_aligned_realloc;
    Taligned_free=scalable_aligned_free;

    // check if we were called to test standard behavior
    for (int i=1; i< argc; i++) {
        if (strcmp((char*)*(argv+i),"-s")==0)
        {
            setSystemAllocs();
            argC--;
            break;
        }
    }

    ParseCommandLine( argC, argV );
#if __linux__
    /* According to man pthreads 
       "NPTL threads do not share resource limits (fixed in kernel 2.6.10)".
       Use per-threads limits for affected systems.
     */
    if ( LinuxKernelVersion() < 2*1000000 + 6*1000 + 10)
        perProcessLimits = false;
#endif    
    //-------------------------------------
#if __APPLE__
    /* Skip due to lack of memory limit enforcing under Mac OS X. */
#else
    limitMem(200);
    ReallocParam();
    limitMem(0);
#endif
    
//for linux and dynamic runtime errno is used to check allocator fuctions
//check if library compiled with /MD(d) and we can use errno
#if _MSC_VER 
#if defined(_MT) && defined(_DLL) //check errno if test itself compiled with /MD(d) only
    #pragma comment(lib, "version.lib")
    char*  version_info_block = NULL;
    int version_info_block_size; 
    LPVOID comments_block = NULL;
    UINT comments_block_size;
#ifdef _DEBUG
#define __TBBMALLOCDLL "tbbmalloc_debug.dll"
#else  //_DEBUG
#define __TBBMALLOCDLL "tbbmalloc.dll"
#endif //_DEBUG
    version_info_block_size = GetFileVersionInfoSize( __TBBMALLOCDLL, (LPDWORD)&version_info_block_size );
    if( version_info_block_size 
        && ((version_info_block = (char*)malloc(version_info_block_size)) != NULL)
        && GetFileVersionInfo(  __TBBMALLOCDLL, NULL, version_info_block_size, version_info_block )
        && VerQueryValue( version_info_block, "\\StringFileInfo\\000004b0\\Comments", &comments_block, &comments_block_size )
        && strstr( (char*)comments_block, "/MD" )
        ){
            __tbb_test_errno = true;
     }
     if( version_info_block ) free( version_info_block );
#endif // defined(_MT) && defined(_DLL)
#else  // _MSC_VER
    __tbb_test_errno = true;
#endif // _MSC_VER

    for( int p=MaxThread; p>=MinThread; --p ) {
        REMARK("testing with %d threads\n", p );
        Harness::SpinBarrier *barrier = new Harness::SpinBarrier(p);
        NativeParallelFor( p, RoundRobin(p, barrier, Verbose) );
        delete barrier;
    }
    if( !error_occurred ) 
        REPORT("done\n");
    return 0;
}

struct TestStruct
{
    DWORD field1:2;
    DWORD field2:6;
    double field3;
    UCHAR field4[100];
    TestStruct* field5;
//  std::string field6;
    std::vector<int> field7;
    double field8;
    bool IsZero() {
        int wordSz = sizeof(TestStruct) / sizeof(intptr_t);
        int tailSz = sizeof(TestStruct) % sizeof(intptr_t);

        intptr_t *buf =(intptr_t*)this;
        char *bufTail =(char*) (buf+wordSz);

        for (int i=0; i<wordSz; i++)
            if (buf[i]) return false;
        for (int i=0; i<tailSz; i++)
            if (bufTail[i]) return false;
        return true;
    }
};

int Tposix_memalign(void **memptr, size_t alignment, size_t size)
{
    int ret = Rposix_memalign(memptr, alignment, size);
    if (0 == ret)
        ASSERT(0==((uintptr_t)*memptr & (alignment-1)),
               "allocation result should be aligned");
    return ret;
}
void* Taligned_malloc(size_t size, size_t alignment)
{
    void *ret = Raligned_malloc(size, alignment);
    if (0 != ret)
        ASSERT(0==((uintptr_t)ret & (alignment-1)),
               "allocation result should be aligned");
    return ret;
}
void* Taligned_realloc(void* memblock, size_t size, size_t alignment)
{
    void *ret = Raligned_realloc(memblock, size, alignment);
    if (0 != ret)
        ASSERT(0==((uintptr_t)ret & (alignment-1)),
               "allocation result should be aligned");
    return ret;
}

inline size_t choose_random_alignment() {
    return sizeof(void*)<<(rand() % POWERS_OF_2);
}

void CMemTest::InvariantDataRealloc(bool aligned)
{
    size_t size, sizeMin;
    CountErrors=0;
    if (FullLog) REPORT("\nInvariant data by realloc....");
    UCHAR* pchar;
    sizeMin=size=rand()%MAX_SIZE+10;
    pchar = aligned?
        (UCHAR*)Taligned_realloc(NULL,size,choose_random_alignment())
        : (UCHAR*)Trealloc(NULL,size);
    if (NULL == pchar)
        return;
    for (size_t k=0; k<size; k++)
        pchar[k]=(UCHAR)k%255+1;
    for (int i=0; i<COUNTEXPERIMENT; i++)
    {
        size=rand()%MAX_SIZE+10;
        UCHAR *pcharNew = aligned?
            (UCHAR*)Taligned_realloc(pchar,size, choose_random_alignment())
            : (UCHAR*)Trealloc(pchar,size);
        if (NULL == pcharNew)
            continue;
        pchar = pcharNew;
        sizeMin=size<sizeMin ? size : sizeMin;
        for (size_t k=0; k<sizeMin; k++)
            if (pchar[k] != (UCHAR)k%255+1)
            {
                CountErrors++;
                if (ShouldReportError())
                {
                    REPORT("stand '%c', must stand '%c'\n",pchar[k],(UCHAR)k%255+1);
                    REPORT("error: data changed (at %llu, SizeMin=%llu)\n",
                           (long long unsigned)k,(long long unsigned)sizeMin);
                }
            }
    }
    if (aligned)
        Taligned_realloc(pchar,0,choose_random_alignment());
    else
        Trealloc(pchar,0);
    if (CountErrors) REPORT("%s\n",strError);
    else if (FullLog) REPORT("%s\n",strOk);
    error_occurred |= ( CountErrors>0 ) ;
    //REPORT("end check\n");
}

struct PtrSize {
    void  *ptr;
    size_t size;
};

static int cmpAddrs(const void *p1, const void *p2)
{
    const PtrSize *a = (const PtrSize *)p1;
    const PtrSize *b = (const PtrSize *)p2;

    return a->ptr < b->ptr ? -1 : ( a->ptr == b->ptr ? 0 : 1);
}

void CMemTest::AddrArifm()
{
    PtrSize *arr = (PtrSize*)Tmalloc(COUNT_ELEM*sizeof(PtrSize));

    if (FullLog) REPORT("\nUnique pointer using Address arithmetics\n");
    if (FullLog) REPORT("malloc....");
    ASSERT(arr, NULL);
    for (int i=0; i<COUNT_ELEM; i++)
    {
        arr[i].size=rand()%MAX_SIZE;
        arr[i].ptr=Tmalloc(arr[i].size);
    }
    qsort(arr, COUNT_ELEM, sizeof(PtrSize), cmpAddrs);

    for (int i=0; i<COUNT_ELEM-1; i++)
    {
        if (NULL!=arr[i].ptr && NULL!=arr[i+1].ptr)
            ASSERT((uintptr_t)arr[i].ptr+arr[i].size <= (uintptr_t)arr[i+1].ptr,
                   "intersection detected");
    }
    //----------------------------------------------------------------
    if (FullLog) REPORT("realloc....");
    for (int i=0; i<COUNT_ELEM; i++)
    {
        size_t count=arr[i].size*2;
        void *tmpAddr=Trealloc(arr[i].ptr,count);
        if (NULL!=tmpAddr) {
            arr[i].ptr = tmpAddr;
            arr[i].size = count;
        } else if (count==0) { // becasue realloc(..., 0) works as free
            arr[i].ptr = NULL;
            arr[i].size = 0;
        }
    }
    qsort(arr, COUNT_ELEM, sizeof(PtrSize), cmpAddrs);

    for (int i=0; i<COUNT_ELEM-1; i++)
    {
        if (NULL!=arr[i].ptr && NULL!=arr[i+1].ptr)
            ASSERT((uintptr_t)arr[i].ptr+arr[i].size <= (uintptr_t)arr[i+1].ptr,
                   "intersection detected");
    }
    for (int i=0; i<COUNT_ELEM; i++)
    {
        Tfree(arr[i].ptr);
    }
    //-------------------------------------------
    if (FullLog) REPORT("calloc....");
    for (int i=0; i<COUNT_ELEM; i++)
    {
        arr[i].size=rand()%MAX_SIZE;
        arr[i].ptr=Tcalloc(arr[i].size,1);
    }
    qsort(arr, COUNT_ELEM, sizeof(PtrSize), cmpAddrs);

    for (int i=0; i<COUNT_ELEM-1; i++)
    {
        if (NULL!=arr[i].ptr && NULL!=arr[i+1].ptr)
            ASSERT((uintptr_t)arr[i].ptr+arr[i].size <= (uintptr_t)arr[i+1].ptr,
                   "intersection detected");
    }
    for (int i=0; i<COUNT_ELEM; i++)
    {
        Tfree(arr[i].ptr);
    }
    Tfree(arr);
}

void CMemTest::Zerofilling()
{
    TestStruct* TSMas;
    size_t CountElement;
    CountErrors=0;
    if (FullLog) REPORT("\nzeroings elements of array....");
    //test struct
    for (int i=0; i<COUNTEXPERIMENT; i++)
    {
        CountElement=rand()%MAX_SIZE;
        TSMas=(TestStruct*)Tcalloc(CountElement,sizeof(TestStruct));
        if (NULL == TSMas)
            continue;
        for (size_t j=0; j<CountElement; j++)
        {
            if (!(TSMas+j)->IsZero())
            {
                CountErrors++;
                if (ShouldReportError()) REPORT("detect nonzero element at TestStruct\n");
            }
        }
        Tfree(TSMas);
    }
    if (CountErrors) REPORT("%s\n",strError);
    else if (FullLog) REPORT("%s\n",strOk);
    error_occurred |= ( CountErrors>0 ) ;
}

#if !__APPLE__
void CMemTest::NULLReturn(UINT MinSize, UINT MaxSize, int total_threads)
{
    // find size to guarantee getting NULL for 1024 B allocations
    const int MAXNUM_1024 = (200+50)*1024;

    std::vector<MemStruct> PointerList;
    void *tmp;
    CountErrors=0;
    int CountNULL, num_1024;
    if (FullLog) REPORT("\nNULL return & check errno:\n");
    UINT Size;
    Limit limit_200M(200*total_threads), no_limit(0);
    void **buf_1024 = (void**)Tmalloc(MAXNUM_1024*sizeof(void*));

    ASSERT(buf_1024, NULL);
    /* We must have space for pointers when memory limit is hit. 
       Reserve enough for the worst case. 
    */
    PointerList.reserve(200*MByte/MinSize);

    /* There is a bug in the specific verion of GLIBC (2.5-12) shipped 
       with RHEL5 that leads to erroneous working of the test 
       on Intel64 and IPF systems when setrlimit-related part is enabled.
       Switching to GLIBC 2.5-18 from RHEL5.1 resolved the issue.
     */
    if (perProcessLimits)
        limitBarrier->wait(limit_200M);
    else
        limitMem(200);

    /* regression test against the bug in allocator when it dereference NULL 
       while lack of memory 
    */
    for (num_1024=0; num_1024<MAXNUM_1024; num_1024++) {
        buf_1024[num_1024] = Tcalloc(1024, 1);
        if (! buf_1024[num_1024]) {
            ASSERT_ERRNO(errno == ENOMEM, NULL);
            break;
        }
    }
    for (int i=0; i<num_1024; i++)
        Tfree(buf_1024[i]);
    Tfree(buf_1024);

    do {
        Size=rand()%(MaxSize-MinSize)+MinSize;
        tmp=Tmalloc(Size);
        if (tmp != NULL)
        {
            memset(tmp, 0, Size);
            PointerList.push_back(MemStruct(tmp, Size));
        }
    } while(tmp != NULL);
    ASSERT_ERRNO(errno == ENOMEM, NULL);
    if (FullLog) REPORT("\n");

    // preparation complete, now running tests
    // malloc
    if (FullLog) REPORT("malloc....");
    CountNULL = 0;
    while (CountNULL==0)
        for (int j=0; j<COUNT_TESTS; j++)
        {
            Size=rand()%(MaxSize-MinSize)+MinSize;
            errno = ENOMEM+j+1;
            tmp=Tmalloc(Size);
            if (tmp == NULL)
            {
                CountNULL++;
                if ( CHECK_ERRNO(errno != ENOMEM) ) {
                    CountErrors++;
                    if (ShouldReportError()) REPORT("NULL returned, error: errno (%d) != ENOMEM\n", errno);
                }
            }
            else
            {
                // Technically, if malloc returns a non-NULL pointer, it is allowed to set errno anyway.
                // However, on most systems it does not set errno.
                bool known_issue = false;
#if __linux__
                if( CHECK_ERRNO(errno==ENOMEM) ) known_issue = true;
#endif /* __linux__ */
                if ( CHECK_ERRNO(errno != ENOMEM+j+1) && !known_issue) {
                    CountErrors++;
                    if (ShouldReportError()) REPORT("error: errno changed to %d though valid pointer was returned\n", errno);
                }
                memset(tmp, 0, Size);
                PointerList.push_back(MemStruct(tmp, Size));
            }
        }
    if (FullLog) REPORT("end malloc\n");
    if (CountErrors) REPORT("%s\n",strError);
    else if (FullLog) REPORT("%s\n",strOk);
    error_occurred |= ( CountErrors>0 ) ;

    CountErrors=0;
    //calloc
    if (FullLog) REPORT("calloc....");
    CountNULL = 0;
    while (CountNULL==0)
        for (int j=0; j<COUNT_TESTS; j++)
        {
            Size=rand()%(MaxSize-MinSize)+MinSize;
            errno = ENOMEM+j+1;
            tmp=Tcalloc(COUNT_ELEM_CALLOC,Size);  
            if (tmp == NULL)
            {
                CountNULL++;
                if ( CHECK_ERRNO(errno != ENOMEM) ){
                    CountErrors++;
                    if (ShouldReportError()) REPORT("NULL returned, error: errno(%d) != ENOMEM\n", errno);
                }
            }
            else
            {
                // Technically, if calloc returns a non-NULL pointer, it is allowed to set errno anyway.
                // However, on most systems it does not set errno.
                bool known_issue = false;
#if __linux__
                if( CHECK_ERRNO(errno==ENOMEM) ) known_issue = true;
#endif /* __linux__ */
                if ( CHECK_ERRNO(errno != ENOMEM+j+1) && !known_issue ) {
                    CountErrors++;
                    if (ShouldReportError()) REPORT("error: errno changed to %d though valid pointer was returned\n", errno);
                }      
                PointerList.push_back(MemStruct(tmp, Size));
            }
        }
    if (FullLog) REPORT("end calloc\n");
    if (CountErrors) REPORT("%s\n",strError);
    else if (FullLog) REPORT("%s\n",strOk);
    error_occurred |= ( CountErrors>0 ) ;
    CountErrors=0;
    if (FullLog) REPORT("realloc....");
    CountNULL = 0;
    if (PointerList.size() > 0)
        while (CountNULL==0)
            for (size_t i=0; i<(size_t)COUNT_TESTS && i<PointerList.size(); i++)
            {
                errno = 0;
                tmp=Trealloc(PointerList[i].Pointer,PointerList[i].Size*2);
                if (PointerList[i].Pointer == tmp) // the same place
                {
                    bool known_issue = false;
#if __linux__
                    if( errno==ENOMEM ) known_issue = true;
#endif /* __linux__ */
                    if (errno != 0 && !known_issue) {
                        CountErrors++;
                        if (ShouldReportError()) REPORT("valid pointer returned, error: errno not kept\n");
                    }      
                    PointerList[i].Size *= 2;
                }
                else if (tmp != PointerList[i].Pointer && tmp != NULL) // another place
                {
                    bool known_issue = false;
#if __linux__
                    if( errno==ENOMEM ) known_issue = true;
#endif /* __linux__ */
                    if (errno != 0 && !known_issue) {
                        CountErrors++;
                        if (ShouldReportError()) REPORT("valid pointer returned, error: errno not kept\n");
                    }
                    // newly allocated area have to be zeroed
                    memset((char*)tmp + PointerList[i].Size, 0, PointerList[i].Size);
                    PointerList[i].Pointer = tmp;
                    PointerList[i].Size *= 2;
                }
                else if (tmp == NULL)
                {
                    CountNULL++;
                    if ( CHECK_ERRNO(errno != ENOMEM) )
                    {
                        CountErrors++;
                        if (ShouldReportError()) REPORT("NULL returned, error: errno(%d) != ENOMEM\n", errno);
                    }
                    // check data integrity
                    BYTE *zer=(BYTE*)PointerList[i].Pointer;
                    for (UINT k=0; k<PointerList[i].Size; k++)
                        if (zer[k] != 0)
                        {
                            CountErrors++;
                            if (ShouldReportError()) REPORT("NULL returned, error: data changed\n");
                        }
                }
            }
    if (FullLog) REPORT("realloc end\n");
    if (CountErrors) REPORT("%s\n",strError);
    else if (FullLog) REPORT("%s\n",strOk);
    error_occurred |= ( CountErrors>0 ) ;
    for (UINT i=0; i<PointerList.size(); i++)
    {
        Tfree(PointerList[i].Pointer);
    }

    if (perProcessLimits)
        limitBarrier->wait(no_limit);
    else
        limitMem(0);
}
#endif /* #if __APPLE__ */

void CMemTest::UniquePointer()
{
    CountErrors=0;
    int **MasPointer = (int **)Tmalloc(sizeof(int*)*COUNT_ELEM);
    size_t *MasCountElem = (size_t*)Tmalloc(sizeof(size_t)*COUNT_ELEM);
    if (FullLog) REPORT("\nUnique pointer using 0\n");
    ASSERT(MasCountElem && MasPointer, NULL);
    //
    //-------------------------------------------------------
    //malloc
    for (int i=0; i<COUNT_ELEM; i++)
    {
        MasCountElem[i]=rand()%MAX_SIZE;
        MasPointer[i]=(int*)Tmalloc(MasCountElem[i]*sizeof(int));
        if (NULL == MasPointer[i])
            MasCountElem[i]=0;
        for (UINT j=0; j<MasCountElem[i]; j++)
            *(MasPointer[i]+j)=0;
    }
    if (FullLog) REPORT("malloc....");
    for (UINT i=0; i<COUNT_ELEM-1; i++)
    {
        for (UINT j=0; j<MasCountElem[i]; j++)
        {
            if (*(*(MasPointer+i)+j)!=0)
            {
                CountErrors++;
                if (ShouldReportError()) REPORT("error, detect 1 with 0x%p\n",(*(MasPointer+i)+j));
            }
            *(*(MasPointer+i)+j)+=1;
        }
    }
    if (CountErrors) REPORT("%s\n",strError);
    else if (FullLog) REPORT("%s\n",strOk);
    error_occurred |= ( CountErrors>0 ) ;
    //----------------------------------------------------------
    //calloc
    for (int i=0; i<COUNT_ELEM; i++)
        Tfree(MasPointer[i]);
    CountErrors=0;
    for (long i=0; i<COUNT_ELEM; i++)
    {
        MasPointer[i]=(int*)Tcalloc(MasCountElem[i]*sizeof(int),2);
        if (NULL == MasPointer[i])
            MasCountElem[i]=0;
    }
    if (FullLog) REPORT("calloc....");
    for (int i=0; i<COUNT_ELEM-1; i++)
    {
        for (UINT j=0; j<*(MasCountElem+i); j++)
        {
            if (*(*(MasPointer+i)+j)!=0)
            {
                CountErrors++;
                if (ShouldReportError()) REPORT("error, detect 1 with 0x%p\n",(*(MasPointer+i)+j));
            }
            *(*(MasPointer+i)+j)+=1;
        }
    }
    if (CountErrors) REPORT("%s\n",strError);
    else if (FullLog) REPORT("%s\n",strOk);
    error_occurred |= ( CountErrors>0 ) ;
    //---------------------------------------------------------
    //realloc
    CountErrors=0;
    for (int i=0; i<COUNT_ELEM; i++)
    {
        MasCountElem[i]*=2;
        *(MasPointer+i)=
            (int*)Trealloc(*(MasPointer+i),MasCountElem[i]*sizeof(int));
        if (NULL == MasPointer[i])
            MasCountElem[i]=0;
        for (UINT j=0; j<MasCountElem[i]; j++)
            *(*(MasPointer+i)+j)=0;
    }
    if (FullLog) REPORT("realloc....");
    for (int i=0; i<COUNT_ELEM-1; i++)
    {
        for (UINT j=0; j<*(MasCountElem+i); j++)
        {
            if (*(*(MasPointer+i)+j)!=0)
            {
                CountErrors++;
            }
            *(*(MasPointer+i)+j)+=1;
        }
    }
    if (CountErrors) REPORT("%s\n",strError);
    else if (FullLog) REPORT("%s\n",strOk);
    error_occurred |= ( CountErrors>0 ) ;
    for (int i=0; i<COUNT_ELEM; i++)
        Tfree(MasPointer[i]);
    Tfree(MasCountElem);
    Tfree(MasPointer);
}

bool CMemTest::ShouldReportError()
{
    if (FullLog)
        return true;
    else
        if (firstTime) {
            firstTime = false;
            return true;
        } else
            return false;
}

void CMemTest::Free_NULL()
{
    CountErrors=0;
    if (FullLog) REPORT("\ncall free with parameter NULL....");
    errno = 0;
    for (int i=0; i<COUNTEXPERIMENT; i++)
    {
        Tfree(NULL);
        if (errno != 0)
        {
            CountErrors++;
            if (ShouldReportError()) REPORT("error is found by a call free with parameter NULL\n");
        }
    }
    if (CountErrors) REPORT("%s\n",strError);
    else if (FullLog) REPORT("%s\n",strOk);
    error_occurred |= ( CountErrors>0 ) ;
}

void CMemTest::TestAlignedParameters()
{
    void *memptr;
    int ret;

    if (Rposix_memalign) {
        // alignment isn't power of 2
        for (int bad_align=3; bad_align<16; bad_align++)
            if (bad_align&(bad_align-1)) {
                ret = Tposix_memalign(NULL, bad_align, 100);
                ASSERT(EINVAL==ret, NULL);
            }
    
        memptr = &ret;
        ret = Tposix_memalign(&memptr, 5*sizeof(void*), 100);
        ASSERT(memptr == &ret,
               "memptr should not be changed after unsuccesful call");
        ASSERT(EINVAL==ret, NULL);
    
        // alignment is power of 2, but not a multiple of sizeof(void *),
        // we expect that sizeof(void*) > 2
        ret = Tposix_memalign(NULL, 2, 100);
        ASSERT(EINVAL==ret, NULL);
    }
    if (Raligned_malloc) {
        // alignment isn't power of 2
        for (int bad_align=3; bad_align<16; bad_align++)
            if (bad_align&(bad_align-1)) {
                memptr = Taligned_malloc(100, bad_align);
                ASSERT(NULL==memptr, NULL);
                ASSERT_ERRNO(EINVAL==errno, NULL);
            }
    
        // size is zero
        memptr = Taligned_malloc(0, 16);
        ASSERT(NULL==memptr, "size is zero, so must return NULL");
        ASSERT_ERRNO(EINVAL==errno, NULL);
    }
    if (Taligned_free) {
        // NULL pointer is OK to free
        errno = 0;
        Taligned_free(NULL);
        /* As there is no return value for free, strictly speaking we can't 
           check errno here. But checked implementations obey the assertion.
        */
        ASSERT_ERRNO(0==errno, NULL);
    }
    if (Raligned_realloc) {
        for (int i=1; i<20; i++) {
            // checks that calls work correctly in presence of non-zero errno
            errno = i;
            void *ptr = Taligned_malloc(i*10, 128);
            ASSERT(NULL!=ptr, NULL);
            ASSERT_ERRNO(0!=errno, NULL);
            // if size is zero and pointer is not NULL, works like free
            memptr = Taligned_realloc(ptr, 0, 64);
            ASSERT(NULL==memptr, NULL);
            ASSERT_ERRNO(0!=errno, NULL);
        }
        // alignment isn't power of 2
        for (int bad_align=3; bad_align<16; bad_align++)
            if (bad_align&(bad_align-1)) {
                void *ptr = &bad_align;
                memptr = Taligned_realloc(&ptr, 100, bad_align);
                ASSERT(NULL==memptr, NULL);
                ASSERT(&bad_align==ptr, NULL);
                ASSERT_ERRNO(EINVAL==errno, NULL);
            }
    }
}

void CMemTest::RunAllTests(int total_threads)
{
    Zerofilling();
    Free_NULL();
    InvariantDataRealloc(/*aligned=*/false);
    if (Raligned_realloc)
        InvariantDataRealloc(/*aligned=*/true);
    TestAlignedParameters();
#if __APPLE__
    REPORT("Known issue: some tests are skipped on Mac OS* X\n");
#else
    UniquePointer();
    AddrArifm();
#if !__TBB_MIC_NATIVE
    NULLReturn(1*MByte,100*MByte,total_threads);
#endif
#endif
    if (FullLog) REPORT("All tests ended\nclearing memory...");
}
