#pragma once

#include <vector>

#include "API.h"

#if defined(_M_IX86)
# define IMAGE_FILE_ARCH IMAGE_FILE_MACHINE_I386
# define AXREG Eax
# define BXREG Ebx
# define CXREG Ecx
# define DXREG Edx
# define SIREG Esi
# define DIREG Edi
# define BPREG Ebp
# define IPREG Eip
# define SPREG Esp
#elif defined(_M_X64)
# define IMAGE_FILE_ARCH IMAGE_FILE_MACHINE_AMD64
# define AXREG Rax
# define BXREG Rbx
# define CXREG Rcx
# define DXREG Rdx
# define SIREG Rsi
# define DIREG Rdi
# define BPREG Rbp
# define IPREG Rip
# define SPREG Rsp
#else
# error Machine type not supported
#endif // _M_IX86

struct _CONTEXT;
typedef _CONTEXT CONTEXT;
typedef CONTEXT* PCONTEXT;
typedef PCONTEXT LPCONTEXT;

struct _EXCEPTION_POINTERS;
typedef _EXCEPTION_POINTERS EXCEPTION_POINTERS;
typedef EXCEPTION_POINTERS* PEXCEPTION_POINTERS;
typedef PEXCEPTION_POINTERS LPEXCEPTION_POINTERS;

typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef unsigned __int64 DWORD64;

namespace Debug
{
  struct ExceptionArgs;
  typedef std::vector<DWORD64> V_DWORD64;

  // Init (need to specify the search paths to the pdbs if they aren't with the executables)
  DEBUG_API bool Initialize( const std::string& pdbPaths = "" );
  DEBUG_API bool IsInitialized();

  // Query information from a bare address (should be pretty safe to call)
  DEBUG_API std::string GetSymbolInfo( DWORD64 adr, bool enumLoadedModules = true );
  DEBUG_API std::exception* GetCxxException( DWORD64 addr );
 
  // Stack traces (capture with or without an explicit context, translate to string rep)
  DEBUG_API bool GetStackTrace( V_DWORD64& trace, unsigned omitFrames = 0 );
  DEBUG_API bool GetStackTrace( LPCONTEXT context, V_DWORD64& stack, unsigned omitFrames = 0 );
  DEBUG_API void TranslateStackTrace( const V_DWORD64& trace, std::string& buffer );

  // Query SEH exception pointers for full report, abbreviated name, or more details
  DEBUG_API std::string GetExceptionInfo( LPEXCEPTION_POINTERS info );
  DEBUG_API const char* GetExceptionClass( DWORD exceptionCode );
  DEBUG_API void GetExceptionDetails( LPEXCEPTION_POINTERS info, ExceptionArgs& args );

  // Core dumps (full dumps include process heaps)
  DEBUG_API std::string WriteDump( LPEXCEPTION_POINTERS info, bool full );
}
