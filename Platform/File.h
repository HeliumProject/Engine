#pragma once

#include "Platform/API.h"

#include "Platform/Types.h"

#ifdef WIN32
# include "Platform/Windows/File.h"
#else
# include "Platform/POSIX/File.h"
#endif

namespace Helium
{
    class SeekOrigins
    {
    public:
        /// Stream seek origin.
        enum SeekOrigin
        {
            SEEK_ORIGIN_FIRST   =  0,
            SEEK_ORIGIN_INVALID = -1,

            SEEK_ORIGIN_CURRENT,  ///< Seek relative to the current location.
            SEEK_ORIGIN_BEGIN,    ///< Seek relative to the beginning of the stream.
            SEEK_ORIGIN_END,      ///< Seek relative to the end of the stream.

            SEEK_ORIGIN_MAX,
            SEEK_ORIGIN_LAST = SEEK_ORIGIN_MAX - 1
        };
    };
    typedef SeekOrigins::SeekOrigin SeekOrigin;

    class FileModes
    {
    public:
        /// File access mode flags.
        enum FileMode
        {
            MODE_READ  = ( 1 << 0 ),  ///< Read access.
            MODE_WRITE = ( 1 << 1 ),  ///< Write access.
        };
    };
    typedef FileModes::FileMode FileMode;

    Handle PLATFORM_API CreateFile( const tchar_t* filename, FileMode mode, bool truncate = true );
    bool PLATFORM_API CloseHandle( Handle& handle );
    bool PLATFORM_API ReadFile( Handle& handle, void* buffer, size_t numberOfBytesToRead, size_t* numberOfBytesRead = NULL );
    bool PLATFORM_API WriteFile( Handle& handle, const void* buffer, size_t numberOfBytesToWrite, size_t* numberOfBytesWritten = NULL );
    bool PLATFORM_API FlushFile( Handle& handle );
    int64_t PLATFORM_API Seek( Handle& handle, int64_t offset, SeekOrigin origin );
    int64_t PLATFORM_API Tell( const Handle& handle );
    int64_t PLATFORM_API GetSize( const Handle& handle );
}