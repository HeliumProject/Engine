#pragma once

//
// Insomniac standard include for windows.h
//  * Try at all costs not to include this in a header
//  * Including this in a header for a frequently used library is BAD
//  * Typically you should include this in your precompiled header file, or at the top of your code file
//

#ifdef _WINDOWS_
#error Windows already included
#endif

#ifdef WINVER
#error WINVER defined
#endif

#ifdef _WIN32_IE
#error _WIN32_IE defined
#endif

#ifdef _WIN32_WINNT
#error _WIN32_WINNT defined
#endif

#ifdef WIN32_LEAN_AND_MEAN
#error WIN32_LEAN_AND_MEAN defined
#endif

// prevent min/max from being #defined
#define NOMINMAX

// prevent winsock1 from being included
#define _WINSOCKAPI_

// prevent infrequently used stuff
#define WIN32_LEAN_AND_MEAN

// Declare macros to ensure Windows XP compatibility level before including windows.h.
#define WINVER _WIN32_WINNT_WINXP
#define _WIN32_WINNT _WIN32_WINNT_WINXP

// internet explorer 6
#define _WIN32_IE 0x0600

#include <tchar.h>
#include <windows.h>
#include <winsock2.h>

// poison incompatible APIs
#define GetThreadId GetThreadId_doesnt_work_with_Windows_XP_32_bit_and_is_not_protected_by_a_preprocessor_guard