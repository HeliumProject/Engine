#include "Platform/Pipe.h"

#include "Platform/Assert.h"

using namespace Platform;

bool Platform::InitializePipes()
{
    return true;
}

void Platform::CleanupPipes()
{

}

bool Platform::CreatePipe(const tchar* name, Pipe& pipe)
{
    // not supported
    HELIUM_BREAK(); return false;
}

bool Platform::OpenPipe(const tchar* name, Pipe& pipe)
{
    if ( ( pipe = (int)fopen( name, TXT( "rw" ) ) ) == 0 )
    {
        return false;
    }

    return true;
}

void Platform::ClosePipe(Pipe& pipe)
{
    fclose( (FILE*)pipe );
}

bool Platform::ConnectPipe(Pipe& pipe, Event& terminate)
{
    // not supported
    HELIUM_BREAK(); return false;
}

void Platform::DisconnectPipe(Pipe& pipe)
{
    // not supported
    HELIUM_BREAK();
}

bool Platform::ReadPipe(Pipe& pipe, void* buffer, u32 bytes, u32& read, Event& terminate)
{
    if (bytes == 0)
    {
        return true;
    }

    uint64_t read_local = 0;
    if ( ( read_local = fread( buffer, bytes, 1, (FILE*)pipe ) ) )
    {
        read = (u32)read_local;
        return true;
    }

    return false;
}

bool Platform::WritePipe(Pipe& pipe, void* buffer, u32 bytes, u32& wrote, Event& terminate)
{
    if (bytes == 0)
    {
        return true;
    }

    uint64_t wrote_local = 0;
    if ( ( wrote_local = fwrite( buffer, 1, bytes, (FILE*)pipe ) ) )
    {
        wrote = (u32) wrote_local;
        return true;
    }

    return false;
}
