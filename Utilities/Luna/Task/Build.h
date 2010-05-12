#pragma once

#include "tuid/TUID.h"
#include "Asset/AssetClass.h"
#include "AssetBuilder/AssetBuilder.h"
#include "AssetBuilder/BuilderOptions.h"

#include "API.h"
#include "Task.h"

namespace Luna
{
  namespace Build
  {
    LUNA_TASK_API void Initialize();
    LUNA_TASK_API void Cleanup();
  }

  struct BuildFinishedArgs : TaskFinishedArgs
  {
    S_tuid m_Assets;
    bool m_View;
    AssetBuilder::BuilderOptionsPtr m_BuilderOptions;

    BuildFinishedArgs()
      : m_View (false)
    {

    }
  };

  LUNA_TASK_API void AddAssetBuiltListener(const AssetBuilder::AssetBuiltSignature::Delegate& listener);
  LUNA_TASK_API void RemoveAssetBuiltListener(const AssetBuilder::AssetBuiltSignature::Delegate& listener);

  LUNA_TASK_API void BuildAsset( const tuid& assetId, wxWindow* parent, AssetBuilder::BuilderOptionsPtr builderOptions = NULL, bool showOptions = false, bool blocking = false, const tuid& viewAsset = TUID::Null );
  LUNA_TASK_API void BuildAssets( const S_tuid& assetIds, wxWindow* parent, AssetBuilder::BuilderOptionsPtr builderOptions = NULL, bool showOptions = false, bool blocking = false, const tuid& viewAsset = TUID::Null );
  LUNA_TASK_API void ViewAsset( const tuid& assetId, wxWindow* parent, AssetBuilder::BuilderOptionsPtr builderOptions, bool showOptions = false, bool blocking = false );
}