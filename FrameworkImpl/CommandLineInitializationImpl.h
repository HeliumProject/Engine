#pragma once

#include "FrameworkImpl/FrameworkImpl.h"
#include "Framework/CommandLineInitialization.h"

namespace Helium
{
    /// Command-line initialization for Windows.
    class HELIUM_FRAMEWORK_IMPL_API CommandLineInitializationImpl : public CommandLineInitialization
    {
    public:
        /// @name Command-line Initialization
        //@{
        virtual void Startup( String& rModuleName, DynamicArray< String >& rArguments );
        //@}
    };
}
