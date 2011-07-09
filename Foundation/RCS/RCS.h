#pragma once

#include <string>
#include <vector>

#include "Platform/Types.h"
#include "Platform/Assert.h"

#include "Foundation/API.h"
#include "Foundation/RCS/RCSTypes.h"
#include "Foundation/RCS/RCSExceptions.h"
#include "Foundation/RCS/Provider.h"

namespace Helium
{
    namespace RCS
    {
        namespace SyncTimes
        {
            const uint64_t Now = 0xFFFFFFFFFFFFFFFF;
        }

        HELIUM_FOUNDATION_API Changeset& DefaultChangeset();

        // set the revision control provider, you must do this before using any other functionality
        HELIUM_FOUNDATION_API void SetProvider( Provider* provider );
        HELIUM_FOUNDATION_API Provider* GetProvider();

        // Note: if you are passing in a directory, make sure it ends in a slash.
        HELIUM_FOUNDATION_API void SetManagedPaths( const std::vector< tstring >& paths );
        HELIUM_FOUNDATION_API void SetIgnoredPaths( const std::vector< tstring >& paths );
        HELIUM_FOUNDATION_API bool PathIsManaged( const tstring& path );

        HELIUM_FOUNDATION_API void SetSyncTimestamp( const uint64_t timestamp = 0 );
        HELIUM_FOUNDATION_API uint64_t  GetSyncTimestamp();

        HELIUM_FOUNDATION_API void GetOpenedFiles( V_File& files );

        HELIUM_FOUNDATION_API void GetChangesets( RCS::V_Changeset& changesets );
        HELIUM_FOUNDATION_API bool IsValidChangeset( const RCS::Changeset& changeset );

        HELIUM_FOUNDATION_API void GetInfo( V_File& files, const GetInfoFlag flags = GetInfoFlags::Default );
        HELIUM_FOUNDATION_API void GetInfo( const tstring& folder, V_File& files, bool recursive = false, uint32_t fileData = FileData::All, uint32_t actionData = ActionData::All );
    }
}