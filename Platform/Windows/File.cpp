#include "Platform/File.h"

#include "Platform/Assert.h"
#include "Platform/Windows/Windows.h"

using namespace Helium;

bool Helium::CloseHandle( Handle& handle )
{
    return 1 == ::CloseHandle( (HANDLE)handle );
}

bool Helium::ReadFile( Handle& handle, void* buffer, uint32_t numberOfBytesToRead, uint32_t* numberOfBytesRead )
{
    HELIUM_ASSERT_MSG( numberOfBytesToRead <= MAXDWORD, TXT( "File read operations are limited to DWORD sizes" ) );
    if( numberOfBytesToRead > MAXDWORD )
    {
        return false;
    }

    HELIUM_ASSERT( buffer || numberOfBytesToRead == 0 );

    DWORD bytesRead = 0;
    return 1 == ::ReadFile( handle, buffer, static_cast< DWORD >( numberOfBytesToRead ), reinterpret_cast< LPDWORD >( numberOfBytesRead ), NULL );
}