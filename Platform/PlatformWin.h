#pragma once

// Temporary workaround for bug in Visual C++ 2008 with including intrin.h and math.h simultaneously
// (see http://connect.microsoft.com/VisualStudio/feedback/details/381422/warning-of-attributes-not-present-on-previous-declaration-on-ceil-using-both-math-h-and-intrin-h).
#pragma warning( push )
#pragma warning( disable : 4985 )  // 'symbol name': attributes not present on previous declaration

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

// windows 7
#define WINVER			0x0601
#define _WIN32_WINNT	0x0601

// internet explorer 7
#define _WIN32_IE		0x0700

#include <tchar.h>
#include <windows.h>
#include <winsock2.h>
#include <intrin.h>

#pragma warning( pop )
