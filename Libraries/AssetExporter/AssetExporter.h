#pragma once

#include "TUID/TUID.h"
#include "API.h"

#include "Reflect/Element.h"
#include "Reflect/Serializers.h"

namespace AssetExporter
{
  ASSETEXPORTER_API void Initialize();
  ASSETEXPORTER_API void Cleanup();

  ASSETEXPORTER_API void FindMayaFiles( const S_tuid& fileIdSet, bool recurse, S_string& mayaFiles );

  ASSETEXPORTER_API void Export( const S_tuid& fileIdSet, bool recurse = false, const std::string& stateTrackerFile = "", bool setupForLighting = false );
  ASSETEXPORTER_API void Export( const tuid fileId, bool recurse = false, const std::string& stateTrackerFile = "", bool setupForLighting = false );
};