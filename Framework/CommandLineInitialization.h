#pragma once

#include "Framework/Framework.h"

namespace Helium
{
    /// Interface for initializing platform command-line parameters.
    class HELIUM_FRAMEWORK_API CommandLineInitialization
    {
    public:
        /// @name Command-line Initialization
        //@{
        virtual void Startup( String& rModuleName, DynamicArray< String >& rArguments ) = 0;
        //@}
    };
}
