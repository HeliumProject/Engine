#include "Platform/Windows/Windows.h"
#include "Platform/Windows/Pipe.h"
#include "Platform/Pipe.h"
#include "Platform/Assert.h"
#include "Platform/Error.h"
#include "Platform/Platform.h"

using namespace Helium;

HELIUM_COMPILE_ASSERT( sizeof( Pipe::Overlapped ) == sizeof( OVERLAPPED ) );

Pipe::Pipe(int)
: m_Handle (0)
{
    memset(&m_Overlapped, 0, sizeof(m_Overlapped));
    m_Overlapped.hEvent = ::CreateEvent(0, true, false, 0);
}

Pipe::~Pipe()
{
    ::CloseHandle( m_Overlapped.hEvent );
}

bool Helium::InitializePipes()
{
    return true;
}

void Helium::CleanupPipes()
{

}

bool Helium::CreatePipe(const tchar* name, Pipe& pipe)
{
    //
    // We must retry here because quickly thrashing the pipe API can sometimes cause
    //  All pipe instances are busy... calling WaitNamedPipe for a while works around it
    //  Issue exists in Vista and Server 2008.
    //

    int retry = 0;
    do
    {
        pipe.m_Handle = ::CreateNamedPipe( name,
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1,
            IPC_PIPE_BUFFER_SIZE,
            IPC_PIPE_BUFFER_SIZE,
            1000,
            NULL );

        if (pipe.m_Handle == INVALID_HANDLE_VALUE) 
        {
            if ( retry > 10 )
            {
                Helium::Print(TXT("Pipe Support: Failed to create pipe '%s' (%s)\n"), name, Helium::GetErrorString().c_str());
                return false;
            }
            else
            {
                retry++;

                if ( !::WaitNamedPipe( name, 0 ) )
                {
                    ::Sleep(100);
                }
            }
        }
    }
    while ( pipe.m_Handle == INVALID_HANDLE_VALUE );

    return true;
}

bool Helium::OpenPipe(const tchar* name, Pipe& pipe)
{
    if ( !::WaitNamedPipe(name, 0) ) 
    {
        return false;
    }

    pipe.m_Handle = ::CreateFile( name,
        GENERIC_READ | GENERIC_WRITE, 
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL );

    if (pipe.m_Handle == INVALID_HANDLE_VALUE)
    {
        if (::GetLastError() != ERROR_PIPE_BUSY) 
        {
            Helium::Print(TXT("Pipe Support: Failed to open pipe (%s)\n"), Helium::GetErrorString().c_str());
        }

        return false;
    }
    else
    {
        // with the pipe connected; change to byte mode
        DWORD mode = PIPE_READMODE_BYTE|PIPE_WAIT; 
        if ( !::SetNamedPipeHandleState(pipe.m_Handle, &mode, NULL, NULL) )
        {
            Helium::Print(TXT("Pipe Support: Failed to set client byte mode (%s)\n"), Helium::GetErrorString().c_str());
            ::CloseHandle( pipe.m_Handle );
            return false;
        }
    }

    return true;
}

void Helium::ClosePipe(Pipe& pipe)
{
    ::CloseHandle(pipe.m_Handle);
}

bool Helium::ConnectPipe(Pipe& pipe, Condition& terminate)
{
    OVERLAPPED connect;
    memset(&connect, 0, sizeof(connect));
    connect.hEvent = ::CreateEvent( 0, true, false, 0 );

    if ( !::ConnectNamedPipe(pipe.m_Handle, &connect) )
    {
        DWORD error = ::GetLastError();

        if ( error == ERROR_PIPE_CONNECTED )
        {
            ::CloseHandle( connect.hEvent );
            return true;
        }

        if ( error != ERROR_IO_PENDING )
        {
#ifdef IPC_PIPE_DEBUG_PIPES
            Helium::Print("Pipe Support: Failed to connect pipe (%s)\n", Helium::GetErrorString().c_str());
#endif
            ::CloseHandle( connect.hEvent );
            return false;
        }
        else
        {
            HANDLE events[] = { terminate.GetHandle(), connect.hEvent };
            DWORD result = ::WaitForMultipleObjects(2, events, FALSE, INFINITE);

            HELIUM_ASSERT( result != WAIT_FAILED );
            if ( result )
            {
                if ( (result - WAIT_OBJECT_0) == 0 )
                {
#ifdef IPC_PIPE_DEBUG_PIPES
                    Helium::Print("Pipe Support: Terminating connect\n");
#endif
                    ::CloseHandle( connect.hEvent );
                    ::CancelIo( pipe.m_Handle );
                    return false;
                }
            }

            DWORD bytes;
            if ( !::GetOverlappedResult(pipe.m_Handle, &connect, &bytes, false) )
            {
#ifdef IPC_PIPE_DEBUG_PIPES
                Helium::Print("Pipe Support: Failed to connect pipe (%s)\n", Helium::GetErrorString().c_str());
#endif
                ::CloseHandle( connect.hEvent );
                return false;
            }
        }
    }

    ::CloseHandle( connect.hEvent );
    return true;
}

void Helium::DisconnectPipe(Pipe& pipe)
{
    if (!::FlushFileBuffers(pipe.m_Handle))
    {
        Helium::Print(TXT("Pipe Support: Failed to flush pipe buffers (%s)\n"), Helium::GetErrorString().c_str());
    }

    if (!::DisconnectNamedPipe(pipe.m_Handle))
    {
        Helium::Print(TXT("Pipe Support: Failed to diconnect pipe (%s)\n"), Helium::GetErrorString().c_str());
    }
}

bool Helium::ReadPipe(Pipe& pipe, void* buffer, u32 bytes, u32& read, Condition& terminate)
{
    if (bytes == 0)
    {
        return true;
    }

    DWORD read_local = 0;
    if ( !::ReadFile(pipe.m_Handle, buffer, bytes, &read_local, (OVERLAPPED*)&pipe.m_Overlapped) )
    {
        if ( ::GetLastError() != ERROR_IO_PENDING )
        {
#ifdef IPC_PIPE_DEBUG_PIPES
            Helium::Print("Pipe Support: Failed to initiate overlapped read (%s)\n", Helium::GetErrorString().c_str());
#endif
            return false;
        }
        else
        {
            HANDLE events[] = { terminate.GetHandle(), pipe.m_Overlapped.hEvent };
            DWORD result = ::WaitForMultipleObjects(2, events, FALSE, INFINITE);

            HELIUM_ASSERT( result != WAIT_FAILED );
            if ( result )
            {
                if ( (result - WAIT_OBJECT_0) == 0 )
                {
#ifdef IPC_PIPE_DEBUG_PIPES
                    Helium::Print("Pipe Support: Terminating read\n");
#endif
                    ::CancelIo( pipe.m_Handle );
                    return false;
                }
            }

            if ( !::GetOverlappedResult(pipe.m_Handle, (OVERLAPPED*)&pipe.m_Overlapped, &read_local, false) )
            {
#ifdef IPC_PIPE_DEBUG_PIPES
                Helium::Print("Pipe Support: Failed read (%s)\n", Helium::GetErrorString().c_str());
#endif
                return false;
            }
        }
    }

    read = (u32)read_local;

    return true;
}

bool Helium::WritePipe(Pipe& pipe, void* buffer, u32 bytes, u32& wrote, Condition& terminate)
{
    if (bytes == 0)
    {
        return true;
    }

    DWORD wrote_local = 0;
    if ( !::WriteFile(pipe.m_Handle, buffer, bytes, &wrote_local, (OVERLAPPED*)&pipe.m_Overlapped) )
    {
        if ( ::GetLastError() != ERROR_IO_PENDING )
        {
#ifdef IPC_PIPE_DEBUG_PIPES
            Helium::Print("Pipe Support: Failed to initiate overlapped write (%s)\n", Helium::GetErrorString().c_str());
#endif
            return false;
        }
        else
        {
            HANDLE events[] = { terminate.GetHandle(), pipe.m_Overlapped.hEvent };
            DWORD result = ::WaitForMultipleObjects(2, events, FALSE, INFINITE);

            HELIUM_ASSERT( result != WAIT_FAILED );
            if ( result )
            {
                if ( (result - WAIT_OBJECT_0) == 0 )
                {
#ifdef IPC_PIPE_DEBUG_PIPES
                    Helium::Print("Pipe Support: Terminating write\n");
#endif
                    ::CancelIo( pipe.m_Handle );
                    return false;
                }
            }

            if ( !::GetOverlappedResult(pipe.m_Handle, (OVERLAPPED*)&pipe.m_Overlapped, &wrote_local, false) )
            {
#ifdef IPC_PIPE_DEBUG_PIPES
                Helium::Print("Pipe Support: Failed write (%s)\n", Helium::GetErrorString().c_str());
#endif
                return false;
            }
        }
    }

    wrote = (u32)wrote_local;

    return true;
}