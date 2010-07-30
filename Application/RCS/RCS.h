#pragma once

#include <string>
#include <vector>

#include "Application/API.h"
#include "RCSTypes.h"
#include "RCSExceptions.h"
#include "Provider.h"
#include "Platform/Types.h"
#include "Platform/Assert.h"

namespace Helium
{
    // RCS API
    namespace RCS
    {
        namespace SyncTimes
        {
            const u64 Now = 0xFFFFFFFFFFFFFFFF;
        }

        APPLICATION_API Changeset& DefaultChangeset();

        // set the revision control provider, you must do this before using any other functionality
        APPLICATION_API void SetProvider( Provider* provider );
        APPLICATION_API Provider* GetProvider();

        // Note: if you are passing in a directory, make sure it ends in a slash.
        APPLICATION_API void SetManagedPaths( const std::vector< tstring >& paths );
        APPLICATION_API void SetIgnoredPaths( const std::vector< tstring >& paths );
        APPLICATION_API bool PathIsManaged( const tstring& path );

        APPLICATION_API void SetSyncTimestamp( const u64 timestamp = 0 );
        APPLICATION_API u64  GetSyncTimestamp();

        APPLICATION_API void GetOpenedFiles( V_File& files );

        APPLICATION_API void GetChangesets( RCS::V_Changeset& changesets );
        APPLICATION_API bool IsValidChangeset( const RCS::Changeset& changeset );

        APPLICATION_API void GetInfo( V_File& files, const GetInfoFlag flags = GetInfoFlags::Default );
        APPLICATION_API void GetInfo( const tstring& folder, V_File& files, bool recursive = false, u32 fileData = FileData::All, u32 actionData = ActionData::All );
    }
}