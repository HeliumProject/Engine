//----------------------------------------------------------------------------------------------------------------------
// CommandLineInitialization.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_FRAMEWORK_COMMAND_LINE_INITIALIZATION_H
#define LUNAR_FRAMEWORK_COMMAND_LINE_INITIALIZATION_H

#include "Framework/Framework.h"

namespace Lunar
{
    /// Interface for initializing platform command-line parameters.
    class LUNAR_FRAMEWORK_API CommandLineInitialization
    {
    public:
        /// @name Construction/Destruction
        //@{
        virtual ~CommandLineInitialization();
        //@}

        /// @name Command-line Initialization
        //@{
        virtual bool Initialize( String& rModuleName, DynArray< String >& rArguments ) = 0;
        //@}
    };
}

#endif  // LUNAR_FRAMEWORK_COMMAND_LINE_INITIALIZATION_H
