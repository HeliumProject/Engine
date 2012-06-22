#pragma once

#include "Foundation/API.h"
#include "Foundation/String.h"
#include "Foundation/File/Path.h"

#include "Platform/Types.h"

namespace Helium
{
    class FileStream;

    /// Low-level file utilities
    class HELIUM_FOUNDATION_API File
    {
    public:

        /// Directory creation results.
        enum EDirectoryCreateResult
        {
            DIRECTORY_CREATE_RESULT_FIRST   =  0,
            DIRECTORY_CREATE_RESULT_INVALID = -1,

            /// Directory creation succeeded.
            DIRECTORY_CREATE_RESULT_SUCCESS,
            /// Directory creation failed.
            DIRECTORY_CREATE_RESULT_FAILED,
            /// Directory already exists.
            DIRECTORY_CREATE_RESULT_ALREADY_EXISTS,

            DIRECTORY_CREATE_RESULT_MAX,
            DIRECTORY_CREATE_RESULT_LAST = DIRECTORY_CREATE_RESULT_MAX - 1
        };

        /// @name Static Initialization
        //@{
        static void Shutdown();
        //@}

        /// @name File Access
        //@{
        static FileStream* CreateStream();

        static FileStream* Open( const tchar_t* pPath, uint32_t modeFlags, bool bTruncate = true );
        static FileStream* Open( const String& rPath, uint32_t modeFlags, bool bTruncate = true );

        //@}

        /// @name Filesystem Information
        //@{
        static const bool GetBaseDirectory( Path& path );
        static const bool GetDataDirectory( Path& path );
        static const bool GetUserDataDirectory( Path& path );
        //@}

    private:
        /// @name Platform-specific Initialization
        //@{
        static void PlatformShutdown();
        //@}
    };
}
