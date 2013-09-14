#pragma once

#include "FrameworkWin/FrameworkWin.h"
#include "Framework/CommandLineInitialization.h"

namespace Helium
{
    /// Command-line initialization for Windows.
    class HELIUM_FRAMEWORK_WIN_API CommandLineInitializationWin : public CommandLineInitialization
    {
    public:
        /// @name Command-line Initialization
        //@{
        virtual bool Initialize( String& rModuleName, DynamicArray< String >& rArguments );
        //@}
    };
}
