#pragma once

#include "Framework/Framework.h"

namespace Helium
{
    /// Interface for initializing platform command-line parameters.
    class HELIUM_FRAMEWORK_API CommandLineInitialization
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~CommandLineInitialization();
        //@}

        /// @name Command-line Initialization
        //@{
        virtual bool Initialize( String& rModuleName, DynamicArray< String >& rArguments ) = 0;
        //@}
    };
}
