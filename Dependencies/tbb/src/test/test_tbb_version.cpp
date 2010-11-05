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

#include "tbb/tbb_stddef.h"

#include <stdio.h>
#include <stdlib.h>

#if !TBB_USE_EXCEPTIONS && _MSC_VER
    // Suppress "C++ exception handler used, but unwind semantics are not enabled" warning in STL headers
    #pragma warning (push)
    #pragma warning (disable: 4530)
#endif

#include <vector>
#include <string>
#include <utility>

#if !TBB_USE_EXCEPTIONS && _MSC_VER
    #pragma warning (pop)
#endif

#include "tbb/task_scheduler_init.h"

#define HARNESS_CUSTOM_MAIN 1
#define HARNESS_NO_PARSE_COMMAND_LINE 1
#define HARNESS_NO_MAIN_ARGS 0
#include "harness.h"

#if defined (_WIN32) || defined (_WIN64)
#define TEST_SYSTEM_COMMAND "test_tbb_version.exe @"
#define putenv _putenv
#else
#define TEST_SYSTEM_COMMAND "./test_tbb_version.exe @"
#endif

enum string_required {
    required,
    not_required
    };

typedef std::pair <std::string, string_required> string_pair;

void initialize_strings_vector(std::vector <string_pair>* vector);

const char stderr_stream[] = "version_test.err";
const char stdout_stream[] = "version_test.out";

HARNESS_EXPORT
int main(int argc, char *argv[] ) {
/* We first introduced runtime version identification in 3014 */
#if TBB_INTERFACE_VERSION>=3014 
    // For now, just test that run-time TBB version matches the compile-time version,
    // since otherwise the subsequent test of "TBB: INTERFACE VERSION" string will fail anyway.
    // We need something more clever in future.
    ASSERT(tbb::TBB_runtime_interface_version()==TBB_INTERFACE_VERSION,
           "Running with the library of different version than the test was compiled against");
#endif
    __TBB_TRY {
        FILE *stream_out;
        FILE *stream_err;   
        char psBuffer[512];
        
        if(argc>1 && argv[1][0] == '@' ) {
            stream_err = freopen( stderr_stream, "w", stderr );
            if( stream_err == NULL ){
                REPORT( "Internal test error (freopen)\n" );
                exit( 1 );
            }
            stream_out = freopen( stdout_stream, "w", stdout );
            if( stream_out == NULL ){
                REPORT( "Internal test error (freopen)\n" );
                exit( 1 );
            }
            {
                tbb::task_scheduler_init init(1);
            }
            fclose( stream_out );
            fclose( stream_err );
            exit(0);
        }
        //1st step check that output is empty if TBB_VERSION is not defined.
        if ( getenv("TBB_VERSION") ){
            REPORT( "TBB_VERSION defined, skipping step 1 (empty output check)\n" );
        }else{
            if( ( system(TEST_SYSTEM_COMMAND) ) != 0 ){
                REPORT( "Error (step 1): Internal test error\n" );
                exit( 1 );
            }
            //Checking output streams - they should be empty
            stream_err = fopen( stderr_stream, "r" );
            if( stream_err == NULL ){
                REPORT( "Error (step 1):Internal test error (stderr open)\n" );
                exit( 1 );
            }
            while( !feof( stream_err ) ) {
                if( fgets( psBuffer, 512, stream_err ) != NULL ){
                    REPORT( "Error (step 1): stderr should be empty\n" );
                    exit( 1 );
                }
            }
            fclose( stream_err );
            stream_out = fopen( stdout_stream, "r" );
            if( stream_out == NULL ){
                REPORT( "Error (step 1):Internal test error (stdout open)\n" );
                exit( 1 );
            }
            while( !feof( stream_out ) ) {
                if( fgets( psBuffer, 512, stream_out ) != NULL ){
                    REPORT( "Error (step 1): stdout should be empty\n" );
                    exit( 1 );
                }
            }
            fclose( stream_out );
        }

        //Setting TBB_VERSION in case it is not set
        if ( !getenv("TBB_VERSION") ){
            putenv(const_cast<char*>("TBB_VERSION=1"));
        }

        if( ( system(TEST_SYSTEM_COMMAND) ) != 0 ){
            REPORT( "Error (step 2):Internal test error\n" );
            exit( 1 );
        }
        //Checking pipe - it should contain version data
        std::vector <string_pair> strings_vector;
        std::vector <string_pair>::iterator strings_iterator;

        initialize_strings_vector( &strings_vector );
        strings_iterator = strings_vector.begin();

        stream_out = fopen( stdout_stream, "r" );
        if( stream_out == NULL ){
            REPORT( "Error (step 2):Internal test error (stdout open)\n" );
            exit( 1 );
        }
        while( !feof( stream_out ) ) {
            if( fgets( psBuffer, 512, stream_out ) != NULL ){
                REPORT( "Error (step 2): stdout should be empty\n" );
                exit( 1 );
            }
        }
        fclose( stream_out );

        stream_err = fopen( stderr_stream, "r" );
        if( stream_err == NULL ){
            REPORT( "Error (step 1):Internal test error (stderr open)\n" );
            exit( 1 );
        }
        
        int skip_line = 0;
        
        while( !feof( stream_err ) ) {
            if( fgets( psBuffer, 512, stream_err ) != NULL ){
                do{
                    if ( strings_iterator == strings_vector.end() ){
                        REPORT( "Error: version string dictionary ended prematurely.\n" );
                        REPORT( "No match for: \t%s", psBuffer );
                        exit( 1 );
                    }
                    if ( strstr( psBuffer, strings_iterator->first.c_str() ) == NULL ){
                        if( strings_iterator->second == required ){
                            REPORT( "Error: version strings do not match.\n" );
                            REPORT( "Expected \"%s\" not found in:\n\t%s", strings_iterator->first.c_str(), psBuffer );
                            exit( 1 );
                        }else{
                            //Do we need to print in case there is no non-required string?
                            skip_line = 1;
                        }
                    }else{
                           skip_line = 0;
                    }
                    if ( strings_iterator != strings_vector.end() ) strings_iterator ++;
                }while( skip_line );
            }
        }
        fclose( stream_err );
    } __TBB_CATCH(...) {
        ASSERT( 0,"unexpected exception" );
    }
    REPORT("done\n");
    return 0;
}


// Fill dictionary with version strings for platforms 
void initialize_strings_vector(std::vector <string_pair>* vector)
{
    vector->push_back(string_pair("TBB: VERSION\t\t3.0", required));          // check TBB_VERSION
    vector->push_back(string_pair("TBB: INTERFACE VERSION\t5003", required)); // check TBB_INTERFACE_VERSION
    vector->push_back(string_pair("TBB: BUILD_DATE", required));
    vector->push_back(string_pair("TBB: BUILD_HOST", required));
    vector->push_back(string_pair("TBB: BUILD_OS", required));
#if _WIN32||_WIN64
#if !__MINGW32__
    vector->push_back(string_pair("TBB: BUILD_CL", required));
#endif
    vector->push_back(string_pair("TBB: BUILD_COMPILER", required));
#elif __APPLE__
    vector->push_back(string_pair("TBB: BUILD_KERNEL", required));
    vector->push_back(string_pair("TBB: BUILD_GCC", required));
    vector->push_back(string_pair("TBB: BUILD_COMPILER", not_required)); //if( getenv("COMPILER_VERSION") )
#elif __sun
    vector->push_back(string_pair("TBB: BUILD_KERNEL", required));
    vector->push_back(string_pair("TBB: BUILD_SUNCC", required));
    vector->push_back(string_pair("TBB: BUILD_COMPILER", not_required)); //if( getenv("COMPILER_VERSION") )
#else //We use version_info_linux.sh for unsupported OSes
    vector->push_back(string_pair("TBB: BUILD_KERNEL", required));
    vector->push_back(string_pair("TBB: BUILD_GCC", required));
    vector->push_back(string_pair("TBB: BUILD_COMPILER", not_required)); //if( getenv("COMPILER_VERSION") )
    vector->push_back(string_pair("TBB: BUILD_GLIBC", required));
    vector->push_back(string_pair("TBB: BUILD_LD", required));
#endif
    vector->push_back(string_pair("TBB: BUILD_TARGET", required));
    vector->push_back(string_pair("TBB: BUILD_COMMAND", required));
    vector->push_back(string_pair("TBB: TBB_USE_DEBUG", required));
    vector->push_back(string_pair("TBB: TBB_USE_ASSERT", required));
    vector->push_back(string_pair("TBB: DO_ITT_NOTIFY", required));
    vector->push_back(string_pair("TBB: ITT", not_required)); //#ifdef DO_ITT_NOTIFY
    vector->push_back(string_pair("TBB: ALLOCATOR", required));
    vector->push_back(string_pair("TBB: RML", not_required));
    vector->push_back(string_pair("TBB: Intel(R) RML library built:", not_required));
    vector->push_back(string_pair("TBB: Intel(R) RML library version:", not_required));
    vector->push_back(string_pair("TBB: SCHEDULER", required));

    return;
}
