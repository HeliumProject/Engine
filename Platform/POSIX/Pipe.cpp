#include "Platform/Pipe.h"

#include "Platform/Assert.h"

using namespace Helium;

bool Helium::InitializePipes()
{
    return true;
}

void Helium::CleanupPipes()
{

}

bool Helium::CreatePipe(const tchar* name, Pipe& pipe)
{
    // not supported
    HELIUM_BREAK(); return false;
}

bool Helium::OpenPipe(const tchar* name, Pipe& pipe)
{
    if ( ( pipe = (int)fopen( name, TXT( "rw" ) ) ) == 0 )
    {
        return false;
    }

    return true;
}

void Helium::ClosePipe(Pipe& pipe)
{
    fclose( (FILE*)pipe );
}

bool Helium::ConnectPipe(Pipe& pipe, Event& terminate)
{
    // not supported
    HELIUM_BREAK(); return false;
}

void Helium::DisconnectPipe(Pipe& pipe)
{
    // not supported
    HELIUM_BREAK();
}

bool Helium::ReadPipe(Pipe& pipe, void* buffer, u32 bytes, u32& read, Event& terminate)
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

bool Helium::WritePipe(Pipe& pipe, void* buffer, u32 bytes, u32& wrote, Event& terminate)
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
