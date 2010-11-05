//----------------------------------------------------------------------------------------------------------------------
// DebugWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"

#if L_OS_WIN

#include "Core/Debug.h"

#if !L_RELEASE

#if L_UNICODE
#define DBGHELP_TRANSLATE_TCHAR
#endif

#include <dbghelp.h>

namespace Lunar
{
    static LwMutex& GetStackWalkMutex()
    {
        static LwMutex stackWalkMutex;

        return stackWalkMutex;
    }

    static void ConditionalSymInitialize()
    {
        static volatile bool bSymInitialized = false;
        if( !bSymInitialized )
        {
            L_LOG( LOG_INFO, L_T( "Initializing symbol handler for the current process...\n" ) );

            HANDLE hProcess = GetCurrentProcess();
            L_ASSERT( hProcess );

            BOOL bInitialized = SymInitialize( hProcess, NULL, TRUE );
            if( bInitialized )
            {
                L_LOG( LOG_INFO, L_T( "Symbol handler initialization successful!\n" ) );
            }
            else
            {
                L_LOG( LOG_ERROR, L_T( "Symbol handler initialization failed (error code %u).\n" ), GetLastError() );
            }

            bSymInitialized = true;
        }
    }

    /// Get the current stack trace.
    ///
    /// @param[out] ppStackTraceArray    Array in which to store the backtrace of program counter addresses.
    /// @param[in]  stackTraceArraySize  Maximum number of addresses to fill in the output array.
    /// @param[in]  skipCount            Number of stack levels to skip before filling the output array, counting the
    ///                                  stack level for this function call.  By default, this is one, meaning that only
    ///                                  the call to this function is skipped.
    ///
    /// @return  Number of addresses stored in the output array.
    size_t GetStackTrace( void** ppStackTraceArray, size_t stackTraceArraySize, size_t skipCount )
    {
        L_ASSERT( ppStackTraceArray || stackTraceArraySize == 0 );

        ScopeLock< LwMutex > scopeLock( GetStackWalkMutex() );
        ConditionalSymInitialize();

        // Get the current context.
        CONTEXT context;
        RtlCaptureContext( &context );

        // Initialize the stack frame structure for the first call to StackWalk64().
        STACKFRAME64 stackFrame;
        MemoryZero( &stackFrame, sizeof( stackFrame ) );
        stackFrame.AddrPC.Mode = AddrModeFlat;
        stackFrame.AddrFrame.Mode = AddrModeFlat;
        stackFrame.AddrStack.Mode = AddrModeFlat;

#if L_OS_WIN32
        const DWORD machineType = IMAGE_FILE_MACHINE_I386;
        stackFrame.AddrPC.Offset = context.Eip;
        stackFrame.AddrFrame.Offset = context.Ebp;
        stackFrame.AddrStack.Offset = context.Esp;
#else
        // Assuming x86-64 (likely not supporting Itanium).
        const DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
        stackFrame.AddrPC.Offset = context.Rip;
        stackFrame.AddrFrame.Offset = context.Rdi/*Rbp*/;
        stackFrame.AddrStack.Offset = context.Rsp;
#endif

        HANDLE hProcess = GetCurrentProcess();
        L_ASSERT( hProcess );

        HANDLE hThread = GetCurrentThread();
        L_ASSERT( hThread );

        // Skip addresses first.
        for( size_t skipIndex = 0; skipIndex < skipCount; ++skipIndex )
        {
            BOOL bResult = StackWalk64(
                machineType,
                hProcess,
                hThread,
                &stackFrame,
                &context,
                NULL,
                SymFunctionTableAccess64,
                SymGetModuleBase64,
                NULL );
            if( !bResult || stackFrame.AddrPC.Offset == 0 )
            {
                return 0;
            }
        }

        // Fill out the remaining stack frame addresses up to the output array limit.
        for( size_t traceIndex = 0; traceIndex < stackTraceArraySize; ++traceIndex )
        {
            BOOL bResult = StackWalk64(
                machineType,
                hProcess,
                hThread,
                &stackFrame,
                &context,
                NULL,
                SymFunctionTableAccess64,
                SymGetModuleBase64,
                NULL );
            if( !bResult || stackFrame.AddrPC.Offset == 0 )
            {
                return traceIndex;
            }

            ppStackTraceArray[ traceIndex ] =
                reinterpret_cast< void* >( static_cast< uintptr_t >( stackFrame.AddrPC.Offset ) );
        }

        return stackTraceArraySize;
    }

    /// Get the symbol for the specified address.
    ///
    /// @param[out] rSymbol   Address symbol.
    /// @param[in]  pAddress  Address to translate.
    ///
    /// @return  True if the address was successfully resolved, false if not.
    void GetAddressSymbol( String& rSymbol, void* pAddress )
    {
        L_ASSERT( pAddress );

        ScopeLock< LwMutex > scopeLock( GetStackWalkMutex() );
        ConditionalSymInitialize();

        rSymbol.Remove( 0, rSymbol.GetSize() );

        HANDLE hProcess = GetCurrentProcess();
        L_ASSERT( hProcess );

        bool bAddedModuleName = false;

        DWORD64 moduleBase = SymGetModuleBase64( hProcess, reinterpret_cast< uintptr_t >( pAddress ) );
        if( moduleBase )
        {
            IMAGEHLP_MODULE64 moduleInfo;
            MemoryZero( &moduleInfo, sizeof( moduleInfo ) );
            moduleInfo.SizeOfStruct = sizeof( moduleInfo );
            if( SymGetModuleInfo64( hProcess, moduleBase, &moduleInfo ) )
            {
                rSymbol += L_T( "(" );
                rSymbol += moduleInfo.ModuleName;
                rSymbol += L_T( ") " );

                bAddedModuleName = true;
            }
        }

        if( !bAddedModuleName )
        {
            rSymbol += L_T( "(???) " );
        }

        uint64_t symbolInfoBuffer[
            ( sizeof( SYMBOL_INFO ) + sizeof( tchar_t ) * ( MAX_SYM_NAME - 1 ) + sizeof( uint64_t ) - 1 ) /
            sizeof( uint64_t ) ];
        MemoryZero( symbolInfoBuffer, sizeof( symbolInfoBuffer ) );

        SYMBOL_INFO& rSymbolInfo = *reinterpret_cast< SYMBOL_INFO* >( &symbolInfoBuffer[ 0 ] );
        rSymbolInfo.SizeOfStruct = sizeof( SYMBOL_INFO );
        rSymbolInfo.MaxNameLen = MAX_SYM_NAME;
        if( SymFromAddr( hProcess, reinterpret_cast< uintptr_t >( pAddress ), NULL, &rSymbolInfo ) )
        {
            rSymbolInfo.Name[ MAX_SYM_NAME - 1 ] = L_T( '\0' );
            rSymbol += rSymbolInfo.Name;
            rSymbol += L_T( " " );
        }
        else
        {
            rSymbol += L_T( "??? " );
        }

        DWORD displacement = 0;
        IMAGEHLP_LINE64 lineInfo;
        MemoryZero( &lineInfo, sizeof( lineInfo ) );
        lineInfo.SizeOfStruct = sizeof( lineInfo );
        if( SymGetLineFromAddr64( hProcess, reinterpret_cast< uintptr_t >( pAddress ), &displacement, &lineInfo ) )
        {
            tchar_t lineNumberBuffer[ 32 ];
            StringFormat( lineNumberBuffer, L_ARRAY_COUNT( lineNumberBuffer ), L_T( "%u" ), lineInfo.LineNumber );
            lineNumberBuffer[ L_ARRAY_COUNT( lineNumberBuffer ) - 1 ] = L_T( '\0' );

            rSymbol += L_T( "(" );
            rSymbol += lineInfo.FileName;
            rSymbol += L_T( ", line " );
            rSymbol += lineNumberBuffer;
            rSymbol += L_T( ")" );
        }
        else
        {
            rSymbol += L_T( "(???, line ?)" );
        }
    }
}

#endif  // !L_RELEASE

#endif  // L_OS_WIN
