#pragma once

#include "FrameworkImpl/FrameworkImpl.h"
#include "Framework/AssetLoaderInitialization.h"

namespace Helium
{
    /// AssetLoader initialization implementation for Windows.
    class HELIUM_FRAMEWORK_IMPL_API AssetLoaderInitializationImpl : public AssetLoaderInitialization
    {
    public:
        /// @name AssetLoader Initialization
        //@{
        virtual AssetLoader* Initialize();
        virtual void Shutdown();
        //@}
    };
}
