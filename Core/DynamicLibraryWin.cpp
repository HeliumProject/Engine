//----------------------------------------------------------------------------------------------------------------------
// DynamicLibraryWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/DynamicLibrary.h"

#include "Platform/Windows/Windows.h"

namespace Lunar
{
#if HELIUM_ENABLE_TRACE
    /// Get the path to the DLL referenced by the specified handle.
    ///
    /// @param[in] pHandle  DLL handle.
    ///
    /// @return  DLL path name.
    static String GetDllPath( void* pHandle )
    {
        tchar_t pathBuffer[ MAX_PATH ];

        DWORD bufferSize = GetModuleFileName(
            static_cast< HMODULE >( pHandle ),
            pathBuffer,
            L_ARRAY_COUNT( pathBuffer ) );
        if( bufferSize == 0 )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "DynamicLibrary: Failed to retrieve DLL file path.\n" ) );
            pathBuffer[ 0 ] = TXT( '\0' );
        }
        else
        {
            pathBuffer[ L_ARRAY_COUNT( pathBuffer ) - 1 ] = TXT( '\0' );
        }

        String result( pathBuffer );

        return result;
    }
#endif

    /// Constructor.
    ///
    /// @param[in] pHandle  Library handle.
    DynamicLibrary::DynamicLibrary( void* pHandle )
        : m_pHandle( pHandle )
    {
        HELIUM_ASSERT( pHandle );
    }

    /// Destructor.
    DynamicLibrary::~DynamicLibrary()
    {
        HELIUM_TRACE( TRACE_DEBUG, TXT( "DynamicLibrary: Unloading \"%s\".\n" ), *GetDllPath( m_pHandle ) );

        FreeLibrary( static_cast< HMODULE >( m_pHandle ) );
    }

    /// Retrieve the address of the function with the specified name in this library.
    ///
    /// @param[in] pFunctionName  Name of the function to locate.
    ///
    /// @return  Pointer to the function address if the function was found, null if the function was not found.
    void* DynamicLibrary::GetFunctionAddress( const char* pFunctionName ) const
    {
        HELIUM_ASSERT( pFunctionName );

#if HELIUM_ENABLE_TRACE
#if HELIUM_UNICODE
        tchar_t functionNameT[ 1024 ];
        mbstowcs_s( NULL, functionNameT, pFunctionName, _TRUNCATE );
#else
        const tchar_t* functionNameT = pFunctionName;
#endif

        HELIUM_TRACE(
            TRACE_DEBUG,
            TXT( "DynamicLibrary: Locating function \"%s\" in library \"%s\".\n" ),
            functionNameT,
            *GetDllPath( m_pHandle ) );
#endif

        FARPROC pFunction = GetProcAddress( static_cast< HMODULE >( m_pHandle ), pFunctionName );

        return pFunction;
    }

    /// Load a library.
    ///
    /// @param[in] pPath  Path of the library file to load.
    ///
    /// @return  Interface to the library if loaded successfully, null if loading failed.  The library is unloaded by
    ///          destroying the returned interface.
    DynamicLibrary* DynamicLibrary::Load( const tchar_t* pPath )
    {
        HELIUM_ASSERT( pPath );

        HELIUM_TRACE( TRACE_DEBUG, TXT( "DynamicLibrary: Loading \"%s\".\n" ), pPath );

        HMODULE hModule = LoadLibrary( pPath );
        if( !hModule )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "DynamicLibrary: Failed to load \"%s\".\n" ), pPath );

            return NULL;
        }

        DynamicLibrary* pLibrary = new DynamicLibrary( hModule );
        HELIUM_ASSERT( pLibrary );

        HELIUM_TRACE( TRACE_DEBUG, TXT( "DynamicLibrary: Loaded \"%s\" successfully.\n" ), pPath );

        return pLibrary;
    }
}
