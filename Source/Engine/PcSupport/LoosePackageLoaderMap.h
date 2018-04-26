#pragma once

#include "PcSupport/PcSupport.h"

#include "Foundation/ConcurrentHashMap.h"

#include "Engine/AssetPath.h"

namespace Helium
{
    class LoosePackageLoader;

    /// Archive package loader management for object loaders.
    class HELIUM_PC_SUPPORT_API LoosePackageLoaderMap
    {
    public:
        /// @name Construction/Destruction
        //@{
        LoosePackageLoaderMap();
        ~LoosePackageLoaderMap();
        //@}

        /// @name Package Loader Access
        //@{
        LoosePackageLoader* GetPackageLoader( AssetPath path );
        void TickPackageLoaders();
        //@}

    private:
        /// Package loader hash map (package path used as loader key).
        ConcurrentHashMap< AssetPath, LoosePackageLoader* > m_packageLoaderMap;

    };
}
