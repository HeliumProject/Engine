#pragma once

#include "API.h"

#include "AssetClass.h"
#include "AssetFlags.h"
#include "AssetFile.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"
#include "File/Manager.h"
#include "TUID/TUID.h"

namespace Asset 
{
  namespace Manager
  {
    void ASSET_API Initialize();
    void ASSET_API Cleanup();

    void ASSET_API GetRelatedFiles( AssetFile* assetFile, S_string& relatedFiles );
  } // namespace Manager
} // namespace Asset
