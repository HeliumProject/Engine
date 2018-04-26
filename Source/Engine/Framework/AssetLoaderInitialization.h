#pragma once

#include "Framework/Framework.h"

namespace Helium
{
    class AssetLoader;

    /// Interface for initializing and uninitializing the AssetLoader and related objects.
    class HELIUM_FRAMEWORK_API AssetLoaderInitialization
    {
    public:
        /// @name AssetLoader Initialization
        //@{
        virtual void Startup() = 0;
        virtual void Shutdown() = 0;
        //@}
    };
}
