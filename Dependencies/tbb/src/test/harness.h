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

// Declarations for rock-bottom simple test harness.
// Just include this file to use it.
// Every test is presumed to have a command line of the form "test [-v] [MinThreads[:MaxThreads]]"
// The default for MinThreads is 1, for MaxThreads 4.
// The defaults can be overridden by defining macros HARNESS_DEFAULT_MIN_THREADS
// and HARNESS_DEFAULT_MAX_THREADS before including harness.h

#ifndef tbb_tests_harness_H
#define tbb_tests_harness_H

#include "tbb/tbb_config.h"

namespace Harness {
    enum TestResult {
        Done,
        Skipped
    };
}

//! Entry point to a TBB unit test application
/** It MUST be defined by the test application.
    
    If HARNESS_NO_PARSE_COMMAND_LINE macro was not explicitly set before including harness.h,
    then global variables Verbose, MinThread, and MaxThread will be available and 
    initialized when it is called.

    Returns Harness::Done when the tests passed successfully. When the test fail, it must 
    not return, calling exit(errcode) or abort() instead. When the test is not supported 
    for the given platform/compiler/etc, it should return Harness::Skipped.
    
    To provide non-standard variant of main() for the test, define HARNESS_CUSTOM_MAIN
    before including harness.h **/
int TestMain ();

#define __TBB_LAMBDAS_PRESENT  ( _MSC_VER >= 1600 && !__INTEL_COMPILER || __INTEL_COMPILER > 1100 && _TBB_CPP0X )

#if defined(_MSC_VER) && _MSC_VER < 1400
    #define __TBB_EXCEPTION_TYPE_INFO_BROKEN 1
#else
    #define __TBB_EXCEPTION_TYPE_INFO_BROKEN 0
#endif

#if __SUNPRO_CC
    #include <stdlib.h>
    #include <string.h>
#else /* !__SUNPRO_CC */
    #include <cstdlib>
#if !TBB_USE_EXCEPTIONS && _MSC_VER
    // Suppress "C++ exception handler used, but unwind semantics are not enabled" warning in STL headers
    #pragma warning (push)
    #pragma warning (disable: 4530)
#endif
    #include <cstring>
#if !TBB_USE_EXCEPTIONS && _MSC_VER
    #pragma warning (pop)
#endif
#endif /* !__SUNPRO_CC */

#include <new>

    #define HARNESS_EXPORT
    #define REPORT_FATAL_ERROR REPORT

#if _WIN32||_WIN64
    #include "tbb/machine/windows_api.h"
#if _XBOX
    #undef HARNESS_NO_PARSE_COMMAND_LINE
    #define HARNESS_NO_PARSE_COMMAND_LINE 1
#endif
    #include <process.h>
#else
    #include <pthread.h>
#endif
#if __linux__
    #include <sys/utsname.h> /* for uname */
    #include <errno.h>       /* for use in LinuxKernelVersion() */
#endif

#include "harness_report.h"

#if !HARNESS_NO_ASSERT
#include "harness_assert.h"

typedef void (*test_error_extra_t)(void);
static test_error_extra_t ErrorExtraCall; 
//! Set additional handler to process failed assertions
void SetHarnessErrorProcessing( test_error_extra_t extra_call ) {
    ErrorExtraCall = extra_call;
    // TODO: add tbb::set_assertion_handler(ReportError);
}
//! Reports errors issued by failed assertions
void ReportError( const char* filename, int line, const char* expression, const char * message ) {
#if __TBB_ICL_11_1_CODE_GEN_BROKEN
    printf("%s:%d, assertion %s: %s\n", filename, line, expression, message ? message : "failed" );
#else
    REPORT_FATAL_ERROR("%s:%d, assertion %s: %s\n", filename, line, expression, message ? message : "failed" );
#endif
    if( ErrorExtraCall )
        (*ErrorExtraCall)();
#if HARNESS_TERMINATE_ON_ASSERT
    TerminateProcess(GetCurrentProcess(), 1);
#elif HARNESS_EXIT_ON_ASSERT
    exit(1);
#else
    abort();
#endif /* HARNESS_EXIT_ON_ASSERT */
}
//! Reports warnings issued by failed warning assertions
void ReportWarning( const char* filename, int line, const char* expression, const char * message ) {
    REPORT("Warning: %s:%d, assertion %s: %s\n", filename, line, expression, message ? message : "failed" );
}
#else
#define ASSERT(p,msg) ((void)0)
#define ASSERT_WARNING(p,msg) ((void)0)
#endif /* HARNESS_NO_ASSERT */

#if !HARNESS_NO_PARSE_COMMAND_LINE

//! Controls level of commentary printed via printf-like REMARK() macro.
/** If true, makes the test print commentary.  If false, test should print "done" and nothing more. */
static bool Verbose;

#ifndef HARNESS_DEFAULT_MIN_THREADS
    #define HARNESS_DEFAULT_MIN_THREADS 1
#endif

//! Minimum number of threads
static int MinThread = HARNESS_DEFAULT_MIN_THREADS;

#ifndef HARNESS_DEFAULT_MAX_THREADS
    #define HARNESS_DEFAULT_MAX_THREADS 4
#endif

//! Maximum number of threads
static int MaxThread = HARNESS_DEFAULT_MAX_THREADS;

//! Parse command line of the form "name [-v] [MinThreads[:MaxThreads]]"
/** Sets Verbose, MinThread, and MaxThread accordingly.
    The nthread argument can be a single number or a range of the form m:n.
    A single number m is interpreted as if written m:m. 
    The numbers must be non-negative.  
    Clients often treat the value 0 as "run sequentially." */
static void ParseCommandLine( int argc, char* argv[] ) {
    if( !argc ) REPORT("Command line with 0 arguments\n");
    int i = 1;  
    if( i<argc ) {
        if( strncmp( argv[i], "-v", 2 )==0 ) {
            Verbose = true;
            ++i;
        }
    }
    if( i<argc ) {
        char* endptr;
        MinThread = strtol( argv[i], &endptr, 0 );
        if( *endptr==':' )
            MaxThread = strtol( endptr+1, &endptr, 0 );
        else if( *endptr=='\0' ) 
            MaxThread = MinThread;
        if( *endptr!='\0' ) {
            REPORT_FATAL_ERROR("garbled nthread range\n");
            exit(1);
        }    
        if( MinThread<0 ) {
            REPORT_FATAL_ERROR("nthread must be nonnegative\n");
            exit(1);
        }
        if( MaxThread<MinThread ) {
            REPORT_FATAL_ERROR("nthread range is backwards\n");
            exit(1);
        }
        ++i;
    }
#if __TBB_STDARGS_BROKEN
    if ( !argc )
        argc = 1;
    else {
        while ( i < argc && argv[i][0] == 0 )
            ++i;
    }
#endif /* __TBB_STDARGS_BROKEN */
    if( i!=argc ) {
        REPORT_FATAL_ERROR("Usage: %s [-v] [nthread|minthread:maxthread]\n", argv[0] );
        exit(1);
    }
}
#endif /* HARNESS_NO_PARSE_COMMAND_LINE */

#if !HARNESS_CUSTOM_MAIN

HARNESS_EXPORT
#if HARNESS_NO_PARSE_COMMAND_LINE
int main() {
#else
int main(int argc, char* argv[]) {
    ParseCommandLine( argc, argv );
#endif
    int res = TestMain ();
    ASSERT( res==Harness::Done || res==Harness::Skipped, "Wrong return code by TestMain");
    REPORT( res==Harness::Done ? "done\n" : "skip\n" );
    return 0;
}

#endif /* !HARNESS_CUSTOM_MAIN */

//! Base class for prohibiting compiler-generated operator=
class NoAssign {
    //! Assignment not allowed
    void operator=( const NoAssign& );
public:
#if __GNUC__
    //! Explicitly define default construction, because otherwise gcc issues gratuitous warning.
    NoAssign() {}
#endif /* __GNUC__ */
};

//! Base class for prohibiting compiler-generated copy constructor or operator=
class NoCopy: NoAssign {
    //! Copy construction not allowed  
    NoCopy( const NoCopy& );
public:
    NoCopy() {}
};

//! For internal use by template function NativeParallelFor
template<typename Index, typename Body>
class NativeParallelForTask: NoCopy {
public:
    NativeParallelForTask( Index index_, const Body& body_ ) :
        index(index_),
        body(body_)
    {}

    //! Start task
    void start() {
#if _WIN32||_WIN64
        unsigned thread_id;
        thread_handle = (HANDLE)_beginthreadex( NULL, 0, thread_function, this, 0, &thread_id );
        ASSERT( thread_handle!=0, "NativeParallelFor: _beginthreadex failed" );
#else
#if __ICC==1100
    #pragma warning (push)
    #pragma warning (disable: 2193)
#endif /* __ICC==1100 */
        // Some machines may have very large hard stack limit. When the test is 
        // launched by make, the default stack size is set to the hard limit, and 
        // calls to pthread_create fail with out-of-memory error. 
        // Therefore we set the stack size explicitly (as for TBB worker threads).
        const size_t MByte = 1<<20;
#if __i386__||__i386
        const size_t stack_size = 1*MByte;
#elif __x86_64__
        const size_t stack_size = 2*MByte;
#else
        const size_t stack_size = 4*MByte;
#endif
        pthread_attr_t attr_stack;
        int status = pthread_attr_init(&attr_stack);
        ASSERT(0==status, "NativeParallelFor: pthread_attr_init failed");
        status = pthread_attr_setstacksize( &attr_stack, stack_size );
        ASSERT(0==status, "NativeParallelFor: pthread_attr_setstacksize failed");
        status = pthread_create(&thread_id, &attr_stack, thread_function, this);
        ASSERT(0==status, "NativeParallelFor: pthread_create failed");
        pthread_attr_destroy(&attr_stack);
#if __ICC==1100
    #pragma warning (pop)
#endif
#endif /* _WIN32||_WIN64 */
    }

    //! Wait for task to finish
    void wait_to_finish() {
#if _WIN32||_WIN64
        DWORD status = WaitForSingleObject( thread_handle, INFINITE );
        ASSERT( status!=WAIT_FAILED, "WaitForSingleObject failed" );
        CloseHandle( thread_handle );
#else
        int status = pthread_join( thread_id, NULL );
        ASSERT( !status, "pthread_join failed" );
#endif 
    }

private:
#if _WIN32||_WIN64
    HANDLE thread_handle;
#else
    pthread_t thread_id;
#endif

    //! Range over which task will invoke the body.
    const Index index;

    //! Body to invoke over the range.
    const Body body;

#if _WIN32||_WIN64
    static unsigned __stdcall thread_function( void* object )
#else
    static void* thread_function(void* object)
#endif
    {
        NativeParallelForTask& self = *static_cast<NativeParallelForTask*>(object);
        (self.body)(self.index);
        return 0;
    }
};

//! Execute body(i) in parallel for i in the interval [0,n).
/** Each iteration is performed by a separate thread. */
template<typename Index, typename Body>
void NativeParallelFor( Index n, const Body& body ) {
    typedef NativeParallelForTask<Index,Body> task;

    if( n>0 ) {
        // Allocate array to hold the tasks
        task* array = static_cast<task*>(operator new( n*sizeof(task) ));

        // Construct the tasks
        for( Index i=0; i!=n; ++i ) 
            new( &array[i] ) task(i,body);

        // Start the tasks
        for( Index i=0; i!=n; ++i )
            array[i].start();

        // Wait for the tasks to finish and destroy each one.
        for( Index i=n; i; --i ) {
            array[i-1].wait_to_finish();
            array[i-1].~task();
        }

        // Deallocate the task array
        operator delete(array);
    }
}

//! The function to zero-initialize arrays; useful to avoid warnings
template <typename T>
void zero_fill(void* array, size_t n) {
    memset(array, 0, sizeof(T)*n);
}

#if __SUNPRO_CC && defined(min)
#undef min
#undef max
#endif

#ifndef min
//! Utility template function returning lesser of the two values.
/** Provided here to avoid including not strict safe <algorithm>.\n
    In case operands cause signed/unsigned or size mismatch warnings it is caller's
    responsibility to do the appropriate cast before calling the function. **/
template<typename T1, typename T2>
T1 min ( const T1& val1, const T2& val2 ) {
    return val1 < val2 ? val1 : val2;
}
#endif /* !min */

#ifndef max
//! Utility template function returning greater of the two values.
/** Provided here to avoid including not strict safe <algorithm>.\n
    In case operands cause signed/unsigned or size mismatch warnings it is caller's
    responsibility to do the appropriate cast before calling the function. **/
template<typename T1, typename T2>
T1 max ( const T1& val1, const T2& val2 ) {
    return val1 < val2 ? val2 : val1;
}
#endif /* !max */

#if __linux__
inline unsigned LinuxKernelVersion()
{
    unsigned digit1, digit2, digit3;
    struct utsname utsnameBuf;
    
    if (-1 == uname(&utsnameBuf)) {
        REPORT_FATAL_ERROR("Can't call uname: errno %d\n", errno);
        exit(1);
    }
    if (3 != sscanf(utsnameBuf.release, "%u.%u.%u", &digit1, &digit2, &digit3)) {
        REPORT_FATAL_ERROR("Unable to parse OS release '%s'\n", utsnameBuf.release);
        exit(1);
    }
    return 1000000*digit1+1000*digit2+digit3;
}
#endif

namespace Harness {

#if !HARNESS_NO_ASSERT
//! Base class that asserts that no operations are made with the object after its destruction.
class NoAfterlife {
protected:
    enum state_t {
        LIVE=0x56781234,
        DEAD=0xDEADBEEF
    } m_state;

public:
    NoAfterlife() : m_state(LIVE) {}
    NoAfterlife( const NoAfterlife& src ) : m_state(LIVE) {
        ASSERT( src.IsLive(), "Constructing from the dead source" );
    }
    ~NoAfterlife() {
        ASSERT( IsLive(), "Repeated destructor call" );
        m_state = DEAD;
    }
    const NoAfterlife& operator=( const NoAfterlife& src ) {
        ASSERT( IsLive(), NULL );
        ASSERT( src.IsLive(), NULL );
        return *this;
    }
    void AssertLive() const {
        ASSERT( IsLive(), "Already dead" );
    }
    bool IsLive() const {
        return m_state == LIVE;
    }
}; // NoAfterlife
#endif /* !HARNESS_NO_ASSERT */

#if _WIN32 || _WIN64
    void Sleep ( int ms ) { ::Sleep(ms); }
#else /* !WIN */
    void Sleep ( int ms ) {
        timespec  requested = { ms / 1000, (ms % 1000)*1000000 };
        timespec  remaining = { 0, 0 };
        nanosleep(&requested, &remaining);
    }
#endif /* !WIN */

} // namespace Harness

#endif /* tbb_tests_harness_H */
