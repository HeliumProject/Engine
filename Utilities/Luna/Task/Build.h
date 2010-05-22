#pragma once

#include "Asset/AssetClass.h"
#include "AssetEditor/AssetClass.h"
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
        File::S_Reference m_Assets;
        AssetBuilder::BuilderOptionsPtr m_BuilderOptions;

        BuildFinishedArgs()
        {

        }
    };

    LUNA_TASK_API void AddAssetBuiltListener(const AssetBuilder::AssetBuiltSignature::Delegate& listener);
    LUNA_TASK_API void RemoveAssetBuiltListener(const AssetBuilder::AssetBuiltSignature::Delegate& listener);

    LUNA_TASK_API void BuildAsset( const File::Reference& asset, wxWindow* parent, AssetBuilder::BuilderOptionsPtr builderOptions = NULL, bool showOptions = false, bool blocking = false );
    LUNA_TASK_API void BuildAssets( const File::S_Reference& assets, wxWindow* parent, AssetBuilder::BuilderOptionsPtr builderOptions = NULL, bool showOptions = false, bool blocking = false );
}