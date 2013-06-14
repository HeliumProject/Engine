#pragma once

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/AssetLoaderInitialization.h"

namespace Helium
{
    /// AssetLoader initialization implementation for Windows.
    class HELIUM_FRAMEWORK_WIN_API AssetLoaderInitializationWin : public AssetLoaderInitialization
    {
    public:
        /// @name AssetLoader Initialization
        //@{
        virtual AssetLoader* Initialize();
        virtual void Shutdown();
        //@}
    };
}
