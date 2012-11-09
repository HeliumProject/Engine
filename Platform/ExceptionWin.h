#pragma once

#include <vector>

#include "Platform/API.h"
#include "Platform/Types.h"

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

namespace Helium
{
	class Exception;

    namespace ExceptionTypes
    {
        enum ExceptionType
        {
            SEH,
            CPP
        };

        static const tchar_t* Strings[] =
        {
            TXT("SEH"),
            TXT("C++"),
        };
    }
    typedef ExceptionTypes::ExceptionType ExceptionType;

    struct HELIUM_PLATFORM_API ExceptionArgs
    {
        ExceptionType           m_Type;
        bool                    m_Fatal;
        tstring                 m_Message;
        tstring                 m_Callstack;
        std::vector< tstring >  m_Threads;
        tstring                 m_State;
        tstring                 m_Dump;

        // SEH-specific info
        uint32_t                     m_SEHCode;
        tstring                 m_SEHClass;
        tstring                 m_SEHControlRegisters;
        tstring                 m_SEHIntegerRegisters;

        // CPP-specific info
        tstring                 m_CPPClass;

        ExceptionArgs( ExceptionType type, bool fatal )
            : m_Type( type )
            , m_Fatal( fatal )
            , m_SEHCode( -1 )
        {
        };
    };

    // Init (need to specify the search paths to the pdbs if they aren't with the executables)
    HELIUM_PLATFORM_API bool InitializeSymbols( const tstring& path = TXT("") );

    // Query information from a bare address (should be pretty safe to call)
    HELIUM_PLATFORM_API tstring GetSymbolInfo( uintptr_t adr, bool enumLoadedModules = true );
    HELIUM_PLATFORM_API Helium::Exception* GetHeliumException( uintptr_t addr );
    HELIUM_PLATFORM_API std::exception* GetStandardException( uintptr_t addr );

    // Stack traces (capture with or without an explicit context, translate to string rep)
    HELIUM_PLATFORM_API bool GetStackTrace( std::vector<uintptr_t>& trace, unsigned omitFrames = 0 );
    HELIUM_PLATFORM_API bool GetStackTrace( LPCONTEXT context, std::vector<uintptr_t>& stack, unsigned omitFrames = 0 );
    HELIUM_PLATFORM_API void TranslateStackTrace( const std::vector<uintptr_t>& trace, tstring& buffer );

    // Query SEH exception pointers for full report, abbreviated name, or more details
    HELIUM_PLATFORM_API tstring GetExceptionInfo( LPEXCEPTION_POINTERS info );
    HELIUM_PLATFORM_API const tchar_t* GetExceptionClass( uint32_t exceptionCode );
    HELIUM_PLATFORM_API void GetExceptionDetails( LPEXCEPTION_POINTERS info, ExceptionArgs& args );

    // Core dumps (full dumps include process heaps)
    HELIUM_PLATFORM_API tstring WriteDump( LPEXCEPTION_POINTERS info, bool full );
}