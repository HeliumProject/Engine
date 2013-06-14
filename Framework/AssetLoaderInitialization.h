#pragma once

#include "Framework/Framework.h"

namespace Helium
{
    class AssetLoader;

    /// Interface for initializing and uninitializing the AssetLoader and related objects.
    class HELIUM_FRAMEWORK_API AssetLoaderInitialization
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~AssetLoaderInitialization();
        //@}

        /// @name AssetLoader Initialization
        //@{
        virtual AssetLoader* Initialize() = 0;
        virtual void Shutdown();
        //@}
    };
}
