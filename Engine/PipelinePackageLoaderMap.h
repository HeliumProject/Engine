#pragma once

#include "PcSupport/PcSupport.h"

#include "Foundation/ConcurrentHashMap.h"

#include "Engine/AssetPath.h"

namespace Helium
{
    class PipelinePackageLoader;

    /// Archive package loader management for object loaders.
    class HELIUM_ENGINE_API PipelinePackageLoaderMap
    {
    public:
        /// @name Construction/Destruction
        //@{
        PipelinePackageLoaderMap();
        ~PipelinePackageLoaderMap();
        //@}

        /// @name Package Loader Access
        //@{
        PipelinePackageLoader* GetPackageLoader( AssetPath path );
        void TickPackageLoaders();
        //@}

    private:
        /// Package loader hash map (package path used as loader key).
        ConcurrentHashMap< AssetPath, PipelinePackageLoader* > m_packageLoaderMap;

    };
}
