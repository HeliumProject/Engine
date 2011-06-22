//----------------------------------------------------------------------------------------------------------------------
// CommandLineInitializationWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_WIN_COMMAND_LINE_INITIALIZATION_WIN_H
#define HELIUM_FRAMEWORK_WIN_COMMAND_LINE_INITIALIZATION_WIN_H

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
        virtual bool Initialize( String& rModuleName, DynArray< String >& rArguments );
        //@}
    };
}

#endif  // HELIUM_FRAMEWORK_WIN_COMMAND_LINE_INITIALIZATION_WIN_H
