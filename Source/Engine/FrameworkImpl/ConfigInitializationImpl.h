#pragma once

#include "FrameworkImpl/FrameworkImpl.h"
#include "Framework/ConfigInitialization.h"

namespace Helium
{
    /// Config initialization for Windows.
    class HELIUM_FRAMEWORK_IMPL_API ConfigInitializationImpl : public ConfigInitialization
    {
    public:
        /// @name Config Initialization
        //@{
        virtual void Startup();
        //@}
    };
}
