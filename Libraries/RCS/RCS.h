#pragma once

#include <string>
#include <vector>

#include "API.h"
#include "Types.h"
#include "Exceptions.h"
#include "Provider.h"
#include "Common/Types.h"
#include "Common/Assert.h"

// RCS API
namespace RCS
{
  namespace SyncTimes
  {
    const u64 Now = 0xFFFFFFFFFFFFFFFF;
  }

  RCS_API Changeset& DefaultChangeset();

  // set the revision control provider, you must do this before using any other functionality
  RCS_API void SetProvider( Provider* provider );
  RCS_API Provider* GetProvider();

  // Note: if you are passing in a directory, make sure it ends in a slash.
  RCS_API void SetManagedPaths( const std::vector< std::string >& paths );
  RCS_API void SetIgnoredPaths( const std::vector< std::string >& paths );
  RCS_API bool PathIsManaged( const std::string& path );

  RCS_API void SetSyncTimestamp( const u64 timestamp = 0 );
  RCS_API u64  GetSyncTimestamp();

  RCS_API void GetOpenedFiles( V_File& files );

  RCS_API void GetChangesets( RCS::V_Changeset& changesets );
  RCS_API bool IsValidChangeset( const RCS::Changeset& changeset );

  RCS_API void GetInfo( V_File& files, const GetInfoFlag flags = GetInfoFlags::Default );
  RCS_API void GetInfo( const std::string& folder, V_File& files, bool recursive = false, u32 fileData = FileData::All, u32 actionData = ActionData::All );
}
