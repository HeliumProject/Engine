#include "Pipe.h"

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
    NOC_BREAK(); return false;
}

bool Platform::OpenPipe(const tchar* name, Pipe& pipe)
{
    if (cellFsOpen(name, CELL_FS_O_RDWR, &pipe, NULL, 0) != CELL_FS_OK)
    {
        return false;
    }

    return true;
}

void Platform::ClosePipe(Pipe& pipe)
{
    cellFsClose(pipe);
}

bool Platform::ConnectPipe(Pipe& pipe, Event& terminate)
{
    // not supported
    NOC_BREAK(); return false;
}

void Platform::DisconnectPipe(Pipe& pipe)
{
    // not supported
    NOC_BREAK();
}

bool Platform::ReadPipe(Pipe& pipe, void* buffer, u32 bytes, u32& read, Event& terminate)
{
    if (bytes == 0)
    {
        return true;
    }

    uint64_t read_local = 0;
    if (cellFsRead(pipe, buffer, bytes, &read_local) != CELL_FS_OK)
    {
        return false;
    }

    read = (u32)read_local;

    return true;
}

bool Platform::WritePipe(Pipe& pipe, void* buffer, u32 bytes, u32& wrote, Event& terminate)
{
    if (bytes == 0)
    {
        return true;
    }

    uint64_t wrote_local = 0;
    if (cellFsWrite(pipe, buffer, bytes, &wrote_local) != CELL_FS_OK)
    {
        return false;
    }

    wrote = (u32)wrote_local;

    return true;
}
