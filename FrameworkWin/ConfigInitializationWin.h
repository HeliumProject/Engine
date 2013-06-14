#pragma once

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/ConfigInitialization.h"

namespace Helium
{
    /// Config initialization for Windows.
    class HELIUM_FRAMEWORK_WIN_API ConfigInitializationWin : public ConfigInitialization
    {
    public:
        /// @name Config Initialization
        //@{
        virtual bool Initialize();
        //@}
    };
}
