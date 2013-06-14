#pragma once

#include "Framework/Framework.h"

namespace Helium
{
    class WindowManager;

    /// Interface for window manager initialization.
    class HELIUM_FRAMEWORK_API WindowManagerInitialization
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~WindowManagerInitialization();
        //@}

        /// @name Window Manager Initialization
        //@{
        virtual bool Initialize() = 0;
        //@}
    };
}
