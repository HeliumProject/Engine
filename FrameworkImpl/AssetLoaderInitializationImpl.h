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
        virtual void Startup() HELIUM_OVERRIDE;
        virtual void Shutdown() HELIUM_OVERRIDE;
        //@}
    };
}
