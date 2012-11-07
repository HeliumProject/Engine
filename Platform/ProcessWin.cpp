#include "PlatformPch.h"
#include "Platform/Process.h"

#include "Platform/Assert.h"
#include "Platform/Error.h"
#include "Platform/Encoding.h"
#include "Platform/Utility.h"

#include <sstream>

int Helium::Execute( const tstring& command, bool showWindow, bool block )
{
    DWORD result = 0;

    STARTUPINFO si;
    memset( &si, 0, sizeof(si) );

    PROCESS_INFORMATION pi;
    memset( &pi, 0, sizeof( pi ) );

    // Start the child process.
    if( !CreateProcess(
        NULL,                                                 // No module name (use command line)
        (LPTSTR) command.c_str(),                            // Command line
        NULL,                                                 // Process handle not inheritable
        NULL,                                                 // Thread handle not inheritable
        FALSE,                                                // Set handle inheritance to FALSE
        showWindow ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW,  // Creation flags
        NULL,                                                 // Use parent's environment block
        NULL,                                                 // Use parent's starting directory
        &si,                                                  // Pointer to STARTUPINFO structure
        &pi ) )                                               // Pointer to PROCESS_INFORMATION structure
    {
        return -1;
    }

    if (block)
    {
        DWORD waitResult = ::WaitForSingleObject( pi.hProcess, INFINITE );
        HELIUM_ASSERT( waitResult != WAIT_FAILED );

        BOOL codeResult = ::GetExitCodeProcess( pi.hProcess, &result );
        HELIUM_ASSERT( codeResult );
    }

    ::CloseHandle( pi.hProcess );
    ::CloseHandle( pi.hThread );

    return result;
}

int Helium::Execute( const tstring& command, tstring& output, bool showWindow )
{
    HANDLE hReadPipe;
    HANDLE hWritePipe;
    SECURITY_ATTRIBUTES sa;
    sa.nLength              = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle       = TRUE;
    if( !CreatePipe( &hReadPipe, &hWritePipe, &sa, 0 ) )
    {
        return -1;
    }

    STARTUPINFO          si;
    memset( &si, 0, sizeof(si) );
    si.cb          = sizeof(si);
    si.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = static_cast<int>( showWindow );
    si.hStdOutput  = hWritePipe;
    si.hStdError   = hWritePipe;      

    PROCESS_INFORMATION  pi;
    memset( &pi, 0, sizeof( pi ) );

	HELIUM_CONVERT_TO_NATIVE( command.c_str(), convertedCommand );

	if( !::CreateProcess(
        NULL,                                                 // filename
        (LPWSTR)convertedCommand,                             // command line for child
        NULL,                                                 // process security descriptor
        NULL,                                                 // thread security descriptor
        TRUE,                                                 // inherit handles?
        showWindow ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW,   // creation flags
        NULL,                                                 // inherited environment address
        NULL,                                                 // startup dir; NULL = start in current
        &si,                                                  // pointer to startup info (input)
        &pi ) )                                               // pointer to process info (output)
    {
        ::CloseHandle( hReadPipe );
        ::CloseHandle( hWritePipe );
        return -1;
    }

    // close the write end of the pipe so the child will terminate it with EOF
    ::CloseHandle( hWritePipe );

    // read from the pipe until EOF condition reached
    tchar_t buffer[80];
    unsigned long count;
    tstringstream stream;
    BOOL success = TRUE;
    do
    {
        while ( success = ReadFile( hReadPipe, buffer, sizeof(buffer), &count, NULL ) )
        {
            if( success )
            {
                stream.write( buffer, count );
            }
            else
            {
                if ( ::GetLastError() == ERROR_BROKEN_PIPE )
                {
                    break;
                }
                else
                {
                    return -1;
                }
            }
        }
    } while( success && count );

    // done reading, close our read pipe
    ::CloseHandle( hReadPipe );

    // copy output string
    output = stream.str();

    // get exit code
    DWORD result = 0;
    BOOL codeResult = ::GetExitCodeProcess( pi.hProcess, &result );
    HELIUM_ASSERT( codeResult );

    // close the process handle
    ::CloseHandle( pi.hProcess );
    ::CloseHandle( pi.hThread );

    return result;
}

tstring Helium::GetProcessString()
{
    tostringstream result;
    result << GetProcessName() << "_" << GetCurrentProcessId() << "_" << GetCurrentThreadId();
    return result.str();
}

tstring Helium::GetProcessPath()
{
	HMODULE moduleHandle = ::GetModuleHandle( NULL );

    wchar_t module[ MAX_PATH ];
	DWORD result = ::GetModuleFileName( moduleHandle, module, MAX_PATH );
    HELIUM_ASSERT( result );
    HELIUM_UNREF( result );

	HELIUM_CONVERT_TO_TCHAR( module, convertedModule );
    return convertedModule;
}

tstring Helium::GetProcessName()
{
    HMODULE moduleHandle = GetModuleHandle( NULL );

    wchar_t module[ MAX_PATH ];
	::GetModuleFileName( moduleHandle, module, MAX_PATH );

    wchar_t file[ MAX_PATH ];
    _wsplitpath( module, NULL, NULL, file, NULL );

	HELIUM_CONVERT_TO_TCHAR( file, convertedFile );
    return convertedFile;
}

tstring Helium::GetDumpDirectory()
{
    wchar_t tempDir[ MAX_PATH ];
	size_t result = ::GetTempPath( MAX_PATH, tempDir );
    if ( result == 0 || result > MAX_PATH )
    {
        return TXT("");
    }

    // Make sure that the directory exists
    wchar_t directory[ MAX_PATH ] = { 0 };
    _snwprintf( directory, sizeof( directory ) - 1, L"%s\\dumps", &tempDir );

	HELIUM_CONVERT_TO_TCHAR( directory, convertedDirectory );
    return convertedDirectory;
}