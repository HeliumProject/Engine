#include "Platform/Directory.h"

#include "Platform/Error.h"
#include "Platform/Exception.h"
#include "Platform/Windows/Windows.h"

using namespace Helium;

DirectoryHandle Helium::FindFirst( const tstring& spec, FileFindData& data )
{
    HELIUM_BREAK();
    return NULL;
}

bool Helium::FindNext( DirectoryHandle& handle, FileFindData& data )
{
    HELIUM_BREAK();
    return false; 
}

bool Helium::CloseFind( DirectoryHandle& handle )
{
    HELIUM_BREAK();
    return false;
}
