#pragma once

#include "Platform/Types.h"

#include "Foundation/String.h"
#include "Foundation/FilePath.h"

#include "Engine/Engine.h"

namespace Helium
{
    class FileStream;

    /// Low-level file utilities
    class HELIUM_ENGINE_API FileLocations
    {
    public:
        /// @name Static Initialization
        //@{
        static void Shutdown();
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
